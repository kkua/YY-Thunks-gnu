# 如何跟踪上游变化

这是本仓库设计的**核心问题**：上游 YY-Thunks 由 Chuyu-Team 持续维护
（新 API 兜底、修复、重构都很频繁），移植层必须做到：

* 上游动了什么，**能立刻知道**；
* 上游动了之后，移植层要做什么，**是确定性的、廉价的**；
* 移植层与上游的"偏差"是**显式、可审计、最小化**的，而不是一份失控的 fork。

---

## 1. 三道防线

| 防线 | 工具 | 能发现什么 |
| --- | --- | --- |
| 版本锁 | `tools/upstream_lock.py` | 上游 commit 变了；具体哪些文件被新增/删除/修改 |
| 转换规则自检 | `tools/portgen.py` | 某条文本转换**不再匹配**（上游把那段代码改掉了） |
| 编译 | CMake 构建 | 上游新增写法（新的 `__if_exists`、新签名）导致编译失败 |

三者都是"大声失败"，不存在静默偏差。

---

## 2. 上游源码的存放方式

`upstream/YY-Thunks` 是**只读**的 git 仓库（可以是 submodule，也可以是普通克隆）。
`upstream.lock.json` 记录：

```json
{
  "repository": "https://github.com/Chuyu-Team/YY-Thunks.git",
  "commit": "bb5ea67de93c323fd3c77d04946dd83497f06ea8",
  "commit_date": "2026-09-20 12:21:16 +0800",
  "commit_subject": "Fea，添加GetNamedPipeClientProcessId、GetNamedPipeServerProcessId",
  "file_count": 169,
  "files": { "src/Thunks/kernel32.hpp": "…sha256…", "…": "…" }
}
```

**移植层绝不修改这里。** 连 `#include` 都不改——所有改动都通过覆盖层生效
（见 §3）。

---

## 3. 偏差只有三种形式，分别声明在 `portrule/` 与 `tools/transforms.py`

纯数据放 `portrule/`，以代码表达的转换规则放 `tools/transforms.py`（它与生成器
同目录，便于 import）。这样"改数据"与"改逻辑"在版本历史里一眼可辨。

### 3.1 模块清单：`portrule/thunks.manifest`

上游用 MSBuild Target 从 `YY-Thunks.UnitTest.vcxproj` 的 `ClInclude` 顺序生成
`YY_Thunks_List.hpp`。移植层用 `portrule/thunks.manifest` 表达同一份有序清单。

上游新增模块时，刷新即可——**上游的登记顺序会被原样保留**
（顺序影响单 TU 内的声明/定义顺序，不能随便改）：

```powershell
python tools/portgen.py manifest --refresh
```

### 3.2 暂不支持的模块：`portrule/exclude.txt`

与清单分离，好处是：刷新清单不会丢掉排除记录，而上游新增模块会立刻暴露出来
（新模块没被排除 → 会参与编译 → 编译失败 → 我们立刻知道要适配它）。

每条排除都写了原因（MinGW-w64 SDK 缺哪个头/哪个类型）。
移掉一行即可让该模块重新参与编译。

### 3.3 源码增量：`tools/transforms.py`

分两类：

* **literal / regex 规则**：`pattern` 在某文件中找不到时，
  `portgen` 会报"上游漂移"并给出规则原因。
  例如"把 `__if_exists(try_get_ClosePackageInfo)` 块改写成宏判断"这类精确替换。
* **`kind="seh"`**：把 `__try`/`__except` 降级为普通语句（见 `docs/PORTING.md` §2.5）。

如果某个文件的改动复杂到正则/文本替换难以表达，可以放一份整文件替代到
`portrule/overrides/`；同样会被漂移检查覆盖。

**没有出现的文件 = 逐字使用上游源码。** 当前只有 10 个文件出现在
`tools/transforms.py` 里。

---

## 4. 上游更新时的标准流程

```powershell
# 1) 拉取上游
git -C upstream/YY-Thunks fetch origin
git -C upstream/YY-Thunks checkout <new-ref>

# 2) 看上游到底动了什么
python tools/upstream_lock.py --diff

# 3) 刷新模块清单（上游新增/删除了 thunk 模块时）
python tools/portgen.py manifest --refresh

# 4) 重新生成移植层；这里会告诉你哪条转换规则失效了
python tools/portgen.py all

# 5) 重新配置 + 构建 + 跑测试
cmake --preset mingw-x64
cmake --build build/x64
.\build\x64\examples\smoke\yythunks_smoke.exe

# 6) 更新版本锁
python tools/upstream_lock.py
```

第 4 步的输出是重点：

```
[portgen][警告] Thunks/YY_Thunks.cpp: 规则未匹配 -> CloseHandle 阴影替换（避免 GetYY_ThunksSharedData 自递归）
[portgen] 检测到上游漂移（tools/transforms.py 中的规则未匹配）。
```

此时按提示打开 `tools/transforms.py`，对照上游新写法更新该条规则即可。

### 常见变更类型与处理方式

| 上游变更 | 需要做什么 |
| --- | --- |
| 新增一个 API 兜底 | **通常什么都不用做**。新头文件会自动进入清单并参与编译；只有用到新的 MSVC 专有构造时才会失败 |
| 新增模块文件 | `manifest --refresh`；若依赖 MinGW 缺失的 SDK 头，加入 `exclude.txt` 并写原因 |
| 改了 `__DEFINE_THUNK` 的调用形式 | 检查 `tools/portgen.py` 的预处理探测是否仍能解析（会报错，不会静默） |
| 改了 `YY_Thunks.cpp/h` 的宏或结构 | 大概率是某条 literal 规则失效，按警告更新 |
| 新增 `__if_exists` 用法 | 加一条转换规则；判定条件优先用 `YY_THUNK_HAS_try_get_X`（由探测精确给出） |
| 新增 SDK 依赖 | 补 `CMakeLists.txt` 的 `target_link_libraries`（`#pragma comment(lib)` 会被 GCC 忽略） |
| 新增 MinGW 缺失的类型/函数 | 补 `src/compat/yy_thunks_sdk_gap.h` |

---

## 5. 为什么这样组织（设计取舍）

* **不 fork 源码**：fork 会随时间发散，且上游每次提交都要手工 merge。
  覆盖层方案让"偏差"始终是一张可审阅的清单。
* **不改上游一行**：保证 `upstream/` 可以随时 `git checkout` 到任意版本，
  `--diff` 的语义也才准确（compare 的是上游自身）。
* **不把改动藏在生成脚本里**：所有偏差都写在 `port/` 的声明式文件里，
  生成物落进构建目录（`${CMAKE_BINARY_DIR}/port`），人只需要读 `port/` 和 `src/compat/`。
* **优先用编译器能力而非文本替换**：能靠宏/编译选项解决的绝不改源码
  （`-fno-toplevel-reorder`、垫片宏……）；
  文本替换只用于宏确实表达不了的地方，且必须可校验。
* **探测而非猜测**：哪些 thunk 参与编译、每个 thunk 的真实签名，
  都由"跑一次真实预处理器"得到，而不是靠移植层自己解析 `#if`。
  这是让声明与定义永不漂移的关键。
