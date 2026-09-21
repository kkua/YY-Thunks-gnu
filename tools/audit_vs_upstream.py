#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""与上游核对：函数名有没有被改动？文本偏差都有哪些？

为什么要单独做这件事
--------------------
本移植的做法是"不改上游一行，用垫片 + 增量转换叠加"。代价是：上游源码在进入
编译器之前会经过一次文本转换，**理论上存在'顺手改坏'的可能** —— 例如某个宏把
thunk 的名字换掉（真实发生过：MinGW 的 esent.h 把 JetGetTableColumnInfoW 别名成
了 ...A，导致 W 版 thunk 定义了 A 入口），或者某条规则动到了行为而不只是语法。

所以需要一层**独立于转换规则本身**的核对（否则就是用规则检查规则）。本工具回答
两个问题：

  1. **名字**：分三层对照，任何一层有差集就是问题
       上游源码原文  ->  覆盖层（转换后）  ->  编译器实际展开结果
     前一段能发现"我们的转换改了名字"，后一段能发现"宏把名字改了"。
  2. **语义**：覆盖层相对上游的**全部**文本偏差，逐行列出来供人审阅。
     工具负责把偏差收敛到可审阅的规模，结论由人读出来。

用法
----
  # --port 是"编译器实际看到什么"的来源，缺了它第 3 层核对照样无从进行
  python tools/audit_vs_upstream.py --port src/port
  python tools/audit_vs_upstream.py --port src/port --diff         # 附完整逐行差异
  python tools/audit_vs_upstream.py --port src/port --out dev.txt  # 导出差异供审阅

已接入 CTest（用例名 upstream_audit），跑 `ctest` 时会自动执行。

退出码：0 = 名字三层一致；2 = 名字被改动，或第 3 层核对无法进行（见下）。
"""

from __future__ import annotations

import argparse
import difflib
import importlib
import re
import sys
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS_DIR))
import portgen  # noqa: E402

for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")  # type: ignore[attr-defined]
    except Exception:  # pragma: no cover
        pass

# 生成的声明头里每条声明的结尾：static ... try_get_<Name>() noexcept;
COMPILED_NAME = re.compile(r"try_get_(\w+)\(\)\s*noexcept;")


def thunk_names(files: "dict[str, Path]") -> "set[str]":
    """用 portgen 的守卫解析取出文件里所有 __DEFINE_THUNK 的函数名（第 5 个实参）。

    注意取的是**源码字面量**，不做宏展开 —— 这正是与"编译器实际看到的名字"
    对照时需要的形态。
    """
    return set(portgen.collect_thunk_guards(files).keys())


def main() -> int:
    parser = argparse.ArgumentParser(description="与上游核对：名字与语义偏差")
    parser.add_argument("--root", default=None, help="工程根目录")
    parser.add_argument("--upstream", default=None, help="上游 YY-Thunks 目录")
    parser.add_argument("--port", default=None,
                        help="生成目录（默认 src/port；须含 YY_Thunks_Declarations.hpp）")
    parser.add_argument("--diff", action="store_true", help="输出完整逐行差异")
    parser.add_argument("--out", default=None, help="把完整差异写入文件")
    args = parser.parse_args()

    root = Path(args.root).resolve() if args.root else TOOLS_DIR.parent
    upstream = (Path(args.upstream).resolve() if args.upstream
                else root / "upstream" / "YY-Thunks")
    upstream_src = upstream / "src"
    thunks_dir = upstream_src / "Thunks"
    port_dir = Path(args.port).resolve() if args.port else root / "src" / "port"

    if not thunks_dir.is_dir():
        portgen.die(f"找不到上游源码：{thunks_dir}")
    if not port_dir.is_dir():
        portgen.die(f"找不到生成目录（先跑一次 configure）：{port_dir}")

    modules, _ = portgen.load_manifest(root / "portrule" / "thunks.manifest",
                                       root / "portrule" / "exclude.txt")

    # ---- 第 1 层 / 第 2 层：上游原文 vs 覆盖层 ---------------------------- #
    upstream_files = {m: thunks_dir / m for m in modules if (thunks_dir / m).is_file()}
    # 覆盖层只物化"被规则改过"的文件；没命中的文件与上游逐字一致，此时回退到
    # 上游读原文是**正确**的（第 1 层核对对它而言平凡成立）。
    # 但必须记住各有多少：否则"一个都没物化"这种退化情形（上游大改导致规则
    # 全部失配就会这样）会伪装成"名字完全一致 ✓"——又一张虚假的健康证明。
    overlay_files: "dict[str, Path]" = {}
    materialized_files = 0
    for m in modules:
        for candidate_dir in (port_dir / "Thunks", port_dir, thunks_dir):
            candidate = candidate_dir / m
            if candidate.is_file():
                overlay_files[m] = candidate
                if candidate_dir != thunks_dir:
                    materialized_files += 1
                break

    overlay_empty = materialized_files == 0
    if overlay_empty:
        print(f"\n[错误] {port_dir} 下没有任何 Thunks 文件 —— 覆盖层未物化。\n"
              "       第 1 层核对会退化成'上游 vs 上游'，结论没有参考价值。\n"
              "       常见原因：上游改动导致 transforms.py 的规则全部失配。",
              file=sys.stderr)

    upstream_names = thunk_names(upstream_files)
    overlay_names = thunk_names(overlay_files)

    renamed_by_transform = overlay_names - upstream_names
    lost_by_transform = upstream_names - overlay_names

    print("=" * 74)
    print("第 1 层：上游源码原文  ->  覆盖层（转换后）")
    print("=" * 74)
    print(f"  上游 thunk 名总数    : {len(upstream_names)}")
    print(f"  覆盖层 thunk 名总数  : {len(overlay_names)}")
    print(f"  取自覆盖层的文件    : {materialized_files}"
          f"（其余 {len(overlay_files) - materialized_files} 个未物化，回退上游原文）")
    if renamed_by_transform or lost_by_transform:
        print(f"  !! 转换后多出的名字  : {len(renamed_by_transform)}")
        for n in sorted(renamed_by_transform)[:20]:
            print(f"       {n}")
        print(f"  !! 转换后消失的名字  : {len(lost_by_transform)}")
        for n in sorted(lost_by_transform)[:20]:
            print(f"       {n}")
    else:
        print("  结论：名字集合完全一致 —— 转换规则没有改动任何 thunk 名 ✓")

    # ---- 第 3 层：编译器实际展开结果 ------------------------------------- #
    decls = port_dir / "YY_Thunks_Declarations.hpp"

    print()
    print("=" * 74)
    print("第 2 层：覆盖层  ->  编译器实际展开结果")
    print("=" * 74)
    compiled_renamed: "set[str]" = set()
    decls_missing = not decls.is_file()
    if decls_missing:
        # 这一层是**唯一**能发现"某个宏把 thunk 改了名"的地方（esent 那类真实
        # 故障就是这样：MinGW 的 esent.h 把 JetGetTableColumnInfoW 别名成 ...A）。
        # 静默跳过等于发出一张虚假的健康证明，所以这里必须算失败。
        print(f"\n[错误] 找不到生成的声明头：{decls}\n"
              "       第 3 层核对无法进行，本次结论不完整。\n"
              "       先跑一次 configure（portgen 会生成它）再执行本核对。",
              file=sys.stderr)
    else:
        compiled = set(COMPILED_NAME.findall(decls.read_text(encoding="utf-8",
                                                            errors="replace")))
        compiled_renamed = compiled - overlay_names
        print(f"  声明头：{decls}")
        print(f"  编译器看到的名字数  : {len(compiled)}（当前目标的启用集合）")
        if compiled_renamed:
            print(f"  !! 覆盖层里没有、编译器却看到的名字：{len(compiled_renamed)}")
            for n in sorted(compiled_renamed)[:20]:
                print(f"       {n}     <- 被某个宏改名了")
        else:
            print("  结论：编译器看到的名字都在覆盖层源码里 —— 没有被宏改名 ✓")

    # ---- 语义：全部文本偏差 ---------------------------------------------- #
    # transforms 与本脚本同在 tools/ 下，直接 import（无需 sys.path 插路径）
    transforms = importlib.import_module("transforms").TRANSFORMS
    touched = sorted({rel for rule in transforms for rel in rule["files"]})
    overrides_dir = root / "portrule" / "overrides"
    if overrides_dir.is_dir():
        touched = sorted(set(touched) | {
            p.relative_to(overrides_dir).as_posix()
            for p in overrides_dir.rglob("*") if p.is_file()})

    print()
    print("=" * 74)
    print("语义：覆盖层相对上游的全部文本偏差")
    print("=" * 74)

    diffs: "list[str]" = []
    materialized = 0
    identical = 0
    missing = 0
    for rel in touched:
        up_path = upstream_src / rel
        ov_path = port_dir / rel
        if not up_path.is_file():
            print(f"  [上游缺失] {rel}")
            missing += 1
            continue
        if not ov_path.is_file():
            # 规则一条都没命中 -> 不物化，等于与上游逐字一致
            identical += 1
            continue
        materialized += 1
        up_lines = portgen.read_text(up_path).splitlines(keepends=True)
        ov_lines = portgen.read_text(ov_path).splitlines(keepends=True)
        if up_lines == ov_lines:
            identical += 1
            continue
        diff = list(difflib.unified_diff(up_lines, ov_lines,
                                         fromfile=f"upstream/{rel}",
                                         tofile=f"src/port/{rel}", n=3))
        shown = sum(1 for line in diff
                    if line[:1] in "+-" and line[:3] not in ("+++", "---"))
        diffs.append("".join(diff))
        print(f"  {rel:<44} 偏差行数 {shown}")

    print()
    print(f"  触及文件总数 : {len(touched)}")
    print(f"  已物化       : {materialized}   （与上游有差异）")
    print(f"  未物化       : {identical}   （与上游逐字一致）")
    if missing:
        print(f"  上游缺失     : {missing}")

    if args.out and diffs:
        Path(args.out).write_text("\n".join(diffs), encoding="utf-8")
        print(f"\n  完整差异已写入：{args.out}")
    if args.diff:
        for d in diffs:
            print(d)

    print()
    print("=" * 74)
    problems = len(renamed_by_transform) + len(lost_by_transform) + len(compiled_renamed)
    incomplete = decls_missing or overlay_empty
    if problems:
        print(f"结论：发现 {problems} 处名字改动，需要人工判断是否为有意为之。")
    elif incomplete:
        reasons = []
        if overlay_empty:
            reasons.append("覆盖层未物化（第 1 层退化为自比）")
        if decls_missing:
            reasons.append("第 3 层核对缺失")
        print("结论：名字集合表面一致，但核对不完整 —— " + "、".join(reasons))
    else:
        print("结论：三层名字一致，未发现函数名变化。")
        print("      语义偏差见上（逐条文本差异可用 --diff / --out 展开审阅）。")
    print("=" * 74)
    return 2 if (problems or incomplete) else 0


if __name__ == "__main__":
    sys.exit(main())
