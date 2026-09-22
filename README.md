# YY-Thunk-gnu

把 [YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks) 移植到 **MinGW-w64（GCC）** ——
让应用照常使用新 API，却仍能在旧版 Windows 上运行。

上游只支持 MSVC。本仓库在**不改动上游一行源码**的前提下，用"垫片 + 增量转换"把它
搬到 GCC 工具链，并改用 CMake + Ninja 组织构建。

## 快速开始

前置：MinGW-w64（GCC ≥ 13，本仓库在 GCC 16.2 上验证）、CMake ≥ 3.20、Ninja。

### 构建本库

```powershell
# 1) 配置：生成构建系统（同时自动生成移植层 + 做预处理探测）
cmake --preset mingw-x64

# 2) 构建：产出静态库与冒烟测试程序
cmake --build build/x64            # 等价 cmake --build --preset mingw-x64

# 3) 测试：运行 3 个用例
ctest --test-dir build/x64         # 等价 ctest --preset mingw-x64
```

产出物：

| 路径 | 说明 |
| --- | --- |
| `build/x64/libyythunks.a` | **静态库**，单个目标文件，等价上游 `YY_Thunks_for_*.obj` |
| `build/x64/yythunks-link-requirements.txt` | 该目标实际需要链接的系统库清单 |
| `build/x64/examples/smoke/yythunks_smoke.exe` | 冒烟测试程序 |

- 想直接构建 Win10 1507 目标：把上面三条命令里的 `mingw-x64` 换成 `mingw-x64-win10`，产物落在 `build/x64-win10/`。
- 重新生成移植层（需要 Python 3 + 上游源码，见「关于 `upstream/`」）：`cmake --build build/x64 --target yythunks_regen`。

### 使用产物

链接时把静态库放在系统导入库之前即可：

```powershell
g++ yourapp.cpp -o yourapp.exe -Lbuild/x64 -lyythunks -lkernel32 -luser32 ...
```

非 CMake 工程（自研构建系统）可直接读取构建期导出的
`build/x64/yythunks-link-requirements.txt` 取得所需的系统库清单 —— GCC 会忽略上游
的 `#pragma comment(lib)`，这份清单只能由构建期算出来。

**构建不需要 Python、不需要上游源码、也不需要 `tools/` 下的脚本。**
（只有上面那条 `yythunks_regen` 才需要。）

### 选择最低兼容系统（目标版本）

目标版本在**配置期（编译期）**决定，不是 build/link 阶段、更不是运行期参数。生成物与目标无关——同一份源码树可同时产出多个目标的库，每个目标用独立构建目录即可。通过缓存变量 `YY_THUNKS_TARGET_MACRO` 在 `cmake` 配置命令行用 `-D` 传入（等价上游 `YY_Thunks_Target`，即"希望支持的最低系统"）：

```powershell
cmake --preset mingw-x64                 # Vista（默认，可省略；预设已指定 gcc/g++）
# 其它目标：显式指定 gcc/g++，避免机器上装了别的编译器被误选
cmake -S . -B build/win7   -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DYY_THUNKS_TARGET_MACRO=__WindowsNT6_1
cmake -S . -B build/win8   -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DYY_THUNKS_TARGET_MACRO=__WindowsNT6_2
cmake -S . -B build/win8_1 -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DYY_THUNKS_TARGET_MACRO=__WindowsNT6_3
cmake -S . -B build/win10  -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DYY_THUNKS_TARGET_MACRO=__WindowsNT10_10240   # 等价 preset mingw-x64-win10
```

- **合法取值**（从上游 `SharedDefs.h` 解析）：`__WindowsNT6`(+`_SP1`/`_SP2`) Vista、`__WindowsNT6_1`(+`_SP1`) Win7、`__WindowsNT6_2`/`__WindowsNT6_3` Win8/8.1、Win10~Win11 为 `__WindowsNT10_10240`/`14393`/`15063`/`16299`/`17134`/`19041`/`20348`/`26100`。**不支持** XP/2003（<`__WindowsNT6`），也**不存在** `__WindowsNT7`/`__WindowsNT8`/光秃 `__WindowsNT10`；注意上游**未定义** `__WindowsNT10_22000`/`22621` 等，传入会报非法。
- 写错会 `FATAL_ERROR` 并列出全部合法取值（不会静默编译成错误配置）。
- 同一构建目录改宏会触发移植层戳记校验；建议各目标独立目录。

### 各目标需链接的系统库（实测）

上游把依赖写在 `#pragma comment(lib)`，GCC 会忽略，所以链接清单只能由构建期算出：CMake 把入口 TU 按当前目标实际编译、`nm -u` 取未定义符号，再为每个符号挑第一个真正定义它的导入库。下表是各目标**最终**清单（生成物见 `build/<目标>/yythunks-link-requirements.txt`）。每个 ✓ 都确由该导入库解析了某个未定义符号，**无冗余 `-l`**：

| 导入库 | Vista (6.0) | Win7 (6.1) | Win8 (6.2) | Win8.1 (6.3) | Win10 10240 | Win10 19041+/Win11 |
| --- | :---: | :---: | :---: | :---: | :---: | :---: |
| `kernel32` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `ntdll` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `user32` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `gdi32` | ✓ | ✓ | ✓ | ✓ | ✓ | |
| `uuid` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `iphlpapi` | ✓ | ✓ | ✓ | ✓ | ✓ | |
| `advapi32` | ✓ | ✓ | ✓ | ✓ | | |
| `ole32` | ✓ | ✓ | | | | |
| `powrprof` | ✓ | ✓ | | | | |
| `winhttp` | ✓ | ✓ | | | | |
| `bcrypt` | ✓ | | | | | |
| `cfgmgr32` | ✓ | | | | | |
| `dxgi` | | ✓ | ✓ | | | |
| `dcomp` | | | ✓ | ✓ | | |
| `shcore` | | | | ✓ | ✓ | |
| **合计** | **12** | **11** | **9** | **9** | **7** | **4** |

> `gcc` 运行时（`__chkstk_ms`/`__popcountdi2` 等）始终需要：g++ 驱动自动加；非 g++ 驱动须在链接命令显式加 `-lgcc`，不计入上表。
>
> 规律：目标越新，参与编译的兜底分支越少，所需库越少（Vista 12 → Win10 19041+/Win11 4）。值得注意的是 **Win10 10240 反而比 19041+ 多 3 个库**（`gdi32`/`iphlpapi`/`shcore`）——10240 仍在编译一批后来版本不再需要的 thunk。`shcore` 仅在 Win8.1~10240 出现、`dxgi`/`dcomp` 仅在 Win7~Win8.1 出现，正是上游 `#pragma comment(lib, "Shcore.lib")` 等随目标变化、被 probe 正确捕获的体现。`__WindowsNT10_14393`/`15063`/`16299`/`17134` 未逐一实测，按 10240（7 个）处理（降级发生在 19041）；`__WindowsNT10_22000`/`22621` 上游未定义，会报非法。

## 特性

- **不改动上游**：`upstream/` 只读，所有偏差以增量形式叠加并逐条记录
- **构建自足**：`src/port/` 是一棵完整的源码树，随包分发，拿到就能编
- **目标由编译选项决定**：`-DYY_THUNKS_TARGET_MACRO=__WindowsNT6_1` 即切换最低
  兼容系统；生成物与目标无关，多个目标共用同一份
- **产物不静态依赖可选 DLL**：缺少 Media Foundation / DirectX 等组件的系统不会在
  **进程启动阶段**失败，只在实际调用时返回错误
- **上游可跟踪**：`tools/upstream_lock.py` 锁定 commit 与逐文件 SHA-256，上游任何
  改动都能被感知；`tools/audit_vs_upstream.py` 三层核对函数名，防止转换改坏

## 支持范围

| 项 | 支持情况 |
| --- | --- |
| 编译器 | GCC (MinGW-w64)。**不支持 Clang** —— 移植依赖 GCC 专有的 `-fno-toplevel-reorder`，Clang 未经验证，configure 阶段显式拒绝 |
| 架构 | x86_64。32 位需要上游 MSVC `__identifier` 的等价实现，未打通，configure 阶段拒绝 |
| 最低系统 | Windows Vista / Server 2008 起。更低的目标（XP/2003）在 GCC 下编不过，configure 阶段拒绝 |

## 目录速览

| 路径 | 说明 |
| --- | --- |
| `src/compat/` | 手写垫片（MSVC 扩展 / SAL / CRT 宏 → GCC 语义映射） |
| `src/port/` | **生成物**：自足的移植源码树，可随包分发 |
| `portrule/` | 移植规则纯数据：模块清单、暂不支持的模块及原因 |
| `tools/` | 生成器与检查脚本（`portgen.py`、`transforms.py` 等） |
| `upstream/` | 上游源码，只读。仓库内不含其内容，构建也不需要 |
| `thunks-gnu-rs/` | Rust crate 封装（用 `cc` 编译同一套源码） |
| `examples/smoke/` | 冒烟测试 |

## 关于 `upstream/`

主仓库**不包含**上游源码（它是独立的 git 仓库，约 32 MB）。构建不需要它 ——
`src/port/` 已含完整副本。

只有"升级上游 / 重新生成移植层"时才需要取回：

```powershell
git clone https://github.com/Chuyu-Team/YY-Thunks.git upstream/YY-Thunks
git -C upstream/YY-Thunks checkout bb5ea67   # upstream.lock.json 锁定的版本
python tools/upstream_lock.py --verify       # 校验基线
cmake --build build/x64 --target yythunks_regen
```

另需 Python 3。完整流程见 [`docs/UPSTREAM.md`](docs/UPSTREAM.md)。

## 文档

| | |
| --- | --- |
| [`docs/DEVELOPMENT.md`](docs/DEVELOPMENT.md) | 完整开发文档：原理、目录结构、当前状态、已知限制 |
| [`docs/PORTING.md`](docs/PORTING.md) | 移植了什么、每个决策为什么这么做 |
| [`docs/UPSTREAM.md`](docs/UPSTREAM.md) | 如何跟踪上游变化 |

## 许可

MIT，见 [LICENSE](LICENSE)。

移植层沿用上游 YY-Thunks 的 MIT 许可。`src/port/` 与 `thunks-gnu/vendor/` 中含
上游源码的派生副本，其版权归 **Chuyu-Team** 所有。
