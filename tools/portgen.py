#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""YY-Thunk-gnu 源码移植生成器。

上游 YY-Thunks 只支持 MSVC，本脚本在 **不修改上游源码** 的前提下，
依据 `portrule/thunks.manifest` 与 `tools/transforms.py` 生成一套 GCC(MinGW-w64)
可编译的"覆盖层"源码树。

子命令
------
  all       依次执行 manifest 校验 + list + features + overlay（默认）
  list      生成 YY_Thunks_List.hpp
  features  生成 YY_Thunks_Features.hpp
  overlay   依据转换规则物化覆盖层源码
  manifest  从上游 vcxproj 刷新模块清单
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

# 让中文日志在 Windows 控制台/CI 上也能正确输出
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")  # type: ignore[attr-defined]
    except Exception:  # pragma: no cover
        pass

# --------------------------------------------------------------------------- #
# 基础工具
# --------------------------------------------------------------------------- #

IDENT_RE = re.compile(r"^[A-Za-z_]\w*$")


def die(msg: str) -> "None":
    print(f"[portgen][错误] {msg}", file=sys.stderr)
    sys.exit(1)


def warn(msg: str) -> None:
    print(f"[portgen][警告] {msg}", file=sys.stderr)


def info(msg: str) -> None:
    print(f"[portgen] {msg}")


def read_text(path: Path) -> str:
    # 上游源码统一为 UTF-8（无 BOM），个别文件带 BOM，用 utf-8-sig 容错
    return path.read_text(encoding="utf-8-sig")


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    # 保持与上游一致的换行风格（CRLF），避免生成物与上游 diff 噪音过大
    data = text.replace("\r\n", "\n").replace("\n", "\r\n")
    path.write_text(data, encoding="utf-8", newline="")


def strip_comments_and_literals(text: str) -> str:
    """把注释与字符串/字符字面量替换为等长空白，保留位置以便做括号配对。"""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join("\n" if ch == "\n" else " " for ch in text[i:j]))
            i = j
        elif c in "\"'":
            quote = c
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == quote:
                    j += 1
                    break
                j += 1
            out.append("".join("\n" if ch == "\n" else " " for ch in text[i:j]))
            i = j
        else:
            out.append(c)
            i += 1
    return "".join(out)


def match_brace(text: str, open_idx: int) -> int:
    """返回与 text[open_idx]（'{'）配对的 '}' 下标；找不到返回 -1。"""
    depth = 0
    for i in range(open_idx, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return i
    return -1


def split_top_level_args(s: str) -> "list[str]":
    """按顶层逗号切分宏实参（忽略括号/中括号/花括号内部的逗号）。"""
    args, depth, cur = [], 0, []
    for ch in s:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            args.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    args.append("".join(cur))
    return args


# --------------------------------------------------------------------------- #
# 清单
# --------------------------------------------------------------------------- #

def load_manifest(manifest_path: Path, exclude_path: "Path | None" = None) -> "tuple[list[str], list[str]]":
    if not manifest_path.is_file():
        die(f"找不到模块清单：{manifest_path}")
    modules = []
    for raw in read_text(manifest_path).splitlines():
        line = raw.strip()
        if not line or line.startswith("#") or line.startswith("-"):
            continue
        modules.append(line.replace("\\", "/"))
    if not modules:
        die(f"模块清单为空：{manifest_path}")
    dup = {m for m in modules if modules.count(m) > 1}
    if dup:
        die(f"模块清单存在重复项：{sorted(dup)}")

    excluded: "list[str]" = []
    if exclude_path is not None and exclude_path.is_file():
        for raw in read_text(exclude_path).splitlines():
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            excluded.append(line.replace("\\", "/"))

    unknown = [m for m in excluded if m not in modules]
    if unknown:
        die(f"排除清单中的模块不在清单里（上游可能已重命名）：{unknown}")

    kept = [m for m in modules if m not in set(excluded)]
    if not kept:
        die("排除后没有任何模块可参与编译")
    return kept, excluded


def refresh_manifest(upstream_thunks: Path, manifest_path: Path) -> None:
    vcxproj = upstream_thunks.parent / "YY-Thunks.UnitTest" / "YY-Thunks.UnitTest.vcxproj"
    if not vcxproj.is_file():
        die(f"找不到上游工程文件：{vcxproj}")
    text = vcxproj.read_text(encoding="utf-8-sig")
    order = re.findall(r'<ClInclude\s+Include="\.\.\\Thunks\\([^"]+?\.hpp)"\s*/>', text)
    if not order:
        die("未能从上游 vcxproj 解析出 Thunks 头文件清单，上游结构可能已变化")
    header = (
        "# YY-Thunks 参与构建的 Thunk 模块清单（有序）\n"
        "#\n"
        "# 本文件由 `python tools/portgen.py manifest --refresh` 从上游 vcxproj 生成。\n"
        "# 顺序即上游登记顺序，会影响单翻译单元内的声明/定义顺序，请勿手工调整。\n"
        "#\n"
        "# 可以通过在行首加 `-` 或删除对应行来排除尚未完成移植的模块。\n\n"
    )
    write_text(manifest_path, header + "\n".join(order) + "\n")
    info(f"已刷新模块清单：{manifest_path}（{len(order)} 个模块）")


# --------------------------------------------------------------------------- #
# 清单 -> YY_Thunks_List.hpp
# --------------------------------------------------------------------------- #

LIST_HEADER = """// 本文件由 tools/portgen.py 自动生成，请勿手工修改！
// 生成依据：portrule/thunks.manifest
//
// 上游 MSVC 工程通过 MSBuild Target 从 vcxproj 的 ClInclude 顺序生成此文件；
// 移植后改由 CMake + portgen.py 生成，顺序语义保持一致。

"""


def gen_list(modules: "list[str]", out_file: Path) -> None:
    lines = [f'#include "{m}"' for m in modules]
    write_text(out_file, LIST_HEADER + "\n".join(lines) + "\n")
    info(f"已生成 Thunk 清单头：{out_file}（{len(modules)} 个模块）")


# --------------------------------------------------------------------------- #
# 清单 -> YY_Thunks_Features.hpp
# --------------------------------------------------------------------------- #

def collect_thunks(thunks_dir: Path, modules: "list[str]") -> "tuple[dict[str, str], set[str]]":
    """返回 (函数名 -> 所在模块, 拥有 Fallback 解析器的函数名集合)。"""
    functions: "dict[str, str]" = {}
    fallback: "set[str]" = set()

    for module in modules:
        path = thunks_dir / module
        if not path.is_file():
            die(f"清单中的模块在上游不存在：{module}（上游可能已重命名/删除）")
        raw = read_text(path)
        clean = strip_comments_and_literals(raw)

        # 1) 收集所有 __DEFINE_THUNK 调用（含被 #if 排除者，保证映射宏总是有定义）
        for m in re.finditer(r"__DEFINE_THUNK\s*\(", clean):
            open_idx = clean.index("(", m.start())
            depth, end = 0, -1
            for i in range(open_idx, len(clean)):
                if clean[i] == "(":
                    depth += 1
                elif clean[i] == ")":
                    depth -= 1
                    if depth == 0:
                        end = i
                        break
            if end < 0:
                die(f"{module}: __DEFINE_THUNK 实参括号不匹配")
            args = split_top_level_args(clean[open_idx + 1:end])
            if len(args) < 5:
                die(f"{module}: __DEFINE_THUNK 实参少于 5 个")
            name = args[4].strip()
            if not IDENT_RE.match(name):
                die(f"{module}: 无法解析 __DEFINE_THUNK 函数名：{name!r}")
            functions.setdefault(name, module)

        # 2) 收集 YY::Thunks::Fallback 命名空间内定义的 try_get_<X> 解析器
        for m in re.finditer(r"namespace\s+YY::Thunks::Fallback\s*(::\w+)?\s*\{", clean):
            if m.group(1):
                continue  # Fallback::PowrProf 之类的子命名空间不属于直接映射目标
            end = match_brace(clean, clean.index("{", m.end() - 1))
            if end < 0:
                die(f"{module}: Fallback 命名空间花括号不匹配")
            block = clean[m.end():end]
            for r in re.finditer(r"try_get_(\w+)\s*\(\s*(?:const\s+)?ProcInfo\s*&", block):
                fallback.add(r.group(1))

    return functions, fallback


FEATURES_HEADER = """// 本文件由 tools/portgen.py 自动生成，请勿手工修改！
//
// 上游通过 MSVC 专有扩展 `__if_exists(YY::Thunks::Fallback::try_get_X)` 判断
// 某个 API 是否存在自定义解析器。GCC 没有该扩展，这里改为一张显式的
// 映射表：每个受支持的 thunk 都有一个 YY_THUNK_RESOLVER_<函数名> 宏。

#pragma once

"""


def gen_features(thunks_dir: Path, modules: "list[str]", out_file: Path,
                 enabled: "set[str] | None" = None,
                 extra_names: "set[str] | None" = None) -> None:
    functions, fallback = collect_thunks(thunks_dir, modules)
    names = set(functions)
    if enabled is not None:
        names |= set(enabled)
    if extra_names:
        names |= set(extra_names)

    lines = []
    for name in sorted(names):
        if name in fallback:
            lines.append(
                f"#define YY_THUNK_RESOLVER_{name} &YY::Thunks::Fallback::try_get_{name}"
            )
        else:
            lines.append(f"#define YY_THUNK_RESOLVER_{name} nullptr")
    write_text(out_file, FEATURES_HEADER + "\n".join(lines) + "\n")
    info(
        f"已生成特性映射头：{out_file}"
        f"（{len(names)} 个条目，其中 {len(fallback & names)} 个带 Fallback 解析器）"
    )


LATE_BOUND_RE = re.compile(r"_APPLY\(\s*([A-Za-z_]\w*)\s*,\s*([A-Za-z_]\w*)\s*\)")


def collect_late_bound_functions(thunks_dir: Path) -> "set[str]":
    """解析 YY_Thunks.cpp 中 _YY_APPLY_TO_LATE_BOUND_FUNCTIONS 登记的函数名。

    这些函数不是 __DEFINE_THUNK thunk，但 YY_Thunks.h 的宏同样会为它们
    展开 YY_THUNK_RESOLVER_<名字>，所以映射表必须一并覆盖。
    """
    path = thunks_dir / "YY_Thunks.cpp"
    if not path.is_file():
        die(f"找不到上游入口文件：{path}")
    clean = strip_comments_and_literals(read_text(path))
    names = set()
    for m in LATE_BOUND_RE.finditer(clean):
        # 排除模块表（第二个参数是字符串字面量，正则本身已过滤，这里再保险一次）
        names.add(m.group(1))
    return names


# --------------------------------------------------------------------------- #
# 预处理探测：确定哪些 thunk 真正参与编译
# --------------------------------------------------------------------------- #

PROBE_SRC = """// 本文件由 tools/portgen.py 自动生成，仅用于预处理探测，不参与编译。
//
// 作用：借用真实编译器的预处理器，精确得到
//   (a) 当前 YY_Thunks_Target / 编译选项下每个 thunk 的实际签名；
//   (b) 哪些 thunk 真正参与编译。
//
// 上游 YY_Thunks.cpp 用"两遍包含"实现声明与定义分离，并用 MSVC 专有的
// __if_not_exists 在第一遍里"跳过函数体"。这里把第一遍(声明)整体挪到
// 预处理阶段一次生成（YY_Thunks_Declarations.hpp），从而：
//   - 不再需要 __if_exists / __if_not_exists；
//   - 不再需要"吞噬函数体"的模板技巧；
//   - 声明与定义始终来自同一份宏实参，天然不会漂移。
#include <yy_thunks_prelude.h>

#define YY_Thunks_Implemented 1

#undef __DEFINE_THUNK
#define __DEFINE_THUNK(_Module, _Size, _Return, _Conv, _Function, ...)                    \\
    YYTHUNKDECL_BEGIN                                                                    \\
    __APPLY_UNIT_TEST_BOOL(_Function);                                                   \\
    typedef _Return(_Conv* _CRT_CONCATENATE(YY_THUNK_FNTYPE_, _Function))(__VA_ARGS__);  \\
    EXTERN_C _Return _Conv _CRT_CONCATENATE_(__FALLBACK_PREFIX, _Function)(__VA_ARGS__); \\
    static _CRT_CONCATENATE(YY_THUNK_FNTYPE_, _Function) __cdecl                         \\
        _CRT_CONCATENATE(try_get_, _Function)() noexcept;                                \\
    YYTHUNKDECL_END

#include "YY_Thunks_List.hpp"
"""

PROBE_BLOCK = re.compile(r"YYTHUNKDECL_BEGIN([\s\S]*?)YYTHUNKDECL_END")
TRY_GET_NAME = re.compile(r"\btry_get_([A-Za-z_]\w*)\s*\(")
# 守卫表自校验用的一次性标记（见 probe_thunks）
GUARD_MARKER = re.compile(r"YYGUARD_ON_(\w+)")

ENABLED_HEADER = """// 本文件由 tools/portgen.py 自动生成，请勿手工修改！
//
// 记录"哪些 thunk 在当前 YY_Thunks_Target 与编译选项下真正参与编译"。
// 上游用 MSVC 专有扩展 `__if_exists(YY::Thunks::try_get_X)` 来表达这个判断，
// GCC 没有该扩展，这里改由预处理探测精确产出：
//
//     #if defined(YY_THUNK_HAS_try_get_CloseHandle)
//         const auto CloseHandle = YY::Thunks::try_get_CloseHandle();
//     #endif
//
// 未参与编译的 thunk 不会产生对应宏（等价于 __if_exists 为假）。

#pragma once

"""

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]', re.M)


# 这些头文件不能被提前包含：
#   - knownfolders/shlguid/initguid/devguid 没有 include guard，提前包含会重复定义 GUID；
#   - YY-Thunks 自己的内部头（Shared/*.h、Thunks/*.h）依赖 TU 内的定义顺序，
#     提前到"声明头"位置会因为前置声明未就绪而编译失败。
_PREFIX_INCLUDE_DENY = {
    "knownfolders.h",
    "shlguid.h",
    "initguid.h",
    "devguid.h",
    "interlockedqueue.h",
    "hookthunk.h",
    "smbios.h",
    "hstringprivate.h",
    "km.h",
    "shareddefs.h",
    "list.h",
    "threadrunner.h",
    "yy_thunks.h",
    "yy_thunks_list.hpp",
    "yy_thunks_features.hpp",
    "yy_thunks_enabled.hpp",
}


def collect_sdk_includes(thunks_dir: Path, modules: "list[str]") -> "list[str]":
    """收集各 thunk 头依赖的 SDK/自身头文件（保序去重）。

    上游的"第一遍(声明)"是紧跟在每个 thunk 头自身的 #include 之后展开的，
    所以声明里可以直接使用 PACKAGE_INFO_REFERENCE / MIB_IF_ROW2 这类 SDK 类型。
    移植层把声明集中成一个头，就必须把这些 #include 一并前置，否则类型不可见。
    """
    seen: "set[str]" = set()
    ordered: "list[str]" = []
    for module in modules:
        path = thunks_dir / module
        if not path.is_file():
            continue
        for m in INCLUDE_RE.finditer(read_text(path)):
            name = m.group(2)
            if name.lower() in _PREFIX_INCLUDE_DENY:
                continue
            line = f"#include {m.group(1)}{name}{'>' if m.group(1) == '<' else '\"'}"
            if line in seen:
                continue
            seen.add(line)
            ordered.append(line)
    return ordered


DECLS_HEADER = """// 本文件由 tools/portgen.py 自动生成，请勿手工修改！
//
// 上游 Thunks/YY_Thunks.cpp 会先以"只声明不定义"的方式把 YY_Thunks_List.hpp
// 包含一遍（第一遍），以便任意 thunk 头都能调用其它 thunk 的 try_get_*。
// 本文件就是那一遍的产物：所有参与编译的 thunk 的
//     - API 函数声明
//     - 函数类型 typedef（YY_THUNK_FNTYPE_<Api>）
//     - static try_get_<Api>() 前置声明
//     - 单元测试用的探针变量
// 全部由预处理探测一次性生成，顺序与上游登记顺序一致。
//
// 关于 typedef：上游用 decltype(函数名) 取函数类型。这在 MSVC 下成立，
// 但 MinGW 的 pathcch.h 等头会额外提供同名的 C++ 内联重载
// （如 PathIsUNCEx(PWSTR, PWSTR*)），与 extern "C" 版本共同构成重载集，
// 使 decltype / 取址产生二义。改为显式 typedef 后取址与类型推导都不再依赖重载解析。
//
// 注意：因为由预处理器展开，这里的类型/调用约定已经展开为最终形式；
// 生成时使用的宏环境必须与真实编译一致（CMake 已保证）。

#pragma once

"""


def probe_thunks(decls_file: Path, probe_src: Path,
                 thunks_dir: Path, modules: "list[str]",
                 cxx: str, cxx_flags: "list[str]",
                 include_dirs: "list[str]",
                 guard_table_file: "Path | None" = None,
                 module_search_dirs: "list[Path] | None" = None) -> "set[str]":
    """用真实编译器的预处理器推断 thunk 签名与启用集合。

    额外职责（给了 guard_table_file 时）
    -----------------------------------
    1. 生成与目标无关的守卫表 `YY_Thunks_EnableTable.hpp` —— 纯文本解析得到，
       见 collect_thunk_guards；
    2. 在**同一宏环境**下把该表逐条求值，与真实探测出的启用集合比对，
       不一致就点名报出。这样"解析出来的守卫"始终被真实预处理器盯着，
       不会静默偏离上游。
    """
    import shlex
    import subprocess
    import tempfile

    # 模块文件优先取覆盖层（转换后），找不到再退回上游原文
    search_dirs = list(module_search_dirs) if module_search_dirs else [thunks_dir]
    module_files: "dict[str, Path]" = {}
    for module in modules:
        for candidate_dir in search_dirs:
            candidate = candidate_dir / module
            if candidate.is_file():
                module_files[module] = candidate
                break

    guards = collect_thunk_guards(module_files)

    probe_text = PROBE_SRC
    if guard_table_file is not None:
        write_text(guard_table_file, ENABLE_TABLE_HEADER + "\n".join(
            f"#define YY_THUNK_GUARD_try_get_{name} {guard}"
            for name, guard in sorted(guards.items())) + "\n")
        info(f"已生成目标无关的守卫表：{guard_table_file}（{len(guards)} 条）")
        # 逐条求值：标记只在本行守卫为真时才会出现在预处理输出里
        probe_text += "\n// ---- 守卫表自校验（由 portgen 生成，勿手工修改）----\n"
        probe_text += '#include "YY_Thunks_EnableTable.hpp"\n'
        for name in sorted(guards):
            probe_text += (f"#if YY_THUNK_GUARD_try_get_{name}\n"
                           f"YYGUARD_ON_{name}\n"
                           f"#endif\n")

    write_text(probe_src, probe_text)
    with tempfile.TemporaryDirectory() as tmp:
        out_i = Path(tmp) / "probe.i"
        cmd = [cxx, "-E", "-P", "-x", "c++", "-std=c++17"]
        cmd += [f"-I{d}" for d in include_dirs]
        cmd += shlex.split(" ".join(cxx_flags))
        cmd += [str(probe_src), "-o", str(out_i)]
        info("预处理探测：" + " ".join(cmd))
        proc = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
        if proc.returncode != 0:
            print(proc.stderr[-4000:], file=sys.stderr)
            die("预处理探测失败，无法确定 thunk 签名与启用集合")
        text = read_text(out_i) if out_i.is_file() else ""

    blocks = PROBE_BLOCK.findall(text)
    if not blocks:
        die("预处理探测未匹配到任何 thunk，请检查 include 路径/编译选项是否与真实编译一致")

    decls: "list[str]" = []
    names: "list[str]" = []
    seen: "set[str]" = set()
    for block in blocks:
        hits = TRY_GET_NAME.findall(block)
        if not hits:
            die(f"探测到的声明块中找不到 try_get_*：{block.strip()[:200]!r}")
        name = hits[-1]
        if name in seen:
            # 上游第一遍用 __if_not_exists 跳过重复声明，这里做同样的去重
            continue
        seen.add(name)
        names.append(name)
        decls.append(block.strip())

    if guard_table_file is not None:
        predicted = set(GUARD_MARKER.findall(text))
        actual = set(names)
        # 守卫表说"不启用"、真实探测说启用 —— 漏报会让调用点走错分支
        missing = sorted(actual - predicted)
        # 守卫表说"启用"、真实探测说没有 —— 多报会让调用点引用未定义的符号
        surplus = sorted(predicted - actual)
        if missing or surplus:
            warn(f"守卫表与真实探测不一致：漏报 {len(missing)} 个、多报 {len(surplus)} 个")
            for tag, items in (("漏报", missing), ("多报", surplus)):
                for name in items[:10]:
                    warn(f"  [{tag}] {name}    守卫 = {guards.get(name, '?')}")
                if len(items) > 10:
                    warn(f"  [{tag}] ...（其余 {len(items) - 10} 个略）")
        else:
            info(f"守卫表自校验通过：{len(guards)} 条守卫求值结果与真实探测的 "
                 f"{len(actual)} 个启用 thunk 完全一致")

    sdk_includes = collect_sdk_includes(thunks_dir, modules)

    # 上游所有 __DEFINE_THUNK 都位于 namespace YY::Thunks 内；
    # 预处理器输出会把命名空间"拍平"，因此必须显式补回来，
    # 否则声明会落到全局作用域，与定义处形成重载/冲突。
    # 声明遍：每条声明都包进它自己的守卫里，使本文件**与目标无关**。
    #
    # 为什么可以这样：本文件由"最低目标"下的探测生成（见 CMakeLists 的
    # YY_THUNKS_PORTGEN_TARGET_MACRO）。因为启用条件是单调阈值，所以
    #     （所有目标的启用集合的并集）
    #   = （最低目标下的启用集合）
    # 于是最低目标下探测出的声明集合是任何目标的超集。再加一层逐条守卫，
    # 每个目标编译时只会激活自己需要的那部分 —— 与"按目标各生成一份"等价，
    # 但本文件只需要生成一次，所有目标共用。
    decls_text = DECLS_HEADER
    decls_text += "// ---- 各 thunk 头依赖的 SDK / 共享头（保证声明中的类型可见）----\n"
    decls_text += "\n".join(sdk_includes) + "\n\n"
    decls_text += "namespace YY::Thunks\n{\n"

    wrapped: "list[str]" = []
    unguarded: "list[str]" = []
    for name, decl in zip(names, decls):
        guard = guards.get(name)
        if guard is None:
            unguarded.append(name)
            wrapped.append(decl)
        elif guard == "1":
            wrapped.append(decl)
        else:
            wrapped.append(f"#if YY_THUNK_GUARD_try_get_{name}\n{decl}\n#endif")
    decls_text += "\n\n".join(wrapped)
    decls_text += "\n} // namespace YY::Thunks\n"
    write_text(decls_file, decls_text)

    if unguarded:
        warn(f"{len(unguarded)} 个 thunk 在守卫表里找不到对应条目，其声明将不受守卫保护："
             + ", ".join(unguarded[:8]))
    info(f"已生成 thunk 声明头：{decls_file}"
         f"（{len(decls)} 个声明，其中 {len(decls) - len(unguarded)} 个带守卫）")

    # 不再产出 per-target 的"启用集合头"：调用点已改用目标无关的守卫表，
    # 这里的启用集合只用于与守卫表比对（见上）。本行仅报告规模。
    info(f"本目标下 {len(names)} 个 thunk 参与编译")
    return set(names)


# --------------------------------------------------------------------------- #
# 转换规则 -> 覆盖层
# --------------------------------------------------------------------------- #

DEFINE_THUNK_OPEN = re.compile(r"^\s*__DEFINE_THUNK\(?\s*$|^\s*__DEFINE_THUNK\(")


def _split_top_level(text: str) -> "list[str]":
    """按顶层逗号切分宏实参（忽略括号内的逗号）。"""
    parts, depth, cur = [], 0, []
    for ch in text:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    parts.append("".join(cur))
    return [p.strip() for p in parts]


def transform_force_thunk(text: str, api: str) -> "tuple[str, int]":
    """把包住指定 thunk 的 `#if` 守卫改成恒真，使该 thunk 始终参与编译。

    为什么需要
    ----------
    上游用 `#if (YY_Thunks_Target < __WindowsNT6)` 表达"这个 API 从 Vista 起就是
    原生的，只有更低版本才需要兜底"。问题是：当 `YY_Thunks_Target` **恰好等于
    Vista** 时，包在同一个文件里、守卫为 `< __WindowsNT6_2` 的**兄弟 thunk**
    仍然会编译，而它的兜底分支会**直接调用**这个"Vista 原生就有"的 API：

        __DEFINE_THUNK(mfplat, 16, HRESULT, STDAPICALLTYPE, MFPutWorkItem2, ...)
        {
            if (auto _pfn = try_get_MFPutWorkItem2()) return _pfn(...);
            return MFPutWorkItem(...);        // ← 解析到 SDK 导入，而不是 thunk
        }

    于是产物**静态导入**了 mfplat.dll / dxgi.dll —— 缺少该 DLL 的系统（例如不含
    Media Foundation 的 Server Core）会在**进程启动阶段**直接失败，连 main 都进不去，
    哪怕应用根本没用到这些 API。

    强制该 thunk 参与编译后，`MFPutWorkItem(...)` 会解析到本库的 thunk，
    由它负责 LoadLibrary + GetProcAddress（延迟加载）；静态导入随之消失，
    DLL 只会在真正调用时才被加载。

    实现：定位 `__DEFINE_THUNK(...)` 中第 6 个实参等于 `api` 的那一处，
    向前跳过空行/注释，把最近的 `#if` 守卫替换为 `#if 1`。
    """
    lines = text.splitlines(keepends=True)
    out = list(lines)
    hits = 0

    for i, line in enumerate(lines):
        if not DEFINE_THUNK_OPEN.match(line):
            continue
        # 收集宏实参（从下一行开始，到与之配对的右括号为止）
        depth = 1
        buf = []
        j = i
        while j + 1 < len(lines) and depth > 0:
            j += 1
            seg = lines[j]
            buf.append(seg)
            depth += seg.count("(") - seg.count(")")
        # __DEFINE_THUNK(_Module, _Size, _Return, _Conv, _Function, ...)
        # 函数名是第 5 个实参（下标 4）
        args = _split_top_level("\n".join(buf).rstrip().rstrip(")"))
        if len(args) < 5 or args[4].strip() != api:
            continue

        # 向前找到最近的 #if（允许中间只有空行与注释）
        k = i - 1
        while k >= 0:
            probe = lines[k].strip()
            if not probe or probe.startswith("//") or probe.startswith("/*"):
                k -= 1
                continue
            break
        if k < 0 or not lines[k].lstrip().startswith("#if"):
            warn(f"{api}: __DEFINE_THUNK 之前没有找到 #if 守卫，跳过强制启用")
            continue

        original = lines[k].strip()
        out[k] = (f"#if 1 /* portgen 强制启用 {api}：避免兜底调用退化成 SDK 静态导入"
                  f"（原条件 {original}） */\n")
        hits += 1

    if hits == 0:
        return text, 0
    return "".join(out), hits


ENABLE_TABLE_HEADER = """// 本文件由 tools/portgen.py 自动生成，请勿手工修改！
//
// 与 YY_Thunks_Enabled.hpp 的区别（重要）：
//   Enabled.hpp    —— "**当前**目标下哪些 thunk 参与编译"，是探测出的布尔集合，
//                     换目标必须重新生成。
//   本文件         —— "哪些 thunk 在**什么条件下**参与编译"，是守卫表达式，
//                     与目标无关，生成一次即可供所有目标使用。
//
// 上游用 `#if (YY_Thunks_Target < __WindowsNT6_2)` 表达"这个 API 从 Win8 起
// 原生就有，只有更低版本才需要兜底"。而 __Version 本身是预处理器算术表达式，
// 所以守卫可以直接在 `#if` 中求值：
//
//     #if YY_THUNK_GUARD_try_get_MFPutWorkItem2
//         ... 该 thunk 在本目标下参与编译 ...
//     #endif
//
// 于是 `-DYY_Thunks_Target=...` 一个编译选项就决定了全部目标差异，
// 与上游（MSVC 的 __if_exists）的行为模型一致。
//
// 校验：portgen 的预处理探测会在**同一宏环境**下逐条求值本文件，
// 与真实探测出的启用集合比对，不一致会报出具体 thunk。

#pragma once

"""

GUARD_DIRECTIVE = re.compile(
    r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b\s*(.*?)\s*$")


def _clause_condition(clause: str, expr: str) -> str:
    """把一条 #if / #ifdef / #ifndef 子句转成可被预处理器求值的条件；#else 无正条件。"""
    if clause == "if":
        return f"({expr})"
    if clause == "ifdef":
        return f"defined({expr})"
    if clause == "ifndef":
        return f"!defined({expr})"
    return "1"


def _level_condition(level: "list[tuple[str, str]]") -> str:
    """求一条 #if / #elif / #else 链在"当前分支"下的条件。

    分支互斥，所以当前分支的条件 =（前面各分支条件取反）且（本分支条件）。
    例如 `#if A` / `#elif B`，thunk 落在 #elif 里 → `!(A) && (B)`。

    #elif / #else 是**替换当前层**而不是压入新层 —— 这一点弄错会让
    # `#endif` 弹出的层不对，守卫栈永久错位（曾把 user32 的 DPI 系列守卫
    // 串成包含两个 `defined(_X86_)` 的怪式子）。
    """
    parts = []
    for index, (clause, expr) in enumerate(level):
        condition = _clause_condition(clause, expr)
        if index + 1 < len(level):
            parts.append(f"!({condition})")   # 前面分支都不成立
        elif condition != "1":
            parts.append(condition)           # 本分支的条件
    return " && ".join(parts)


def collect_thunk_guards(module_files: "dict[str, Path]") -> "dict[str, str]":
    """为每个 thunk 提取包住其 __DEFINE_THUNK 的守卫表达式。

    返回 ( {thunk 名: 可被预处理器求值的守卫表达式}, [无法可靠表达的 thunk] )。

    为什么要这一步
    --------------
    现在"某个 thunk 是否启用"是**探测**出来的布尔集合，所以每换一个目标都要
    重新探测、重新生成整套头文件。改成守卫表达式后，这份表与目标无关：
    编译期由 `-DYY_Thunks_Target=...` 求值决定，生成一次即可。

    实现：正向扫描 + 守卫栈。遇到 __DEFINE_THUNK 时，栈里各层的"当前分支条件"
    的合取就是启用条件。`#elif` / `#else` 由 _level_condition 处理成
    "前面分支取反 且 本分支" 的形式，所以任何分支结构都能表达。

    注意：必须读**覆盖层**（转换后）的文件 —— force_thunk 会把某些守卫
    改写成 `#if 1`，从上游原文读会得到过期的守卫。
    """
    guards: "dict[str, str]" = {}

    for rel in sorted(module_files):
        path = module_files[rel]
        # 每个元素是一条 #if/#elif/#else 条件链
        stack: "list[list[tuple[str, str]]]" = []
        lines = read_text(path).splitlines()
        i = 0
        while i < len(lines):
            m = GUARD_DIRECTIVE.match(lines[i])
            if m:
                kind, expr = m.group(1), m.group(2).strip()
                if kind == "endif":
                    if stack:
                        stack.pop()
                elif kind in ("elif", "else"):
                    # 替换当前层，而不是压入新层
                    if stack:
                        stack[-1].append((kind, expr))
                    else:
                        stack.append([(kind, expr)])
                else:
                    stack.append([(kind, expr)])
                i += 1
                continue

            if DEFINE_THUNK_OPEN.match(lines[i]):
                # 跨行收集 __DEFINE_THUNK 的实参，并跳过整段
                depth, buf, j = 1, [], i
                while j + 1 < len(lines) and depth > 0:
                    j += 1
                    buf.append(lines[j])
                    depth += lines[j].count("(") - lines[j].count(")")
                args = _split_top_level("\n".join(buf).rstrip().rstrip(")"))
                if len(args) >= 5:
                    name = args[4].strip()
                    conditions = [_level_condition(level) for level in stack]
                    guards[name] = " && ".join(c for c in conditions if c) or "1"
                i = j
            i += 1

    return guards


SEH_TOKEN = re.compile(r"\b(__try|__except|__finally|__leave)\b")


def transform_seh(text: str) -> "tuple[str, int]":
    """把 MSVC 的 SEH 语法降级为普通语句。

    - `__try { A }`                        -> `{ A }`
    - `__except (FILTER) { B }`            -> `if (false) { B }`
    - `__finally { B }` / `__leave`        -> `if (false) { B }` / 空

    正常路径语义与上游完全一致；异常路径不再被捕获（已知限制）。
    """
    out: "list[str]" = []
    pos, count, n = 0, 0, len(text)
    while True:
        m = SEH_TOKEN.search(text, pos)
        if not m:
            out.append(text[pos:])
            break
        out.append(text[pos:m.start()])
        token = m.group(1)
        pos = m.end()
        count += 1
        if token == "__try":
            out.append("/*__try*/")
        elif token == "__except":
            # 连同后面的过滤表达式一起吞掉（括号可能嵌套）
            j = pos
            while j < n and text[j].isspace():
                j += 1
            if j < n and text[j] == "(":
                depth, k = 0, j
                while k < n:
                    if text[k] == "(":
                        depth += 1
                    elif text[k] == ")":
                        depth -= 1
                        if depth == 0:
                            break
                    k += 1
                pos = k + 1
            out.append("/*__except*/ if (false)")
        elif token == "__finally":
            out.append("/*__finally*/ if (false)")
        else:  # __leave
            out.append("/*__leave*/")
    return "".join(out), count


def apply_rule(rel: str, text: str, rule: dict) -> "tuple[str, int]":
    kind = rule["kind"]
    if kind == "seh":
        return transform_seh(text)
    if kind == "force_thunk":
        return transform_force_thunk(text, rule["pattern"].strip())
    pattern = rule["pattern"]
    repl = rule["repl"]
    if kind == "literal":
        count = text.count(pattern)
        if count:
            text = text.replace(pattern, repl)
        return text, count
    if kind == "regex":
        text, count = re.subn(pattern, repl, text)
        return text, count
    die(f"未转换规则类型：{kind}")
    return text, 0


def gen_overlay(upstream_src: Path, out_root: Path, transforms: "list[dict]",
                overrides_dir: Path,
                mirror_dirs: "list[str] | None" = None) -> "dict":
    touched: "dict[str, list[dict]]" = {}

    for rule in transforms:
        for rel in rule["files"]:
            touched.setdefault(rel, []).append(rule)

    # 自足模式：把指定目录下的**全部**文件纳入物化范围（没有规则的也逐字复制）。
    #
    # 为什么需要：默认只物化"规则命中"的文件，其余靠 include 路径回退到上游读原文。
    # 这让生成目录很小（17 个文件），但也意味着**编译必须带着上游源码** —— 实测
    # 编译会从上游读 72 个头文件，其中包括 ThreadRunner.h 这种不在模块清单里的。
    # 整个目录复制过来之后，生成目录就是一棵完整可读的源码树，编译不再需要上游。
    mirrored: "set[str]" = set()
    for d in mirror_dirs or []:
        base = upstream_src / d
        if not base.is_dir():
            die(f"自足模式：上游目录不存在：{base}")
        for p in base.rglob("*"):
            if p.is_file():
                rel = p.relative_to(upstream_src).as_posix()
                mirrored.add(rel)
                touched.setdefault(rel, [])

    overrides: "dict[str, Path]" = {}
    if overrides_dir.is_dir():
        for p in overrides_dir.rglob("*"):
            if p.is_file():
                overrides[p.relative_to(overrides_dir).as_posix()] = p
                touched.setdefault(p.relative_to(overrides_dir).as_posix(), [])

    report = {"materialized": [], "skipped": [], "applied": [], "drift": []}
    for rel in sorted(touched):
        src = upstream_src / rel
        if not src.is_file():
            report["drift"].append({"file": rel, "reason": "上游文件不存在"})
            warn(f"上游文件不存在：{rel}")
            continue

        text = read_text(src)
        applied = []
        matched = False
        for rule in touched[rel]:
            if rule.get("kind") == "override":
                continue
            text, count = apply_rule(rel, text, rule)
            applied.append({"note": rule.get("note", ""), "matches": count})
            if count:
                matched = True
            elif rule.get("required", True):
                report["drift"].append({
                    "file": rel,
                    "reason": f"规则未匹配：{rule.get('note', '')}",
                    "pattern": rule["pattern"][:120],
                })
                warn(f"{rel}: 规则未匹配 -> {rule.get('note', '')}")

        if rel in overrides:
            text = overrides[rel].read_text(encoding="utf-8-sig")
            applied.append({"note": f"使用整文件替代：{overrides[rel].name}", "matches": 1})
            matched = True

        if not matched and rel not in mirrored:
            # 一条规则都没命中：不要物化，避免一个与上游逐字一致的副本
            # 静默"遮蔽"上游文件（上游更新后我们会看不到变化）。
            # 例外：自足模式明确要求完整副本，此时"看到上游变化"改由
            # tools/upstream_lock.py 承担。
            report["skipped"].append(rel)
            continue

        write_text(out_root / rel, text)
        report["materialized"].append(rel)
        report["applied"].append({"file": rel, "rules": applied})

    info(f"已物化覆盖层 {len(report['materialized'])} 个文件 -> {out_root}")
    return report


# --------------------------------------------------------------------------- #
# 生成戳记：防止多个构建目录互相覆盖共用的生成物
# --------------------------------------------------------------------------- #

STAMP_NAME = "portgen-stamp.json"

# 参与"生成物能否复用"判定的输入。
#
# 为什么需要：戳记原本只记 cxx_flags / 模块数 / 自足性。而 portgen 用的是
# "最低受支持目标"，与 YY_THUNKS_TARGET_MACRO 无关，所以换目标不会改变戳记 ——
# 这本是好事（生成物确实与目标无关），但副作用是**改了 transforms.py 之后戳记
# 依然匹配**，configure 于是复用旧生成物，改动无声失效。这类错误极难发现，
# 所以把这些输入一并纳入指纹：任何一个变了都必须重新生成。
STAMP_INPUTS = (
    "portrule/thunks.manifest",
    "portrule/exclude.txt",
    # 转换规则是代码，与生成器同在 tools/ 下
    "tools/transforms.py",
)

# 工具自身：存在就纳入指纹（开发场景下升级工具会触发重新生成），
# **不存在则跳过** —— 分发自足场景下 tools/ 本来就不随包发出，
# 若把它当成必需输入，生成物在那里会永远被判定为过期、无法复用。
STAMP_OPTIONAL_INPUTS = (
    "tools/portgen.py",
)


def sync_dist(src_root: Path, dist_dir: Path) -> None:
    """把 `src/`（生成物 port/ + 手写垫片 compat/）整个复制到发布目录。

    为什么放在 portgen 而不是构建脚本里
    ------------------------------------
    同步的唯一正确时机是"生成成功之后"。重新生成不止一条路径：CMake configure、
    `yythunks_regen` 目标、以及手工运行本脚本都会触发。若把同步写在 CMake 的
    configure 分支里，后两条路径就同步不到 —— 生成物已经更新，副本却还是旧的，
    而且没有任何提示。挂在生成之后，任何生成路径都会带上同步。

    为什么 port/ 是整目录替换
    --------------------------
    它完全由本脚本产出。增量覆盖会让旧副本残留早已不存在的模块（自足化之前
    那里只有 17 个文件，现在是 95 个）。compat/ 是手写的，直接覆盖即可。
    """
    if not src_root.is_dir():
        die(f"移植源码目录不存在：{src_root}")
    target = dist_dir / "src"
    stale_port = target / "port"
    if stale_port.is_dir():
        shutil.rmtree(stale_port)
    shutil.copytree(src_root, target, dirs_exist_ok=True)
    print(f"[portgen] 已同步移植源码 -> {target}")


def compute_inputs_fingerprint(root: Path) -> "list[str]":
    """列出 portgen 全部输入的 `路径:SHA-256`（含 overrides 目录），按路径排序。

    为什么是"逐文件哈希"而不是"拼在一起算一个总哈希"：
    校验方除了本脚本，还有 CMake 侧（`file(SHA256)` 是 CMake 内置功能）。
    用逐文件哈希，CMake 不调用 Python 就能独立判定生成物能否复用 ——
    于是**构建不再依赖 tools/**（分发自足场景下使用者只拿 src/ 即可构建）。
    """
    rels = list(STAMP_INPUTS)
    overrides = root / "portrule" / "overrides"
    if overrides.is_dir():
        for p in sorted(overrides.rglob("*")):
            if p.is_file():
                rels.append("port/overrides/"
                            + p.relative_to(overrides).as_posix())

    items = []
    for rel in rels:
        p = root / rel
        # 记录"缺失"而不是跳过：输入被删掉同样应该让生成物过期。
        digest = (hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file()
                  else "<missing>")
        items.append(f"{rel}:{digest}")

    # 可选输入：不存在就不进列表（见 STAMP_OPTIONAL_INPUTS 的说明）
    for rel in STAMP_OPTIONAL_INPUTS:
        p = root / rel
        if p.is_file():
            items.append(f"{rel}:{hashlib.sha256(p.read_bytes()).hexdigest()}")

    return sorted(items)


def write_stamp(out_root: Path, cxx_flags: str, modules: "list[str]",
                self_contained: bool = False,
                inputs: "list[str] | None" = None) -> None:
    """把本次生成所用的配置写进生成目录，供 `verify` 子命令校验。

    生成物放在源码树里（src/port），会被**所有构建目录共用**。若某个目录
    （不同目标宏或前缀模式）重新生成过它，另一个目录再直接 `cmake --build`
    就会用到不属于自己的生成物 —— 而 CMake 不会重新 configure，于是静默出错。
    戳记 + verify 把这种情况变成显式报错。
    """
    flags = " ".join(sorted(cxx_flags.split()))
    stamp = {
        "generator": "tools/portgen.py",
        "cxx_flags": flags,
        "target": next((f.split("=", 1)[1] for f in flags.split()
                        if f.startswith("-DYY_Thunks_Target=")), None),
        "modules": len(modules),
        # 是否物化了上游 Thunks/ 与 Shared/ 的完整副本。
        # 只有自足的生成物才能在"没有上游"的情况下被复用，否则 configure 时
        # 光看戳记通过、实际编译却找不到文件，会出一个很难定位的错误。
        "self_contained": bool(self_contained),
        "inputs": inputs or [],
    }
    write_text(out_root / STAMP_NAME,
               json.dumps(stamp, ensure_ascii=False, indent=2) + "\n")


def verify_stamp(out_root: Path, cxx_flags: str,
                 self_contained: bool = False,
                 inputs: "list[str] | None" = None) -> int:
    """校验生成目录里的戳记与当前构建配置是否一致。"""
    path = out_root / STAMP_NAME
    if not path.is_file():
        print(f"[portgen][错误] 生成目录里没有戳记：{path}\n"
              f"          请先重新 configure（会重新生成 {out_root}）。", file=sys.stderr)
        return 1

    recorded = json.loads(read_text(path))
    current = " ".join(sorted(cxx_flags.split()))
    if recorded.get("cxx_flags") != current:
        print(
            "[portgen][错误] 生成物与当前构建配置不一致！\n"
            f"  生成目录：{out_root}\n"
            f"  生成时  ：{recorded.get('cxx_flags')}\n"
            f"  当前配置：{current}\n"
            "这通常说明另一个构建目录（不同目标宏 / 前缀模式）重新生成过这份"
            "被共用的代码，\n"
            "继续构建会产出与配置不符的产物。\n"
            "解决：重新运行 cmake configure 即可按当前配置重新生成。",
            file=sys.stderr)
        return 1

    if bool(recorded.get("self_contained")) != bool(self_contained):
        print(
            "[portgen][错误] 生成物的自足性与当前要求不一致！\n"
            f"  生成时    ：self_contained={recorded.get('self_contained')}\n"
            f"  当前要求  ：self_contained={bool(self_contained)}\n"
            "  只有自足的生成物才能在缺少上游源码时复用（否则编译会找不到头文件）。\n"
            "  解决：重新运行 cmake configure 以按当前要求重新生成。",
            file=sys.stderr)
        return 1

    if inputs and recorded.get("inputs") != inputs:
        old = set(recorded.get("inputs") or [])
        new = set(inputs)
        changed = sorted(x.split(":", 1)[0]
                         for x in (old ^ new))
        print(
            "[portgen][错误] 生成输入已变化，生成物已过期！\n"
            f"  变化的文件：{', '.join(changed) if changed else '（集合相同但顺序不同）'}\n"
            "  若复用旧生成物，改动会无声失效。解决：重新 configure 以重新生成。",
            file=sys.stderr)
        return 1

    info(f"生成戳记校验通过：{out_root}（目标 {recorded.get('target')}，"
         f"自足={recorded.get('self_contained')}）")
    return 0


# --------------------------------------------------------------------------- #
# 入口
# --------------------------------------------------------------------------- #

def main() -> int:
    parser = argparse.ArgumentParser(description="YY-Thunks MinGW 移植生成器")
    parser.add_argument("command", nargs="?", default="all",
                        choices=["all", "list", "probe", "features", "overlay",
                                 "manifest", "verify", "dist"])
    parser.add_argument("--root", default=None, help="工程根目录（默认取脚本上级）")
    parser.add_argument("--upstream", default=None, help="上游 YY-Thunks 目录")
    parser.add_argument("--out", default=None,
                        help="覆盖层输出目录（必填，如 build/x64/port）")
    parser.add_argument("--compat", default=None, help="垫片目录（默认 src/compat）")
    parser.add_argument("--cxx", default=None, help="编译器（默认取环境变量 CXX 或 g++）")
    parser.add_argument("--cxx-flags", default="", help="与真实编译一致的额外编译选项")
    parser.add_argument("--self-contained", action="store_true",
                        help="把上游 Thunks/ 与 Shared/ 整个复制到生成目录，"
                             "使其自足（编译不再需要上游源码）")
    parser.add_argument("--dist-dir", default=None,
                        help="发布目录：`all` 生成成功后自动同步一份 src/ 进去；"
                             "配合 `dist` 子命令则只做同步")
    parser.add_argument("--include-dirs", default="", help="include 路径，分号分隔")
    parser.add_argument("--refresh", action="store_true", help="manifest 子命令：从上游刷新")
    parser.add_argument("--report", default=None, help="生成报告 JSON 路径")
    args = parser.parse_args()

    root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parent.parent
    upstream = Path(args.upstream).resolve() if args.upstream else root / "upstream" / "YY-Thunks"
    compat_dir = Path(args.compat).resolve() if args.compat else root / "src" / "compat"

    # --out 不给默认值：显式总比隐式安全。
    #
    # 生成物落在源码树里（src/port），由所有构建目录共用。这是可行的 ——
    # 其中清单、覆盖层、守卫表都已与目标无关；仍与目标相关的部分由
    # "生成戳记 + verify 子命令"保障：跨目录覆盖会变成显式报错，而不是静默出错。
    # dist / verify 不产出生成物，因此不需要 --out（dist 只读 src/ 往外复制）。
    if args.command not in ("manifest", "verify", "dist") and not args.out:
        die("缺少 --out。生成目录必须显式指定，例如：\n"
            "  python tools/portgen.py all --out src/port\n"
            "（CMake 配置阶段会自动传入；不设默认值是为了避免误写到非预期位置。）")
    out_root = Path(args.out).resolve() if args.out else root / "src" / "port"

    if args.command == "verify":
        return verify_stamp(out_root, args.cxx_flags, args.self_contained,
                            compute_inputs_fingerprint(root))

    if args.command == "dist":
        # 只做同步，不生成 —— 因此不需要上游源码。
        if not args.dist_dir:
            die("dist 子命令需要 --dist-dir")
        sync_dist(root / "src", Path(args.dist_dir))
        return 0

    upstream_src = upstream / "src"
    thunks_dir = upstream_src / "Thunks"
    manifest_path = root / "portrule" / "thunks.manifest"

    if not thunks_dir.is_dir():
        die(f"上游源码目录不存在：{thunks_dir}")

    if args.command == "manifest":
        refresh_manifest(thunks_dir, manifest_path)
        return 0

    modules, excluded = load_manifest(manifest_path,
                                      root / "portrule" / "exclude.txt")
    if excluded:
        info(f"按 portrule/exclude.txt 排除 {len(excluded)} 个模块："
             f"{', '.join(excluded)}")

    if args.command in ("all", "list", "probe", "features"):
        for module in modules:
            if not (thunks_dir / module).is_file():
                die(f"清单模块在上游缺失：{module}")

    if args.command in ("all", "list"):
        gen_list(modules, out_root / "YY_Thunks_List.hpp")

    # 注意执行顺序：必须先物化覆盖层，再做预处理探测，
    # 否则探测读到的会是未经转换的上游源码，生成的声明与真实编译不一致。
    report = {}
    if args.command in ("all", "overlay"):
        # transforms 与本脚本同在 tools/ 下，直接 import 即可 ——
        # 此前它在顶层 port/ 目录，需要手工 sys.path.insert 才能导入。
        import transforms as transforms_mod
        report = gen_overlay(upstream_src, out_root, transforms_mod.TRANSFORMS,
                             root / "portrule" / "overrides",
                             mirror_dirs=(["Thunks", "Shared"]
                                          if args.self_contained else None))

    # 自足模式下，覆盖层本身就是一棵完整的源码树（含上游 Thunks/ 与 Shared/ 的
    # 全部副本），所以探测与后续生成都从它读取 —— 上游源码不必再参与。
    # 这正是"完全不依赖上游"的关键一环：gen_overlay 之后的所有步骤都不碰上游，
    # 于是把生成目录随包发出后，使用者无需 clone 上游即可构建。
    read_root = out_root if args.self_contained else upstream_src
    read_thunks = read_root / "Thunks"

    enabled = None
    if args.command in ("all", "probe", "features"):
        import os
        cxx = args.cxx or os.environ.get("CXX") or "g++"
        # 顺序必须与真实编译的 include 搜索顺序完全一致：
        # 覆盖层优先于上游，否则探测到的签名会与真正编译时的不一致。
        include_dirs = [str(out_root), str(out_root / "Thunks"), str(out_root / "Shared"),
                        str(compat_dir), str(read_root),
                        str(read_thunks), str(read_root / "Shared")]
        if args.include_dirs:
            include_dirs = [p for p in args.include_dirs.split(";") if p] + include_dirs
        enabled = probe_thunks(out_root / "YY_Thunks_Declarations.hpp",
                               out_root / "_yy_thunks_probe.cpp",
                               read_thunks, modules,
                               cxx, args.cxx_flags.split(), include_dirs,
                               guard_table_file=out_root / "YY_Thunks_EnableTable.hpp",
                               # 必须按真实的 include 搜索顺序找模块文件：
                               # 覆盖层优先（force_thunk 会把守卫改成 `#if 1`），
                               # 否则会读到上游原文的过期守卫。
                               module_search_dirs=[out_root / "Thunks", out_root,
                                                   read_thunks, read_root])

    if args.command in ("all", "features"):
        gen_features(read_thunks, modules, out_root / "YY_Thunks_Features.hpp", enabled,
                     collect_late_bound_functions(read_thunks))

    # 记录本次生成所用的配置，供 `verify` 校验（防多个构建目录互相覆盖）
    write_stamp(out_root, args.cxx_flags, modules, args.self_contained,
                compute_inputs_fingerprint(root))

    # 生成成功 -> 同步一份到发布目录。
    # 放在这里（而不是构建脚本里）是因为重新生成有多条路径：CMake configure、
    # yythunks_regen 目标、手工运行本脚本。挂在这里，任何一条都会带上同步。
    if args.dist_dir and args.command == "all":
        sync_dist(root / "src", Path(args.dist_dir))

    if args.report and report:
        write_text(Path(args.report), json.dumps(report, ensure_ascii=False, indent=2))

    if report.get("drift"):
        warn(f"检测到 {len(report['drift'])} 处上游漂移，请复核 tools/transforms.py")
        # 退出码 3（不是 2）：Python 自身出错（找不到脚本、语法错误…）的退出码
        # 恰好也是 2，用它表示"漂移"会让"脚本根本没跑起来"被静默当成漂移放过。
        return 3

    return 0


if __name__ == "__main__":
    sys.exit(main())
