#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""上游版本锁定与漂移校验。

用途
----
`upstream/YY-Thunks` 是只读的上游源码。移植层（src/compat、port/、src/port）
是建立在其之上的增量，因此必须能回答两个问题：

  1. 我现在移植的是上游哪个版本？（可复现）
  2. 上游动了哪些文件、有没有动到我依赖的接口？（可感知）

本脚本把上游的仓库地址、commit 以及 src/ 下每个文件的 SHA-256 记入
`upstream.lock.json`，之后可用 `--verify` 一键比对。

用法
----
  python tools/upstream_lock.py            # 生成/更新 upstream.lock.json
  python tools/upstream_lock.py --verify   # 校验当前上游是否与锁文件一致
  python tools/upstream_lock.py --diff     # 打印发生变化的文件清单
"""

from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")  # type: ignore[attr-defined]
    except Exception:  # pragma: no cover
        pass

LOCK_NAME = "upstream.lock.json"


def sha256_of(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as fp:
        for chunk in iter(lambda: fp.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def git(upstream: Path, *args: str) -> "str | None":
    try:
        out = subprocess.run(("git", "-C", str(upstream)) + args,
                             capture_output=True, text=True, errors="replace")
    except OSError:
        return None
    if out.returncode != 0:
        return None
    return out.stdout.strip()


def snapshot(upstream: Path) -> dict:
    src = upstream / "src"
    files = {}
    for path in sorted(src.rglob("*")):
        if path.is_file():
            files[path.relative_to(upstream).as_posix()] = sha256_of(path)
    remote = git(upstream, "remote", "get-url", "origin")
    return {
        "repository": remote,
        "commit": git(upstream, "rev-parse", "HEAD"),
        "commit_date": git(upstream, "log", "-1", "--pretty=%ci"),
        "commit_subject": git(upstream, "log", "-1", "--pretty=%s"),
        "file_count": len(files),
        "files": files,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="上游版本锁定/校验")
    parser.add_argument("--root", default=None, help="工程根目录")
    parser.add_argument("--upstream", default=None, help="上游 YY-Thunks 目录")
    parser.add_argument("--verify", action="store_true", help="校验是否与锁文件一致")
    parser.add_argument("--diff", action="store_true", help="打印差异清单")
    args = parser.parse_args()

    root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parent.parent
    upstream = Path(args.upstream).resolve() if args.upstream else root / "upstream" / "YY-Thunks"
    lock_path = root / LOCK_NAME

    if not (upstream / "src").is_dir():
        print(f"[上游锁] 找不到上游源码：{upstream}", file=sys.stderr)
        return 1

    current = snapshot(upstream)

    if not args.verify:
        lock_path.write_text(json.dumps(current, ensure_ascii=False, indent=2) + "\n",
                             encoding="utf-8")
        print(f"[上游锁] 已写入 {lock_path}")
        print(f"[上游锁] commit={current['commit']} files={current['file_count']}")
        return 0

    if not lock_path.is_file():
        print(f"[上游锁] 缺少锁文件 {lock_path}，请先运行不带 --verify 的一次", file=sys.stderr)
        return 1

    locked = json.loads(lock_path.read_text(encoding="utf-8"))
    problems = []

    if locked.get("commit") != current.get("commit"):
        problems.append(f"commit: {locked.get('commit')} -> {current.get('commit')}")

    old_files, new_files = locked.get("files", {}), current["files"]
    added = sorted(set(new_files) - set(old_files))
    removed = sorted(set(old_files) - set(new_files))
    modified = sorted(f for f in set(old_files) & set(new_files)
                      if old_files[f] != new_files[f])

    if args.diff:
        for tag, items in (("新增", added), ("删除", removed), ("修改", modified)):
            for name in items:
                print(f"  [{tag}] {name}")

    if added or removed or modified or problems:
        print("[上游锁] 上游已发生变化：", file=sys.stderr)
        for p in problems:
            print(f"  {p}", file=sys.stderr)
        print(f"  新增 {len(added)} / 删除 {len(removed)} / 修改 {len(modified)}",
              file=sys.stderr)
        print("  请复核 tools/transforms.py 与 portrule/exclude.txt，"
              "并重新生成（cmake --preset ... 或 yythunks_regen）。", file=sys.stderr)
        return 2

    print("[上游锁] 与锁文件一致")
    return 0


if __name__ == "__main__":
    sys.exit(main())
