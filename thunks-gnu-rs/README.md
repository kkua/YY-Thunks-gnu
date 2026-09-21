# thunks-gnu

构建期辅助 crate：把 [YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks)
（MinGW-w64 / GCC 移植版）编译成 `libyythunks.a` 并交给链接器，让 Rust 程序
在旧版 Windows 上也能调用较新的 Win32 API。

本 crate 不导出任何 Rust 符号，唯一作用就是被你的 `build.rs` 调用。

> English version: [README_EN.md](README_EN.md)。

## 用法

```toml
# Cargo.toml
[build-dependencies]
thunks-gnu = "0.1"
```

```rust
// build.rs
fn main() {
    thunks_gnu::build();
}
```

就这样：`build()` 会把 vendored 的 YY-Thunks 源码编译成 `$OUT_DIR/libyythunks.a`，
并输出相应的 `cargo:rustc-link-*` 指令（静态库排在最前，这样 YY-Thunks 会顶替
系统导入库里的同名符号）。

## 前置条件

| 项目 | 说明 |
| --- | --- |
| 目标三元组 | **仅 `x86_64-pc-windows-gnu`**。其他目标只打印一条 `cargo::warning` 后直接返回；`i686` 与 Clang 工具链会给出明确说明后失败 |
| 编译器 | `PATH` 上要有 MinGW-w64 的 `g++`（或用 `CXX` 指定）。只支持 GCC：移植依赖 `-fno-toplevel-reorder` |
| Rust | 1.85 及以上（`edition = "2024"`） |
| 构建耗时 | 单个翻译单元约 5~10 秒（每个 profile / out-dir 只编译一次） |

## feature 与最低支持系统

feature 的含义是**你希望支持的最低系统**（即上游的 `YY_Thunks_Target`）。
目标越高，参与编译的 thunk 越少。

| feature | `YY_Thunks_Target` | 对应系统 |
| --- | --- | --- |
| `vista`（默认） | `__WindowsNT6` | Vista / Server 2008 |
| `win7` | `__WindowsNT6_1` | Windows 7 / Server 2008 R2 |
| `win8` | `__WindowsNT6_2` | Windows 8 / Server 2012 |
| `win10_10240` | `__WindowsNT10_10240` | Windows 10 1507 |
| `win10_19041` | `__WindowsNT10_19041` | Windows 10 2004 / 20H1 |

同时开启多个时取最高的那个。`windows_vista`、`win10` 分别是 `vista`、
`win10_10240` 的别名。

低于 Vista 的目标（XP / Server 2003）与 32 位 x86 在本 GCC 移植中不受支持，
也没有对应的 feature。

## 注意事项与限制

- **仅支持 `x86_64-pc-windows-gnu` 工具链**：这是硬性限制。构建脚本会在编译期检查并强制：
  - 非 windows-gnu 目标（如 `x86_64-pc-windows-msvc`）——只打印一条 `cargo::warning` 后直接返回（不产出 `.a`、也不输出任何 `-l`）；
  - `i686-pc-windows-gnu`——上游依赖 MSVC 的 `__identifier` 拼出含 `@` 的 stdcall IAT 符号（`_imp__Api@N`），GCC 无法拼出该记号，构建脚本直接 `panic`；
  - Clang 工具链——移植依赖 GCC 专有的 `-fno-toplevel-reorder`（上游靠"节内首元素是边界哨兵"做自动注册），Clang 不支持，构建脚本同样 `panic`。
  简言之：本 crate 只能用于 `x86_64-pc-windows-gnu`，其它目标三元组一律不参与编译。
- **链接顺序很关键**：`yythunks` 是刻意排在系统导入库之前输出的；如果你自己
  追加 `-l`，请保持它在最前。顺序错会在链接期报 `multiple definition`，而不是
  静默失去覆盖。
- **系统导入库**：上游的依赖写在 `#pragma comment(lib, ...)` 里，GCC 会忽略。
  Rust 的 `windows-gnu` 目标已经链接了常用的那几个；若遇到
  `undefined reference`，按需自己补 `-l`（完整清单以注释形式保留在
  `src/lib.rs` 末尾，CMake 构建也会生成 `yythunks-link-requirements.txt`）。
- **不需要 C++ 运行时**：本库用 `-fno-exceptions -fno-unwind-tables` 编译，不会
  引用 `__gxx_personality_seh0` / `_Unwind_Resume`。但若链接器不是 `g++` 驱动，
  仍可能需要 `-lgcc`（`__chkstk_ms`、`__popcountdi2`）。
- **SEH 被降级**：GCC 没有 `__try` / `__except`，移植层会原样执行 `__try` 体、
  让 `__except` 体成为死代码。正常路径行为与上游一致，但畸形 PE、探测性内存读取
  这类异常场景不再被捕获。
- **产物尺寸与 CMake 版一致**：CMake 未设 `CMAKE_BUILD_TYPE`（既不优化也不生成调试
  信息），而 cc 会按 Cargo profile 追加 `-g -gdwarf-2` 和无条件的
  `-ffunction-sections -fdata-sections`；本 crate 显式关掉了它们，因此 dev 构建的
  `libyythunks.a` 与 `build/x64` 下的 CMake 产物同尺寸（差异仅在 ar 头部的几十字节）。
  同时强制 `-O0`（同样对齐 CMake）：实测 `-O3` 会在静态注册阶段触发
  `STATUS_ACCESS_VIOLATION`，port 是极薄的转发层，不优化无性能损失。
- **构建输出默认静默**：上游代码在 GCC 下会产生大量告警（含 GCC 没有 `-Wno-` 开关的
  宏重定义告警）以及它自己用 `#pragma message` 留下的提示，这些都不再打印。
  排查时设 `THUNKS_GNU_SHOW_WARNINGS=1` 即可恢复显示（编译失败时的错误信息不受
  此开关影响，始终会打印）。
- 编译所需的一切都在 `vendor/` 下，因此可离线构建，不依赖上游源码树。

## 许可

MIT，与上游 YY-Thunks 相同。vendored 源码衍生自 YY-Thunks —— Copyright (c) 2018
Chuyu-Team。详见 [LICENSE](LICENSE)。
