#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""跨目标快照：把每个 thunk 的"启用条件"归纳成目标阈值，并检查两件事。

背景
----
本移植目前需要按目标各生成一份 Enabled / Declarations 头，原因是 GCC 没有
MSVC 的 `__if_exists`，"某个 thunk 在当前目标下是否参与编译"无法在编译期窥视，
只能在构建前用真实预处理器探出来（见 docs/PORTING.md）。

若要把生成物变成"一份、目标无关"、完全由 `-DYY_Thunks_Target=...` 在编译期决定，
前提是两个前提同时成立（`__Version` 是预处理器算术表达式，所以阈值可以写成
`#if YY_Thunks_Target < __WindowsNTx`）：

  1. **单调性**：每个 thunk 的启用集合，在版本序上必须恰好是一段前缀
     （"到某个版本之前启用，之后变成原生、不再需要 thunk"）。
     不是前缀 ⇒ 单个阈值表达式表达不了 ⇒ 该 thunk 必须单独处理。
  2. **签名不漂移**：同一个 thunk 的声明文本在不同目标下必须一致，
     否则"无条件声明全部 thunk"不成立。

本脚本在**全部合法目标**上各跑一次真实探测，直接给出这两条的答案，
并产出一份 JSON 快照作为重构前后的回归判据（逐位一致才算没改语义）。

用法
----
  python tools/target_matrix.py                          # 打印结论
  python tools/target_matrix.py --out-json baseline.json # 顺便存快照
  python tools/target_matrix.py --prefixed               # 带 __FALLBACK_PREFIX
  python tools/target_matrix.py --keep                   # 保留工作目录（排查用）

退出码：0 = 两个前提都成立；2 = 存在需要单独处理的 thunk。
"""

from __future__ import annotations

import argparse
import hashlib
import importlib
import json
import re
import shutil
import sys
import tempfile
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS_DIR))
import portgen  # noqa: E402

for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")  # type: ignore[attr-defined]
    except Exception:  # pragma: no cover
        pass

# #define __WindowsNT6_2 __Version(6, 2, 9200, 0)
VERSION_DEF = re.compile(
    r"^#define\s+(__WindowsNT[A-Za-z0-9_]*)\s+"
    r"__Version\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)")
# 每行一条 thunk 声明，以 try_get_X() noexcept; 结尾
TRY_GET_TAIL = re.compile(r"try_get_(\w+)\(\)\s*noexcept;")

# 与 CMake 的 YY_THUNKS_COMMON_DEFS 保持一致（目标宏除外，它逐次替换）
BASE_DEFS = ["_WIN32_WINNT=0x0A00", "WINVER=0x0A00", "YY_Thunks_Port=1"]


def parse_targets(shared_defs: Path) -> "list[tuple[int, str]]":
    """按版本序返回 [(版本值, 宏名), ...]。版本值即 __Version 展开后的整数。"""
    found = []
    for raw in shared_defs.read_text(encoding="utf-8", errors="replace").splitlines():
        m = VERSION_DEF.match(raw.strip())
        if not m:
            continue
        major, minor, build, revision = (int(m.group(i)) for i in range(2, 6))
        value = (major * 0x1000000000000 + minor * 0x100000000
                 + build * 0x10000 + revision)
        found.append((value, m.group(1)))
    found.sort()
    return found


def prefix_check(observed: "list[bool]") -> "tuple[bool, int]":
    """观察到的启用序列是否形如 True*n + False*(N-n)；返回 (是否, n)。"""
    n = 0
    while n < len(observed) and observed[n]:
        n += 1
    return all(not v for v in observed[n:]), n


def main() -> int:
    parser = argparse.ArgumentParser(description="跨目标探测快照与阈值归纳可行性分析")
    parser.add_argument("--root", default=None, help="工程根目录")
    parser.add_argument("--upstream", default=None, help="上游 YY-Thunks 目录")
    parser.add_argument("--cxx", default=None, help="编译器（默认 CXX 环境变量或 g++）")
    parser.add_argument("--cxx-flags", default="", help="额外的编译选项")
    parser.add_argument("--prefixed", action="store_true",
                        help="附加 __FALLBACK_PREFIX=YY_Thunks_（lib 模式）")
    parser.add_argument("--out-json", default=None, help="把快照写入 JSON")
    parser.add_argument("--keep", action="store_true", help="保留工作目录")
    args = parser.parse_args()

    root = Path(args.root).resolve() if args.root else TOOLS_DIR.parent
    upstream = (Path(args.upstream).resolve() if args.upstream
                else root / "upstream" / "YY-Thunks")
    upstream_src = upstream / "src"
    thunks_dir = upstream_src / "Thunks"
    compat_dir = root / "src" / "compat"
    import os
    cxx = args.cxx or os.environ.get("CXX") or "g++"

    if not thunks_dir.is_dir():
        portgen.die(f"上游源码目录不存在：{thunks_dir}")

    targets = parse_targets(upstream_src / "Shared" / "SharedDefs.h")
    if not targets:
        portgen.die("无法从 SharedDefs.h 解析出版本宏")

    base_defs = list(BASE_DEFS)
    if args.prefixed:
        base_defs.append("__FALLBACK_PREFIX=YY_Thunks_")

    modules, excluded = portgen.load_manifest(
        root / "portrule" / "thunks.manifest", root / "portrule" / "exclude.txt")
    print(f"[目标矩阵] {len(modules)} 个模块参与编译"
          + (f"，{len(excluded)} 个被排除" if excluded else ""))
    print(f"[目标矩阵] 待探测目标 {len(targets)} 个："
          + ", ".join(n for _, n in targets))

    work = Path(tempfile.mkdtemp(prefix="yy-target-matrix-"))
    try:
        # 覆盖层与目标无关（见 docs/PORTING.md），只物化一次供全部目标复用。
        # 这一步本身就是对该性质的验证：若覆盖层依赖目标，就不可能复用。
        overlay_root = work / "overlay"
        # transforms 与本脚本同在 tools/ 下，直接 import（无需 sys.path 插路径）
        transforms = importlib.import_module("transforms").TRANSFORMS
        overlay_report = portgen.gen_overlay(
            upstream_src, overlay_root, transforms,
            root / "portrule" / "overrides")
        # 清单头同样与目标无关，一并只生成一次
        portgen.gen_list(modules, overlay_root / "YY_Thunks_List.hpp")
        drift = overlay_report.get("drift") or []
        if drift:
            portgen.warn(f"覆盖层存在 {len(drift)} 处漂移，结果可能不准确")

        include_dirs = [
            str(overlay_root), str(overlay_root / "Thunks"), str(overlay_root / "Shared"),
            str(compat_dir), str(upstream_src), str(thunks_dir), str(upstream_src / "Shared")]

        enabled_map: "dict[str, set[str]]" = {}
        decl_map: "dict[str, dict[str, str]]" = {}
        failed: "list[str]" = []

        print()
        for _, name in targets:
            out_dir = work / name
            out_dir.mkdir(parents=True, exist_ok=True)
            flags = ([f"-DYY_Thunks_Target={name}"]
                     + [f"-D{d}" for d in base_defs]
                     + (args.cxx_flags.split() if args.cxx_flags else []))
            try:
                enabled = portgen.probe_thunks(
                    out_dir / "YY_Thunks_Declarations.hpp",
                    out_dir / "_yy_probe.cpp",
                    thunks_dir, modules, cxx, flags, include_dirs,
                    # 每个目标都顺手做一次"守卫表自校验"：
                    # 目标无关的守卫表在该目标下求值，必须与真实探测完全一致。
                    # 同一份表在 22 个目标上都能对上，才算证明它真的与目标无关。
                    guard_table_file=overlay_root / "YY_Thunks_EnableTable.hpp",
                    module_search_dirs=[overlay_root / "Thunks", overlay_root, thunks_dir])
            except SystemExit:
                failed.append(name)
                print(f"  {name:<26} 探测失败（已跳过）", file=sys.stderr)
                continue

            enabled_map[name] = set(enabled)

            decls = {}
            text = (out_dir / "YY_Thunks_Declarations.hpp").read_text(
                encoding="utf-8", errors="replace")
            for line in text.splitlines():
                m = TRY_GET_TAIL.search(line)
                if m:
                    decls[m.group(1)] = line.strip()
            decl_map[name] = decls
            print(f"  {name:<26} 启用 {len(enabled):>4} 个")

        ok_targets = [n for _, n in targets if n in enabled_map]
        if not ok_targets:
            portgen.die("没有任何目标探测成功，无法分析")

        all_names = sorted(set().union(*(enabled_map[t] for t in ok_targets)))

        # ---- 前提 1：单调阈值 ----
        print()
        print("=" * 72)
        print("前提 1：启用条件是否可归纳为单调阈值（版本序上是一段前缀）")
        print("=" * 72)
        thresholds: "dict[str, int]" = {}
        nonmono: "list[tuple[str, list[bool]]]" = []
        for name in all_names:
            observed = [name in enabled_map[t] for t in ok_targets]
            is_prefix, k = prefix_check(observed)
            if is_prefix:
                thresholds[name] = k
            else:
                nonmono.append((name, observed))

        # 顺带报告目标之间的"有效配置"重复（x64 上低于架构下限的宏会被上游钳制）
        sig = {}
        for t in ok_targets:
            key = (len(enabled_map[t]),
                   hashlib.sha1(",".join(sorted(enabled_map[t])).encode()).hexdigest()[:8])
            sig.setdefault(key, []).append(t)
        dupes = [v for v in sig.values() if len(v) > 1]

        print(f"thunk 总数            : {len(all_names)}")
        print(f"可归纳为阈值          : {len(thresholds)}")
        print(f"不可归纳（需单独处理）: {len(nonmono)}")
        if dupes:
            print(f"有效配置相同的目标组  : "
                  + "; ".join(" = ".join(g) for g in dupes))

        buckets: "dict[int, list[str]]" = {}
        for name, k in thresholds.items():
            buckets.setdefault(k, []).append(name)
        print()
        print("阈值分布（k = 从第 k 个目标起变为原生、不再需要 thunk）：")
        for k in sorted(buckets):
            where = "全程启用" if k == len(ok_targets) else (
                "全程不启用" if k == 0 else f"< {ok_targets[k]}")
            print(f"  k={k:<3} {len(buckets[k]):>4} 个   {where}")

        if nonmono:
            print()
            print(f"!! 以下 {len(nonmono)} 个 thunk 的启用条件不是单调阈值：")
            for name, observed in nonmono:
                marks = "".join("1" if v else "0" for v in observed)
                print(f"   {name:<44} {marks}")
            print("   （列为目标序，1=启用。形如 110111 即为非前缀）")

        # ---- 前提 2：声明文本是否随目标漂移 ----
        print()
        print("=" * 72)
        print("前提 2：同一 thunk 的声明文本是否随目标变化")
        print("=" * 72)
        texts: "dict[str, set[str]]" = {}
        for t in ok_targets:
            for name, line in decl_map[t].items():
                texts.setdefault(name, set()).add(line)

        drifted = {n: v for n, v in texts.items() if len(v) > 1}
        print(f"出现声明的 thunk 数        : {len(texts)}")
        print(f"声明随目标漂移的 thunk 数  : {len(drifted)}")
        if drifted:
            print()
            print("!! 以下 thunk 在不同目标下的声明不同（按行哈希分组）：")
            for name, variants in sorted(drifted.items()):
                print(f"   {name:<44} {len(variants)} 种形态")
                for v in sorted(variants):
                    where = ", ".join(
                        t for t in ok_targets if decl_map[t].get(name) == v)
                    digest = hashlib.sha1(v.encode()).hexdigest()[:8]
                    print(f"      [{digest}] {where}")
                    print(f"        {v[:150]}{'...' if len(v) > 150 else ''}")

        # ---- 快照 ----
        if args.out_json:
            snapshot = {
                "cxx": cxx,
                "cxx_flags": base_defs,
                "modules": len(modules),
                "excluded": sorted(excluded),
                "targets": ok_targets,
                "failed_targets": failed,
                "enabled": {t: sorted(enabled_map[t]) for t in ok_targets},
                "decl_sha1": {
                    t: {n: hashlib.sha1(v.encode()).hexdigest()
                        for n, v in sorted(decl_map[t].items())}
                    for t in ok_targets},
                "thresholds": thresholds,
                "non_monotonic": [n for n, _ in nonmono],
                "decl_drifted": sorted(drifted),
            }
            Path(args.out_json).write_text(
                json.dumps(snapshot, ensure_ascii=False, indent=2) + "\n",
                encoding="utf-8")
            print()
            print(f"[目标矩阵] 快照已写入 {args.out_json}")

        blockers = len(nonmono) + len(drifted)
        print()
        print("=" * 72)
        if blockers == 0:
            print("结论：两个前提均成立 —— 可以做成「一份目标无关的生成物」，")
            print("      由 -DYY_Thunks_Target=... 在编译期决定目标。")
        else:
            print(f"结论：有 {blockers} 处需要单独处理"
                  f"（非单调 {len(nonmono)}，签名漂移 {len(drifted)}）。")
        print("=" * 72)
        return 2 if blockers else 0
    finally:
        if args.keep:
            print(f"[目标矩阵] 工作目录保留在 {work}")
        else:
            shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
