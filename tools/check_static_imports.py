#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""检查产物的静态导入表，找出"可能拖垮启动"的 DLL 依赖。

背景
----
YY-Thunks 的价值在于"新 API 在旧系统上也能用"，其手段是**延迟加载**：
先 LoadLibrary，再 GetProcAddress。因此产物**不应该静态导入**那些
"不一定存在"的 DLL —— 一旦静态导入，缺少该 DLL 的系统会在进程启动阶段
就报"找不到 xxx.dll"，连 main 都进不去。

由于上游用 `#pragma comment(lib, ...)` 表达依赖，而 GCC 会忽略它，移植层是按
模块把系统库列进 CMake 的；只要某个模块的**兜底代码**里直接调用了某个 SDK 函数，
就会产生静态导入。本项目就踩过：目标为 Vista 时 `MFPutWorkItem` 未被 thunk，
导致 `MFPutWorkItem2` 的兜底直接调用了 SDK 版本，于是产物静态依赖 MFPlat.DLL
—— 在 Server Core（不含 Media Foundation）上直接启动失败。

用法
----
  python tools/check_static_imports.py <exe-or-dll> [...]
  python tools/check_static_imports.py build/x64/examples/smoke/yythunks_smoke.exe

退出码：0 = 没有风险依赖；2 = 发现风险依赖。
"""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path

for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")  # type: ignore[attr-defined]
    except Exception:  # pragma: no cover
        pass

# 高风险：在 Server Core / 精简安装 / 旧系统上**确实可能不存在**，
# 一旦静态导入，缺少它的机器会在进程启动阶段直接失败。
HIGH_RISK_DLLS = {
    "mfplat.dll": "Media Foundation 平台层；Server Core 不含",
    "mf.dll": "Media Foundation；Server Core 不含",
    "mfcore.dll": "Media Foundation；Server Core 不含",
    "mfreadwrite.dll": "Media Foundation 读写；Server Core 不含",
    "mfuuid.dll": "Media Foundation",
    "mmdevapi.dll": "音频设备 API；Server Core 可能不含",
    "avrt.dll": "多媒体实时线程",
    "dxgi.dll": "DirectX Graphics Infrastructure；Server Core 图形栈受限",
    "d3d9.dll": "Direct3D 9",
    "d3d10.dll": "Direct3D 10",
    "d3d11.dll": "Direct3D 11",
    "d3d12.dll": "Direct3D 12",
    "d2d1.dll": "Direct2D",
    "dcomp.dll": "DirectComposition",
    "dxva2.dll": "DXVA2 视频加速",
    "dwrite.dll": "DirectWrite",
    "d3dcompiler_47.dll": "D3D 着色器编译器；XP/Vista 默认不含",
    "windowscodecs.dll": "WIC 编解码器；Server Core 可选",
    "winusb.dll": "WinUSB；Server 默认不含",
    "bluetoothapis.dll": "蓝牙；Server 默认不含",
    "esent.dll": "可扩展存储引擎",
    "wlanapi.dll": "WLAN 服务；Server 默认不含",
    "ncrypt.dll": "CNG 密钥存储",
    "uiautomationcore.dll": "UI Automation",
}

# 低风险：属于系统基础组件，Server Core 与各类精简镜像通常都带；
# 只提示，不让 CI 失败。
LOW_RISK_DLLS = {
    "iphlpapi.dll": "IP Helper；系统基础组件，Server Core 带",
    "winhttp.dll": "WinHTTP；系统基础组件，Server Core 带",
    "powrprof.dll": "电源管理；系统基础组件",
    "bcrypt.dll": "CNG 基础加密；Vista 起系统自带",
    "setupapi.dll": "设备安装；系统基础组件",
    "cfgmgr32.dll": "设备配置管理；系统基础组件",
    "pdh.dll": "性能计数器；系统基础组件",
    "wevtapi.dll": "事件日志；系统基础组件",
    "userenv.dll": "用户环境；系统基础组件",
    "credui.dll": "凭据 UI（Server Core 无 GUI 时才是问题）",
    "dwmapi.dll": "DWM；Server Core 无 DWM 时才是问题",
    "uxtheme.dll": "视觉样式；Server Core 不适用",
}

DLL_LINE = re.compile(r"DLL Name:\s*(\S+)")
# objdump 的导入项形如：
#     vma:     Ordinal  Hint  Member-Name  Bound-To
#     0003d8d0  <none>  0000  GetIfEntry2
IMPORT_LINE = re.compile(
    r"^\s+[0-9a-fA-F]+\s+(?:<none>|[0-9a-fA-F]+)\s+[0-9a-fA-F]+\s+(\S+)\s*$")


def objdump() -> str:
    for name in ("x86_64-w64-mingw32-objdump", "i686-w64-mingw32-objdump",
                 "objdump", "llvm-objdump"):
        if shutil.which(name):
            return name
    print("[导入检查] 找不到 objdump，请安装 binutils 或 llvm-tools", file=sys.stderr)
    sys.exit(1)


def parse_imports(tool: str, path: Path) -> "dict[str, list[str]]":
    out = subprocess.run([tool, "-p", str(path)],
                         capture_output=True, text=True, errors="replace").stdout
    result: "dict[str, list[str]]" = {}
    current = None
    for line in out.splitlines():
        m = DLL_LINE.search(line)
        if m:
            current = m.group(1)
            result.setdefault(current, [])
            continue
        if current:
            m = IMPORT_LINE.match(line)
            if m:
                result[current].append(m.group(1))
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description="检查静态导入中的风险 DLL")
    parser.add_argument("binaries", nargs="+", help="要检查的 exe/dll")
    parser.add_argument("--quiet", action="store_true", help="只输出问题")
    args = parser.parse_args()

    tool = objdump()
    failed = False

    for raw in args.binaries:
        path = Path(raw)
        if not path.is_file():
            print(f"[导入检查] 跳过（不存在）：{path}", file=sys.stderr)
            continue

        imports = parse_imports(tool, path)
        if not imports:
            # 静态库、文本文件、路径写错都会走到这里。若静默通过，
            # 就成了一张"虚假的健康证明"，所以这里必须出声。
            print(f"[导入检查] 警告：{path} 未解析到任何导入表条目"
                  f"（不是 PE 映像？），本次检查结果无意义。", file=sys.stderr)
            continue

        high = {d: s for d, s in imports.items() if d.lower() in HIGH_RISK_DLLS}
        low = {d: s for d, s in imports.items() if d.lower() in LOW_RISK_DLLS}

        # --quiet：干净的产物完全不输出，只有发现问题时才打印。
        verbose = not args.quiet

        if verbose or high:
            print(f"\n=== {path} ===")
            print(f"静态导入共 {len(imports)} 个 DLL")

        if verbose:
            for dll, syms in sorted(low.items()):
                print(f"  [低风险] {dll} —— {LOW_RISK_DLLS[dll.lower()]}"
                      f"（{len(syms)} 个符号）")

        if not high:
            if verbose:
                print("未发现高风险 DLL 依赖 ✓")
            continue

        failed = True
        print(f"\n发现 {len(high)} 个高风险 DLL —— 缺少时会在**进程启动阶段**直接失败：")
        for dll, syms in sorted(high.items()):
            print(f"  [!] {dll} —— {HIGH_RISK_DLLS[dll.lower()]}")
            for s in sorted(syms)[:12]:
                print(f"        {s}")
            if len(syms) > 12:
                print(f"        ...（共 {len(syms)} 个）")
        print("\n  修法：若某个 thunk 的**兜底分支**直接调用了同族的原生 API，"
              "就会产生静态导入。\n"
              "        可在 port/transforms.py 里用 kind=\"force_thunk\" 把该 thunk "
              "强制启用，\n"
              "        使其改走本库的延迟加载路径（参考 MFPutWorkItem / CreateDXGIFactory）。")

    return 2 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
