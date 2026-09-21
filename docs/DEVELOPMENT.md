# YY-Thunk-gnu —— YY-Thunks 的 MinGW-w64 移植

> **本文是完整开发文档**（原仓库根 `README.md` 的内容，原样保留）。
> 面向使用者的精简首页见仓库根 [`README.md`](../README.md)。

[YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks) 通过"动态加载 + 兜底实现"
让应用能在旧版 Windows 上使用新 API，但它本身**只支持 MSVC**。
本仓库把它移植到 **MinGW-w64（GCC）** 工具链，并改用 **CMake + Ninja** 组织构建。

设计目标（按优先级）：

1. **不改动上游**：`upstream/YY-Thunks` 保持只读，移植以"增量"形式叠加；
2. **上游可跟踪**：上游任何变化都能被感知、复核、低成本跟进；
3. **现代构建**：CMake + Ninja，一个 `preset` 就能出产物，无需 MSBuild / LibMaker.exe。

> **工具链范围：只支持 GCC(MinGW-w64)，不含 Clang。**
> Clang 从未被编译 / 链接 / 运行验证过。移植使用 `-fno-toplevel-reorder`（GCC 专有），
> 而上游靠"节内首元素是边界哨兵"实现自动注册数组、依赖定义顺序不被重排 ——
> Clang 无此选项，其行为需要实测才能确认。因此 configure 阶段会显式拒绝 Clang，
> 而不是给出一个未经验证的"支持"。

---

## 1. 快速开始

前置：MinGW-w64（本仓库在 GCC 16.2 + [w64devkit](https://github.com/skeeto/w64devkit) 上验证）、
CMake ≥ 3.20、Ninja、Python 3。

```powershell
# 配置（会自动生成移植覆盖层 + 预处理探测）
cmake --preset mingw-x64

# 构建静态库与冒烟测试
cmake --build build/x64

# 运行冒烟测试
.\build\x64\examples\smoke\yythunks_smoke.exe
```

产物：

| 产物 | 说明 |
| --- | --- |
| `build/x64/libyythunks.a` | 静态库，**单个目标文件**，等价上游的 `YY_Thunks_for_*.obj` |
| `build/x64/examples/smoke/yythunks_smoke.exe` | 冒烟测试，验证链接顺序与运行期解析链路 |

把它接到自己的目标上：

```cmake
add_subdirectory(path/to/YY-Thunk-gnu)
target_link_libraries(my_app PRIVATE yythunks)   # 必须排在系统导入库之前
```

> 链接顺序是关键：`yythunks` 要排在 `kernel32` / `user32` 等系统导入库**前面**，
> 链接器才会优先使用 YY-Thunks 提供的符号，而不是导入库里的同名声明的桩函数。
> CMake 的 `target_link_libraries` 天然满足这一点（系统库由 GCC 驱动追加在最后）。
>
> 即使导入库里也有同名函数（例如 `GetSystemTimePreciseAsFileTime` 在 MinGW 的
> `libkernel32.a` 里就有），顺序正确时 YY-Thunks 依然覆盖成功；顺序颠倒会在链接期
> 报 `multiple definition` 而**不是静默失去覆盖**。原理、实测证据与自检方法见
> [`PORTING.md` §3](PORTING.md)。

### 非 CMake 工程（Rust/Cargo、自研构建系统…）

`yythunks` 是普通的静态库，但因为是 GCC 编译产物，链接时需要注意两点：

1. **系统导入库**：上游把依赖库写在 `#pragma comment(lib, ...)` 里，GCC 会忽略，
   必须由链接方显式添加。配置阶段会**自动生成**两份等价产物：

   | 文件 | 用途 |
   | --- | --- |
   | `build/<preset>/yythunks-link-requirements.txt` | 机器可读的清单 |
   | `build/<preset>/yythunks-cargo-libs.rs` | 可直接粘贴进 Rust `build.rs` 的片段 |

   > 试过把依赖库嵌进产物（`.drectve` 节 + `-defaultlib:`，即 `#pragma comment(lib)`
   > 的底层形式），但 **GNU ld 不支持**该选项（实测报
   > `.drectve '-defaultlib:bcrypt' unrecognized`），所以只能落在构建系统里。
   > 好消息是链接顺序无关紧要，多写几个用不到的 `-l` 也不会出错。

2. **GCC 运行时**：`__chkstk_ms`、`__popcountdi2` 等来自 `libgcc`。
   `g++` 驱动会自动带上；非 `g++` 驱动（例如 Rust 的 linker 调用）需要自行加 `-lgcc`。

> 库本身**不引用 C++ 运行时**：移植层用 `-fno-exceptions -fno-unwind-tables` 编译，
> 因此不会出现 `__gxx_personality_seh0` / `_Unwind_Resume` 之类的未定义符号，
> 也不需要 `-lstdc++`。`GUID_NULL` 也在库内提供了弱定义，不强制使用者链接 `uuid`。

以 Rust 为例：

```rust
// build.rs
println!("cargo:rustc-link-search=native=<path-to>/YY-Thunk-gnu/build/x64");
println!("cargo:rustc-link-lib=static=yythunks");
// 下面这段由 CMake 生成，直接 include! 即可，无需手抄
include!("<path-to>/YY-Thunk-gnu/build/x64/yythunks-cargo-libs.rs");
```

等价的命令行形式：

```text
-L<path-to>/build/x64 -lyythunks -lgcc \
-lkernel32 -luser32 -ladvapi32 ... -lcfgmgr32 -ldnsapi -lnetapi32
```

### 可选配置项

| 变量 | 默认 | 说明 |
| --- | --- | --- |
| `YY_THUNKS_TARGET_MACRO` | `__WindowsNT6` | 上游 `YY_Thunks_Target`，即最低兼容系统。对应上游的 `YY_Thunks_for_Vista.obj` / `..._for_Win7.obj` 等。**不得低于 `__WindowsNT6`**（XP / 2003 不支持，见下） |
| `YY_THUNKS_PREFIXED_SYMBOLS` | `OFF` | 打开后符号带 `YY_Thunks_` 前缀（等价上游 lib 模式） |
| `YY_THUNKS_BUILD_EXAMPLES` | `ON` | 是否构建示例/冒烟测试 |

内置 preset：`mingw-x64`（Vista）、`mingw-x64-win10`（Win10）。

#### 指定目标系统版本

`YY_THUNKS_TARGET_MACRO` 就是上游的 `YY_Thunks_Target`，含义是**你希望支持的最低
系统**（不是编译所在的系统）。值越大，参与编译的 thunk 越少 —— 到 Win10 时几乎
全部关闭（实测 Vista 目标启用 259 个 thunk，Win10 1507 只有 26 个）。

```powershell
cmake --preset mingw-x64-win10               # 用 preset（各自独立构建目录）
cmake -S . -B build/win7 -G Ninja `
      -DYY_THUNKS_TARGET_MACRO=__WindowsNT6_1   # 或直接传
```

合法取值（解析自上游 `SharedDefs.h`，configure 时会校验）：

| 取值 | 对应系统 |
| --- | --- |
| `__WindowsNT5` | Windows 2000 —— **不支持** |
| `__WindowsNT5_1` / `_SP1` / `_SP2` / `_SP3` | Windows XP —— **不支持** |
| `__WindowsNT5_2` / `__WindowsNT5_2_SP1` | Server 2003 —— **不支持** |
| `__WindowsNT6` / `_SP1` / `_SP2` | Vista / Server 2008 ← **支持的最低点** |
| `__WindowsNT6_1` / `_SP1` | Windows 7 / Server 2008 R2 |
| `__WindowsNT6_2` / `__WindowsNT6_3` | Windows 8 / 8.1、Server 2012 / 2012 R2 |
| `__WindowsNT10_10240` … `__WindowsNT10_26100` | Win10 各版本 ~ Win11 24H2 / Server 2025 |

注意**没有** `__WindowsNT7`、`__WindowsNT8`、光秃秃的 `__WindowsNT10`。

> 这个值写错**不会自动报错**：预处理器把 `#if` 中未定义的标识符当成 `0`，于是
> `#if (YY_Thunks_Target < __WindowsNT6_2)` 恒真，**所有** thunk 都被编译进来
> （565 个），静默产出一个与预期完全不同的配置。因此 configure 阶段做了显式校验，
> 非法值直接 `FATAL_ERROR` 并列出全部合法取值。

---

## 2. 目录结构

```
YY-Thunk-gnu/
├─ upstream/YY-Thunks/          # 上游源码，只读。绝不修改
├─ upstream.lock.json           # 上游版本锁（commit + 每个文件的 SHA-256）
├─ portrule/                    # 移植规则：纯数据，人读（不含代码）
│  ├─ thunks.manifest           # 参与编译的模块清单（有序，可从上游 vcxproj 刷新）
│  └─ exclude.txt               # 本期未打通的模块及原因
├─ src/
│  ├─ compat/                   # 兼容垫片（手写，唯一需要人读的移植代码）
│  │  ├─ yy_thunks_prelude.h    # MSVC 扩展/CRT 宏/SAL -> GCC 映射
│  │  ├─ yy_thunks_postlude.h   # 依赖 Windows 类型的弱符号定义
│  │  └─ yy_thunks_sdk_gap.h    # 补齐 MinGW-w64 相对 Windows SDK 的缺口
│  └─ port/                     # portgen 产出：自足的移植源码树（可随包分发）
│     ├─ YY_Thunks_List.hpp         # 模块清单
│     ├─ YY_Thunks_EnableTable.hpp  # 守卫表（与目标无关，关键产物）
│     ├─ YY_Thunks_Declarations.hpp # 声明遍
│     ├─ YY_Thunks_Features.hpp     # Fallback 解析器映射
│     ├─ Thunks/**                  # 上游 Thunks 的完整副本（逐文件增量转换）
│     ├─ Shared/**                  # 上游 Shared 的完整副本
│     └─ portgen-stamp.json         # 生成戳记（判定能否复用）
├─ tools/
│  ├─ portgen.py                # 清单生成 + 守卫提取 + 预处理探测 + 增量转换
│  ├─ transforms.py             # 转换规则（代码形式，与生成器同目录便于 import）
│  ├─ target_matrix.py          # 全目标交叉验证（守卫表正确性）
│  ├─ check_static_imports.py   # 产物静态导入风险检查（CTest 用例）
│  ├─ audit_vs_upstream.py      # 与上游核对：函数名三层对照（CTest 用例）
│  └─ upstream_lock.py          # 上游版本锁定与漂移校验
├─ examples/smoke/              # 冒烟测试
├─ docs/
│  ├─ PORTING.md                # 移植了什么、为什么这么移植
│  └─ UPSTREAM.md               # 如何跟踪上游变化（重点）
├─ CMakeLists.txt
└─ CMakePresets.json
```

**关键点：目标由编译选项决定，生成物与目标无关。**

上游用 `#if (YY_Thunks_Target < __WindowsNT6_2)` 表示"这个 API 从 Win8 起原生就有"。
portgen 把这些守卫**提取成表达式**写进 `YY_Thunks_EnableTable.hpp`：

```cpp
#define YY_THUNK_GUARD_try_get_MFPutWorkItem2 (YY_Thunks_Target < __WindowsNT6_2)
```

因为 `__Version` 是预处理器算术表达式，调用点统一写成
`#if YY_THUNK_GUARD_try_get_X`，由 `-DYY_Thunks_Target=...` 在**编译期**求值。
所以 `src/port` 一份就够，多个构建目录（Vista / Win7 / Win10 …）共用。

> 上游（MSVC）靠 `__if_exists` 在编译期窥视整个 TU，天然不需要任何生成物；
> GCC 没有该扩展，"哪些 thunk 参与编译"必须先探一遍。守卫表就是把这次探测的
> 结果**归纳成与目标无关的表达式**，从而恢复了上游的"一份源码 + 编译选项"模型。

声明遍同样是目标无关的：它在**最低受支持目标**
（`YY_THUNKS_PORTGEN_TARGET_MACRO`，即 `YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO`，
当前为 `__WindowsNT6`）上探测一次 —— 按单调性，该目标的启用集合就是所有受支持
目标的**并集** —— 然后把每条声明包进它自己的守卫里。于是任何目标编译时只会激活
自己需要的那部分，而文件本身只需生成一次。

Fallback 映射（`YY_Thunks_Features.hpp`）同样与目标无关。**因此 portgen 的全部
产物都与当前目标无关**：换目标只改一个 `-D`，不触发生成。

生成戳记仍然保留，但守护的是**其他会改变生成结果的配置**（符号前缀模式
`YY_THUNKS_PREFIXED_SYMBOLS` 等）：若某个构建目录用不同配置重新生成过 `src/port`，
另一个目录构建时会**显式报错**并给出解决办法，而不是静默编译到不属于自己的生成物。

链接依赖库也不再手工维护：CMake 会**汇总上游全部 `#pragma comment(lib, ...)`**
（它们本身随目标变化，例如 `Shcore.lib` 只在 `YY_Thunks_Target >= __WindowsNT6_3`
时出现）并入依赖清单，因此换目标不会再漏库。

真正需要人工维护的只有三处：`portrule/`（纯数据：模块清单与排除项）、
`src/compat/`（垫片）、`tools/`（生成逻辑，含转换规则 `transforms.py`）。

> 目录划分是"**数据与代码分开**"：`portrule/` 只放人读的纯数据，`tools/` 放
> 可执行逻辑。`transforms.py` 虽以规则列表为主，但它终究是 Python 代码，且被
> portgen/audit/target_matrix 三个脚本 import，放在 `tools/` 下可直接导入，
> 不必再手工 `sys.path.insert`。
>
> 名字叫 `portrule` 而不是 `port`，是为了与生成物 `src/port/` 区分开 —— 后者
> 才是随包分发的完整源码树，两者极易混淆。

---

## 3. 当前状态

| 项目 | 状态 |
| --- | --- |
| 上游版本 | `bb5ea67`（2026-09-20） |
| 模块覆盖 | 上游 92 个模块中 **81 个**参与编译，11 个因 MinGW-w64 SDK 缺口暂缺（见 `portrule/exclude.txt`） |
| 需改动的上游文件 | **仅 10 个**（其余 130+ 个文件逐字使用上游源码） |
| 验证 | `mingw-x64`（Vista）全绿：静态库 + 冒烟测试 + 静态导入检查 |
| 验证 | `mingw-x64-win10`：库可构建（示例按设计跳过），与 Vista **共用同一份 `src/port`** |
| 明确不支持 | Windows XP / Server 2003 及更早（`< __WindowsNT6`）：那批 `target < N6` 的 thunk 在 GCC/MinGW-w64 下编不过（实测 37 个硬错误），configure 阶段会直接拒绝该目标 |
| 明确不支持 | 32 位（i686）：上游用 MSVC 的 `__identifier` 定义 stdcall 修饰的 IAT 符号（`_imp__Api@N`），GCC 无法用 `##` 拼出含 `@` 的记号（实测 1000+ 错误）；configure 阶段会直接拒绝 |

冒烟测试实际验证了：

* YY-Thunks 确实**顶替**了系统导入库里的同名符号
  （`WaitOnAddress` 在未链接 YY-Thunks 时是链接错误，链接后可用）；
* 运行期解析链路（延迟初始化 → 模块加载/动态解析 → 函数指针缓存）正常；
* Windows 8/10 才有的 API（高精度时间、线程描述、`WaitOnAddress`）在
  目标最低版本为 Vista 时仍能正确调用；
* 产物**没有静态导入**"可能不存在"的 DLL（见下）。

### 产物不静态依赖可选 DLL

本库用 `LoadLibrary` 做延迟加载，因此产物**不应静态导入**可选 DLL —— 否则缺少该
DLL 的系统（例如不含 Media Foundation 的 Server Core）会在进程启动阶段就失败。
项目就踩过这个坑（`MFPlat.DLL`），根因与修法见
[`PORTING.md` §3.3](PORTING.md)。

回归守护已接入 CTest，共 3 个用例：

```powershell
ctest --test-dir build/x64 --output-on-failure
#   upstream_audit    与上游核对函数名（三层对照）
#   smoke             运行期解析链路
#   static_imports    产物不得静态导入可选 DLL

# 也可以单独检查任意 exe/dll
python tools/check_static_imports.py yourapp.exe
```

`upstream_audit` 独立于转换规则本身，回答"上游源码在进入编译器之前有没有被改坏"：
名字分三层对照 —— 上游原文 → 覆盖层（转换后）→ 编译器实际展开结果，任一层有差集
即失败。真实故障（`esent.h` 用 `__MINGW_NAME_AW` 把 `JetGetTableColumnInfoW`
别名成 `...A`）恰恰出在最后一层，而编译、链接、冒烟测试**全都正常**，只有这一层
能发现。详见 [`PORTING.md` §3.4](PORTING.md)。

### 构建不依赖上游源码（自足模式）

`YY_THUNKS_SELF_CONTAINED`（默认开）让 portgen 把上游 `Thunks/` 与 `Shared/`
**整个复制**进 `src/port`，于是它成为一棵完整源码树（109 个文件，2.15 MB）。
配合"戳记匹配就跳过生成"，**只带 `src/` 就能用 cmake/ninja 构建** —— 不需要上游、
不需要 `tools/`、也不需要 Python：

```powershell
# 验证：把上游与 tools 都移开后从零构建
Rename-Item upstream/YY-Thunks/src src-hidden; Rename-Item tools tools-hidden
cmake --preset mingw-x64; cmake --build build/x64; ctest --test-dir build/x64
#   -- 复用已有生成物（戳记匹配，未重新生成）
#   -- 跳过 upstream_audit / static_imports / yythunks_port_stamp（都依赖 Python）
#   100% tests passed out of 1        # 只剩 smoke
Rename-Item upstream/YY-Thunks/src-hidden src; Rename-Item tools-hidden tools
```

做到这一点有两个前提，都不是免费的：

- **戳记校验用纯 CMake 实现**（`file(READ)` + `string(JSON)` + `file(SHA256)`），
  不调用 Python。判据（编译宏、自足性、逐文件 SHA-256）全都写在
  `src/port/portgen-stamp.json` 里。
- **`tools/portgen.py` 只是"可选"输入**：它存在时纳入指纹（升级工具会触发重新
  生成），不存在则跳过 —— 否则分发场景下生成物会永远被判定为过期。

注意 Python 相关的步骤会自动跳过而不是失败：`upstream_audit`、`static_imports`
两个 CTest 用例，以及 `yythunks_port_stamp` 校验目标。它们是**开发期**守护，
分发场景下本来也不适用。

为什么必须"整个复制"而不能只复制被转换的文件：实测编译会从上游读 **72 个**头
文件，其中包含 `ThreadRunner.h` 这类**不在模块清单里**的文件 —— 只按清单复制必
然漏。因此物化单位是**目录**，不是清单。

上游只在两种情况下需要：**重新生成**（`cmake --build <dir> --target
yythunks_regen`，如升级上游）和**与上游核对**（`upstream_audit`）。二者缺失都不
会让构建失败；`upstream_audit` 在找不到上游时会按设计**不注册**（没有上游无从
核对，让它失败只会把"没有上游"伪装成移植缺陷）。

> `src/port` 因此**不再 gitignore**，可随源码树分发。上游基线仍由
> `tools/upstream_lock.py` 守护（commit + 每个文件的 SHA-256）。

自足化额外带来一个收益：审计工具的第 1 层名字核对从 13 个文件扩展到**全部
81 个模块** —— 原先 68 个模块走的是"未物化=逐字一致"的假设，并未真正比对。

### 同步一份源码到 Rust crate（thunks-gnu）

`YY_THUNKS_GNU_DIST_DIR`（默认 `<repo>/thunks-gnu/vendor/YY-Thunks-gnu`）指向 Rust
crate 里自带的那份源码。**portgen 生成成功后会自动同步** `src/` 过去 —— 即生成物
`port/` 加上手写垫片 `compat/`。

目录叫 **`vendor`** 而不是 `assets`：这里放的是**要交给编译器编译的 C/C++ 源码**
（112 个文件），不是运行时加载的数据。`vendor` 的语义是"第三方来源的代码副本，
纳入本仓库以便独立构建"，与 `cargo vendor` 一致；叫 `assets` 容易被理解成
"改它不必重新编译"。

同步写在 `tools/portgen.py` 里（`--dist-dir`），而不是构建脚本里 —— 因为重新生成
有三条路径：CMake configure、`yythunks_regen` 目标、手工运行 `portgen.py`。挂在
"生成成功之后"这一个时机上，三条都会带上同步；写在 configure 分支里则后两条会漏，
生成物更新了而副本仍是旧的，且毫无提示。

```powershell
python tools/portgen.py dist --dist-dir thunks-gnu/vendor/YY-Thunks-gnu  # 只同步
cmake --build build/x64 --target yythunks_dist                          # 等价的便捷目标
```

> 该变量是 CMake 的 `CACHE PATH`：**改默认值不会覆盖已缓存的旧值**。若改了这个
> 目录的位置，必须清理构建目录（或显式传 `-DYY_THUNKS_GNU_DIST_DIR=<新路径>`），
> 否则仍会同步到旧路径、把已被移走的目录重新创建出来。

`port/` 采用**整目录替换**而不是增量覆盖：它完全由生成器产出，旧副本残留会留下
早已不存在的模块（自足化之前那里只有 17 个文件，现在是 95 个）。`compat/` 是手写
的，直接覆盖即可。

---

## 4. 已知限制

1. **SEH 降级**：GCC/MinGW 不提供 `__try`/`__except`。移植层把 `__try` 体原样执行、
   `__except` 体变成死代码。正常路径行为与上游一致；遇到畸形 PE、探测性内存读取等
   异常场景时不再被捕获，而是直接崩溃。见 `PORTING.md`。
2. **11 个模块未启用**，全部是 MinGW-w64 SDK 头缺失/缺陷导致，逐条记录了原因。
3. **`DllMainCRTStartup.hpp` 未启用**：它依赖 MSVC 的 `/alternatename` 弱别名，
   且逻辑只在目标低于 Vista 时生效。默认目标为 Vista，因此无功能损失。
4. **x86 目标未验证**：x86 分支还额外需要 `__identifier` / IAT 符号的等价实现。

---

## 5. 延伸阅读

* [`PORTING.md`](PORTING.md)：移植原理、做了哪些映射、每个决策的原因。
* [`UPSTREAM.md`](UPSTREAM.md)：**如何跟踪上游变化**，以及上游变更后要做的事。

## 6. 许可

移植层代码沿用上游 YY-Thunks 的 MIT 许可；上游源码版权归 Chuyu-Team 所有。
