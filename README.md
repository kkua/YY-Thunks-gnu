# YY-Thunk-gnu

把 [YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks) 移植到 **MinGW-w64（GCC）** ——
让应用照常使用新 API，却仍能在旧版 Windows 上运行。

上游只支持 MSVC。本仓库在**不改动上游一行源码**的前提下，用"垫片 + 增量转换"把它
搬到 GCC 工具链，并改用 CMake + Ninja 组织构建。

## 快速开始

前置：MinGW-w64（GCC ≥ 13，本仓库在 GCC 16.2 上验证）、CMake ≥ 3.20、Ninja。

```powershell
cmake --preset mingw-x64      # 配置（自动生成移植层 + 预处理探测）
cmake --build build/x64       # 构建静态库与冒烟测试
ctest --test-dir build/x64    # 运行测试
```

产物是 `build/x64/libyythunks.a` —— **单个目标文件**的静态库，等价上游的
`YY_Thunks_for_*.obj`。链接时把它放在系统导入库之前即可：

```powershell
g++ yourapp.cpp -o yourapp.exe -Lbuild/x64 -lyythunks -lkernel32 -luser32 ...
```

非 CMake 工程（Rust/Cargo、自研构建系统）可直接读取构建期导出的
`build/x64/yythunks-link-requirements.txt` 或 `yythunks-cargo-libs.rs`
取得所需的系统库清单 —— GCC 会忽略上游的 `#pragma comment(lib)`，这份清单只能
由构建期算出来。

**构建不需要 Python、不需要上游源码、也不需要 `tools/` 下的脚本。**
（重新生成移植层时才需要，见下。）

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
| `thunks-gnu/` | Rust crate 封装（用 `cc` 编译同一套源码） |
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
