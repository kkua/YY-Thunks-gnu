# 移植说明（MSVC → MinGW-w64）

本文说明 YY-Thunks 究竟是**卡在哪些 MSVC 特性上**、移植层分别怎么解决，
以及每个方案的选择理由与副作用。

---

## 1. 总体策略：不改上游，用"垫片 + 增量转换"叠加

上游有 130+ 个文件，其中绝大多数是纯业务逻辑（各 API 的兜底实现），
和编译器无关。真正依赖 MSVC 的只有几类构造。因此移植采用三层：

| 层 | 位置 | 解决的问题 | 是否改动上游源码 |
| --- | --- | --- | --- |
| 垫片头 | `src/compat/` | 宏能表达的差异（CRT 宏、SAL、调用约定别名…） | 否 |
| 增量转换 | `tools/transforms.py` | 宏表达不了的差异（`__if_exists`、SEH、节属性…） | 只输出到 `src/port/` |
| 预处理探测 | `tools/portgen.py` | 上游"两遍包含"编译模型在 GCC 下无法复现 | 否 |

结果：只有 **10 个上游文件**需要被物化成覆盖层，其余直接编译上游原文。

---

## 2. 逐项问题与解法

### 2.1 `__declspec(allocate(".Sec"))` —— 节即数组

上游用"自定义节 + 链接器按节名排序"实现自动注册数组：

```
.YYThr$AAA  ->  各 thunk 的初始化函数指针（预载 DLL 用）
.YYThr$AAB  ->  反初始化函数指针
.YYThu$AAA  ->  模块句柄缓存
.YYThu$AAB  ->  函数指针缓存
```

MSVC 靠 `__declspec(allocate())` 放置、靠 `/merge` 合并。
GCC 下 `__declspec(allocate)` 会退化成"未知属性，仅告警并静默忽略"。

**实测结论**：GNU ld 会自动把 `.Name$AAA` / `.Name$AAB` 按 `$` 后缀排序并合并成
同一个输出节 `.Name`，与 MSVC 行为一致。因此只要把
`__declspec(allocate("X"))` 映射为 `__attribute__((section("X")))` 即可，
上游那套机制原样可用。

配套约束：

* 必须加 `-fno-toplevel-reorder`：否则 GCC 会重排顶层定义，
  破坏"节的首元素是边界哨兵"这一前提（实测不加会乱序）。
* `.YYThr$AAA` 里的 `pInit_*` 必须去掉 `const`：该节的哨兵是非 const（可写）
  的，GCC 不允许把 const 变量放进可写节（`section type conflict`）。

### 2.2 "两遍包含"编译模型与 `__if_exists`

上游 `YY_Thunks.cpp` 把模块清单**包含两遍**：

* 第一遍：`__DEFINE_THUNK` 展开成"API 声明 + `try_get_*` 前置声明"，
  并以 MSVC 专有的 `__if_not_exists(try_get_X)` 结尾来**跳过紧随其后的函数体**；
* 第二遍：`__DEFINE_THUNK` 展开成真正的定义。

这样任意 thunk 头都能调用在**后面**才定义的其他 thunk 的 `try_get_*`，也能反向调用。

GCC 既没有 `__if_exists` / `__if_not_exists`，也没有等效的"跳过花括号块"手段
（试过"永不实例化的模板函数体"，但第一遍里 `internal::` 助手尚未定义，
函数体仍会被解析并报错）。

**解法**：把第一遍**整体搬到预处理阶段一次生成**。

```
tools/portgen.py
  ├─ 生成 YY_Thunks_List.hpp        （模块清单）
  ├─ 用真实编译器 -E 跑一遍探测：
  │     __DEFINE_THUNK 被重定义为"展开出完整声明 + 标记"
  ├─ 抽取标记之间的文本 -> YY_Thunks_Declarations.hpp
  │     （API 声明 + 函数类型 typedef + try_get_* 前置声明）
  └─ 抽取到的函数名集合 -> YY_Thunks_Enabled.hpp
        （YY_THUNK_HAS_try_get_X 宏，用于替代 __if_exists）
```

然后 `YY_Thunks.cpp` 的第 1 遍被替换为一次 `#include "YY_Thunks_Declarations.hpp"`。

这个方案的好处：

* 声明与定义来自**同一份宏实参**，天然不会漂移；
* 声明顺序、`#if` 门控完全由真实预处理器的结果决定，与上游语义一致；
* 上游若新增同类写法，编译会立刻报错，不会静默出错。

踩过的坑（已在实现里修正）：

* **命名空间**：预处理输出会把 `namespace YY::Thunks` 拍平，生成的声明头必须显式
  补回该命名空间，否则声明落到全局作用域、与定义形成重载/冲突。
* **类型可见性**：上游第一遍是在各 thunk 头自身的 `#include <sdk.h>` 之后展开的，
  所以声明里能直接使用 `PACKAGE_INFO_REFERENCE`、`MIB_IF_ROW2` 这类 SDK 类型。
  生成的声明头因此需要把各模块依赖的 SDK 头**一并前置**。
  但不能前置所有头：`knownfolders.h`、`shlguid.h` 没有 include guard，
  YY 自己的 `InterlockedQueue.h` 等依赖 TU 内的定义顺序（见 `_PREFIX_INCLUDE_DENY`）。
* **顺序**：必须先物化覆盖层，再做预处理探测；否则探测读到的是未转换的上游源码，
  生成的声明会与真正编译时不一致。

### 2.3 `__if_exists(YY::Thunks::Fallback::try_get_X)` —— 自定义解析器

上游用它在宏里判断"某 API 是否存在自定义解析器"（按序号/偏移解析导出表等）。

**解法**：`portgen` 扫描各模块中 `namespace YY::Thunks::Fallback` 里形如
`try_get_X(const ProcInfo&)` 的定义，生成一张显式映射表
`YY_Thunks_Features.hpp`：

```cpp
#define YY_THUNK_RESOLVER_GetProcAddress &YY::Thunks::Fallback::try_get_GetProcAddress
#define YY_THUNK_RESOLVER_GetTickCount64 nullptr
```

宏里改为 `_CRT_CONCATENATE(YY_THUNK_RESOLVER_, _FUNCTION)`。
映射表同时覆盖 `_YY_APPLY_TO_LATE_BOUND_FUNCTIONS` 登记的延迟绑定函数。

### 2.4 `__if_exists(try_get_X)` —— 遮蔽式替换

上游在函数体里用这种写法避免自递归/死锁：

```cpp
__if_exists(YY::Thunks::try_get_CloseHandle) {
    const auto CloseHandle = YY::Thunks::try_get_CloseHandle();   // 解析原生指针
}
CloseHandle(h);   // 未启用 thunk 时，这里就是 SDK 导入
```

共 13 处（`YY_Thunks.cpp/h`、`api-ms-win-appmodel-runtime.hpp`、`user32.hpp`）。

**解法**：`portgen` 的探测结果已经知道每个 thunk 是否参与编译，因此逐处改写为
`#if defined(YY_THUNK_HAS_try_get_X)` / `#else` / `#endif`。
这 13 处用**精确文本替换**表达（而不是正则），一旦上游改动就会报"规则未匹配"，
不会静默失效。

### 2.5 结构化异常（SEH）

GCC/MinGW 不提供 `__try` / `__except`。上游只在"防御畸形 PE、探测性内存读取"
这类场景使用（`YY_Thunks.h/cpp`、`DllMainCRTStartup.hpp`、fibers / libraryloader /
Iphlpapi / ntdll 共 7 个文件）。

**解法**：源码级文本改写（`kind="seh"`）：

```cpp
__try { A }  __except (FILTER) { B }
   ↓
/*__try*/ { A }  /*__except*/ if (false) { B }
```

* 正常路径行为与上游**完全一致**；
* 异常路径不再被捕获，而是直接崩溃。

**为什么不定义 `__try` / `__except` 宏**：libstdc++ 的
`bits/exception_defines.h` 已经把 `__try` 定义成 `try` 的别名（异常关闭时是
`if (true)`），宏覆盖会破坏 STL 的异常处理。这是实测踩到的坑。
用 VEH 模拟 `__except` 会引入全局异常回调、不可接受的性能开销，以及与宿主程序
异常处理策略冲突的风险，收益不成比例。

### 2.6 其他映射（都在垫片里，不改源码）

| MSVC | MinGW-w64 | 说明 |
| --- | --- | --- |
| `__pragma(x)` | `_Pragma(#x)` | |
| `_CRT_CONCATENATE` / `_CRT_STRINGIZE_` / `_CRT_WIDE_` | 自备等价定义 | MinGW 只提供部分变体 |
| `__foreinclude(x)` | `((void)(x))` | 上游用它防止节内容被优化掉 |
| `min` / `max` 宏 | 自备 | MinGW 的 minwindef.h 在 C++ 下关掉了它们，且 windows.h 默认 `NOMINMAX` |
| `UNREFERENCED_PARAMETER(x)` | `((void)(x))` | MinGW 版是 `{(x)=(x);}`，对 const/函数指针会编译失败 |
| `_fastcall` / `_cdecl` / `_stdcall` | `__fastcall` 等 | MinGW 只有双下划线版本 |
| `0x...i64` | `0x...LL` | MSVC 的整型字面量后缀 |
| `__declspec(selectany)` | `__attribute__((selectany))` | MinGW 已由 `__declspec` 宏转好 |
| `winnt.h` 的 `UFIELD_OFFSET` | `__builtin_offsetof` | `&((T*)0)->f` 会被 GCC 的常量求值拒绝，而上游拿它做 `static_assert` |
| WDK/旧式 SAL（`__drv_*`、`__in`、`__out_bcount_opt` …） | 空 | 分析器注解，无运行时语义 |
| `#pragma comment(lib, ...)` | 忽略（改为 CMake `target_link_libraries`） | |
| `#pragma section` / `#pragma comment(linker, "/merge:...")` | 忽略（ld 自动按 `$` 排序合并） | |
| `#pragma detect_mismatch` | 忽略 | |
| LibMaker.exe 生成的弱符号 | `src/compat/yy_thunks_postlude.h` 里的弱定义 | 见下 |

### 2.7 可选弱符号（替代 LibMaker FixObj）

上游用 `LibMaker.exe FixObj /WeakExternFix:...` 把
`__security_cookie`、`__acrt_atexit_table`、`__YY_Thunks_Disable_Rreload_Dlls`、
`__pfnYY_Thunks_CustomLoadLibrary` 等做成"可选弱符号"，使用者可以强定义来定制。

GCC 侧改为**弱定义 + 默认值**，语义等价（强定义会覆盖弱定义）：

```cpp
extern "C" __attribute__((weak)) UINT_PTR __security_cookie = 0;
extern "C" __attribute__((weak)) void* __acrt_atexit_table = nullptr;
extern "C" __attribute__((weak)) BOOL __YY_Thunks_Disable_Rreload_Dlls = FALSE;
```

也提供了 MinGW 完全没有的 `InterlockedCompareExchange128`（内联汇编实现，
语义与 MSVC 固有函数一致，用于 `api-ms-win-core-kernel32-legacy.hpp`）。

### 2.8 MinGW-w64 SDK 缺口

`src/compat/yy_thunks_sdk_gap.h` 集中补齐 MinGW-w64 相对 Windows SDK 缺失的
**类型/枚举/宏**（不是函数实现）：

* `CREATEFILE3_EXTENDED_PARAMETERS`（复用 CreateFile2 的结构体）
* `MIB_IF_ENTRY_LEVEL`
* `DEVICE_NOTIFY_CALLBACK` / `DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS`
* `SPCRP_UPPERFILTERS` / `SPCRP_LOWERFILTERS`
* `PDSREG_JOIN_INFO`、`GetAddrInfoExCancel` 等

两个反复踩到的坑：

1. **不要在这里重复声明函数原型**。函数原型由 `portgen` 依据上游
   `__DEFINE_THUNK` 实参生成，签名与定义完全一致；手写一份很容易在返回类型
   （如 `NETIO_STATUS` vs `DWORD`）、调用约定、链接（C vs C++）上冲突。
   只有 MinGW 完全没声明的（`GetIfEntry2Ex`、`CreateFile3`）才补，
   且必须放在 `namespace YY::Thunks` 内并写 `EXTERN_C`。
2. **不要用 `WINBASEAPI` / `NETIOAPI_API`**：它们在 MinGW 下展开成
   `__declspec(dllimport)`，而这些 API 正是 YY-Thunks 要顶替的对象，
   必须让本编译单元里的同名定义成为真正的定义。

### 2.9 其它针对具体文件的转换

| 文件 | 改动 | 原因 |
| --- | --- | --- |
| `Shared/km.h` | `#if _MSC_VER <= 1500` → `#if defined(_MSC_VER) && _MSC_VER <= 1500` | GCC 下 `_MSC_VER` 未定义即 0，会误判为老 MSVC 而重复定义 `FIRMWARE_TYPE` |
| `Shared/km.h` | 去掉 `__if_not_exists(NtCurrentTeb)` 块 | MinGW 的 winnt.h 已提供 |
| `Thunks/shell32.hpp` | 去掉重复的 `#include <Knownfolders.h>` | MinGW 的 knownfolders.h 没有 include guard，ShlGuid.h 内部又会包含它 |
| `Thunks/YY_Thunks.cpp` | 不再 `#define INITKNOWNFOLDERS` | 同上；GUID 改由 `libuuid.a` 提供 |
| `Thunks/yy_Thunks.h/.cpp` | `decltype(函数名)` → 生成的函数类型 typedef | MinGW 的 `pathcch.h` 额外提供了同名 C++ 内联重载（`PathIsUNCEx(PWSTR,PWSTR*)`），与 `extern "C"` 版本共同构成重载集，使 `decltype`/取址二义 |
| `Thunks/ntdll.hpp` | `RtlAddGrowableFunctionTable` 返回类型 `NTSTATUS` → `DWORD` | MinGW 声明为 `DWORD`，签名不一致会被当成重载 |
| `Thunks/api-ms-win-core-memory.hpp` | `Offer/ReclaimVirtualMemory` 的地址参数 `void const*` → `PVOID` | 同上 |

---

## 3. 链接模型

上游 OBJ 模式的做法是"把 obj 直接塞进 Additional Dependencies"，
因为 MSVC 的导入库是按需拉取的归档，obj 里的定义会先于导入库生效。

MinGW 下的对应关系：

* 本移植产出**静态库** `libyythunks.a`，且只有**一个目标文件**（单翻译单元），
  因此等价于上游的 obj。
* `ld` 同样按需拉取归档成员：只要 `libyythunks.a` 排在系统导入库之前，
  YY 的定义就会先被取用，导入库里的同名桩不会被拉进来。
* YY-Thunks 还会定义 `__imp_<Api>`（IAT 符号），让 MinGW 头里
  `__declspec(dllimport)` 产生的直接引用也指向自己。

冒烟测试反过来验证了这一点：不链接 `libyythunks.a` 时，
`WaitOnAddress` 是**链接错误**（MinGW 的导入库里没有它），链接后可用。

### 3.2 "导入库里也有这个函数" 时谁赢？——实测

这是移植是否真正生效的关键问题。结论：**只要 `yythunks` 排在系统库前面，就一定覆盖**；
顺序错了则**报错而非静默降级**。

用 `GetSystemTimePreciseAsFileTime` 实测（它是 Windows 8 API，
**MinGW 的 `libkernel32.a` 里确实有**，因此是最有说服力的样本）：

| 链接顺序 | 结果 |
| --- | --- |
| `objs libyythunks.a -lkernel32 …` | 成功；最终 PE 的**导入表里没有** `GetSystemTimePreciseAsFileTime`，`__imp_GetSystemTimePreciseAsFileTime` 也是 PE 内部的 `R` 符号 |
| `objs -lkernel32 … libyythunks.a` | `multiple definition of 'GetSystemTimePreciseAsFileTime'; libkernel32.a(libkernel32s00792.o) first defined here` |

机制：`ld` 处理归档时**只按需提取成员**。扫描 `libyythunks.a` 时该符号还是未定义的，
于是单目标文件的成员被提取并定义它；等扫描到 `libkernel32.a` 时符号已定义，
而且 MinGW 导入库是**一个符号一个成员**（`libkernel32s00792.o` 只导出这一个符号），
没有理由被提取 —— 系统桩永远进不来。

因此"编译机的导入库里有没有这个函数"**不影响**覆盖结果。

两点补充：

* **不要用 `-Wl,--allow-multiple-definition`**。它取第一个定义、静默忽略后续，
  正好拆掉上面这个安全网：系统库在前时会**静默失去覆盖**。
* 另外两类符号与顺序无关：
  YY 只在兜底分支里**调用**的 SDK 函数（`BCryptCreateHash`、`MFPutWorkItem` …）
  正常从导入库取；YY 定义的 `__imp_<Api>` 与函数符号同生共死。

### 3.3 静态导入：产物绝不能依赖"可能不存在"的 DLL

这是移植过程中踩到的一个**真实故障**，值得单独记录。

**现象**：在 Windows Server 2012（Server Core 风格安装）上运行，报
"找不到 MFPlat.DLL" —— 进程在**启动阶段**就失败，连 `main` 都进不去，
而应用根本没有用到任何 Media Foundation API。

**根因**：上游用目标版本守卫来表达"这个 API 从 Vista 起就原生存在，
只有更低版本才需要兜底"：

```cpp
#if (YY_Thunks_Target < __WindowsNT6)      // ← 目标恰好是 Vista 时为假
    __DEFINE_THUNK(mfplat, 12, HRESULT, STDAPICALLTYPE, MFPutWorkItem, ...)
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)    // ← Vista 时需要，于是编译进来
    __DEFINE_THUNK(mfplat, 16, HRESULT, STDAPICALLTYPE, MFPutWorkItem2, ...)
    {
        if (auto _pfn = try_get_MFPutWorkItem2()) return _pfn(...);
        return MFPutWorkItem(...);          // ← 直接调用 SDK 版本！
    }
#endif
```

当 `YY_Thunks_Target` **恰好等于 `__WindowsNT6`** 时：
`MFPutWorkItem` 不参与编译（`6.0 < 6.0` 为假），但 `MFPutWorkItem2` 参与编译
（`6.0 < 6.2` 为真），而后者的兜底分支**直接调用** `MFPutWorkItem`
→ 解析到 **SDK 导入**而不是本库的 thunk → 产物静态导入 `MFPlat.DLL`。

同类问题共三处（都是 `#if (YY_Thunks_Target < __WindowsNT6)` 守卫）：

| 文件 | API | 是谁的兜底在调用它 | 被静态导入的 DLL |
| --- | --- | --- | --- |
| `mfplat.hpp` | `MFPutWorkItem` | `MFPutWorkItem2` | `MFPlat.DLL` |
| `dxgi.hpp` | `CreateDXGIFactory` | `CreateDXGIFactory1/2` | `dxgi.dll` |
| `Iphlpapi.hpp` | `GetIfEntry2` | `GetIfEntry2Ex` | `IPHLPAPI.DLL` |

**修法**：让这些 thunk **始终参与编译**（`kind="force_thunk"`）。这样兜底里的
`MFPutWorkItem(...)` 会解析到本库的 thunk，由它做 LoadLibrary + GetProcAddress
——静态导入消失，`MFPlat.DLL` 只在**真正调用**时才被加载。若该 DLL 不存在，
调用返回失败（`E_NOTIMPL` / 错误码）而不是让进程起不来。

前两处已在 `tools/transforms.py` 中启用；第三处（Iphlpapi）的 thunk 体依赖同文件
另一个被禁用块中的助手函数（`MIB_IFROW_To_MIB_IF_ROW2`），强行启用会编译失败。
权衡后保留该导入 —— `IPHLPAPI.DLL` 是系统基础组件，Server Core 也自带，
由检查工具归入"低风险"。

**为什么这比"减少模块"更好**：卸模块会连 thunk 一起失去；而
`force_thunk` 保留了功能，只是把"启动期强依赖"换成"首次调用时按需加载"。
这也正是本库存在的意义。

**防回归**：`tools/check_static_imports.py` 解析产物的导入表，
按"高风险/低风险"分级报告，已接入 CTest：

```powershell
python tools/check_static_imports.py build/x64/examples/smoke/yythunks_smoke.exe
ctest --test-dir build/x64 --output-on-failure
```

高风险 DLL（Media Foundation、DirectX 系、WinUSB、蓝牙、WLAN…）一旦出现即判定失败。
上游若新增同类写法，CI 会立刻拦住。

> 注意：检查结果与 `YY_Thunks_Target` 相关。换目标（Vista / Win10 …）后应重新跑一遍。

### 3.4 构建期防护

上面这些坑都属于"**不报错但结果是错的**"，所以各自配了显式防护：

| 风险 | 后果 | 防护 |
| --- | --- | --- |
| `YY_THUNKS_TARGET_MACRO` 写错 | `#if` 里未定义标识符按 `0` 处理 → 所有 thunk 都被编译（565 个而非 259 个），静默产出另一个配置 | `CMakeLists.txt` 从上游 `SharedDefs.h` 解析合法值并校验，非法值 `FATAL_ERROR` 并列出全部取值 |
| 多个构建目录共用 `src/port`（生成物在源码树里） | 互相覆盖；`cmake --build` **不会**重新 configure，于是静默编译到别人的生成物 | 生成物带**生成戳记**（记录目标宏/前缀/模块数）；构建前置目标 `yythunks_port_stamp` 每次校验，不一致立即失败并指出解决办法 |
| 手工解析出的守卫与真实编译条件不一致 | 启用集合判断错误：该编的没编、不该编的编了 | 生成时把守卫表在同一宏环境下**逐条求值**，与真实预处理探测比对；`tools/target_matrix.py` 在全部 22 个目标上重复该比对（开发期/CI） |
| 改了 `transforms.py` / `manifest` 后直接 build | 沿用旧生成物，改动无声失效 | 这些文件登记为 `CMAKE_CONFIGURE_DEPENDS`，改动后自动触发重新 configure + 重新生成 |
| 兜底调用退化成可选 DLL 的静态导入 | 缺少该 DLL 的系统**启动阶段**就失败（见 §3.3） | `tools/check_static_imports.py` 作为 CTest 用例，出现高风险 DLL 即判定失败 |
| 转换规则把 thunk 名字/行为改坏（或某宏把 `...W` 别名成 `...A`） | 编译、链接、冒烟测试**全部正常**，只是名字变了 —— 产物行为与上游不符 | `tools/audit_vs_upstream.py` 作为 CTest 用例，三层名字对照（上游原文 → 覆盖层 → 编译器展开结果），任一差集即失败 |
| 复用生成物时输入已变化（改了 `transforms.py` 却复用了旧生成物） | 改动**无声失效** —— 最难发现的一类 | 戳记含 `transforms.py` / `thunks.manifest` / `exclude.txt` / `portgen.py` / `overrides/**` 的 SHA-256 指纹，任一变化即判"已过期"并重新生成 |
| `portgen.py` 没跑起来被当成"上游漂移" | Python 找不到脚本/语法错误的退出码是 **2**，与"漂移"同码 → 静默放过。实测移开 `tools/` 后 configure 仍报 `Configuring done` | 漂移码改为 **3**，其余非 0 一律 `FATAL_ERROR` |
| 构建依赖 `tools/` / Python | 分发自足场景下使用者只拿到 `src/`，却被迫装 Python、带着工具脚本 | 戳记校验改为**纯 CMake**（`file(READ)`+`string(JSON)`+`file(SHA256)`）；`tools/portgen.py` 降为可选输入；依赖 Python 的步骤在无 Python 时自动跳过 |

前三项是"配置与产物不一致"，第四项是"产物与运行环境不兼容"，第五项是"产物与
上游行为不一致"，第六、七、八项是"生成物与生成输入 / 工具链不一致"。共同点都
值得记一笔：**这类错误不会自己冒出来，必须靠构建系统主动拦。**

第七项尤其值得单独说：它说明**用退出码传递语义时要避开工具自身的保留码**。
Python 用 2 表示"无法运行脚本"，我们再用 2 表示"漂移"，两个语义就撞车了 ——
而撞车的结果是往"静默放过"的方向偏，恰好是最坏的那个方向。

两个 CTest 用例还遵循同一条原则：**核对不完整时判定失败，而不是静默跳过**。
`upstream_audit` 在覆盖层未物化（规则全部失配）或找不到生成的声明头时都会失败 ——
否则第 1 层会退化成"上游 vs 上游"的自比，发出一张虚假的健康证明。

### 3.5 如何验证覆盖是否真的生效

对最终产物做两个检查：

```powershell
# 1) 被 thunk 的 API 不应出现在导入表里（输出为空 = 覆盖成功）
x86_64-w64-mingw32-objdump -p yourapp.exe |
    Select-String "SetThreadDescription|WaitOnAddress|GetSystemTimePreciseAsFileTime"

# 2) YY-Thunks 的内部符号应存在（证明库确实被拉进来了）
x86_64-w64-mingw32-nm yourapp.exe | Select-String "__YY_Thunks_Installed|try_get_"
```

运行期也可以直接判断 —— `__YY_Thunks_Installed` 是上游 `YY_Thunks.h` 导出的版本标记
（值就是 `YY_Thunks_Target`）：

```cpp
extern "C" const UINT64 __YY_Thunks_Installed;
if (__YY_Thunks_Installed) { /* 覆盖已生效 */ }
```

顺带一个有利的副作用：如果应用**完全没有引用**任何被 thunk 的 API，
`libyythunks.a` 的成员不会被提取，等于零成本、零体积增长。

### 3.1 让产物"非 g++ 驱动也能直接链接"

本库经常被 Rust/Cargo、自研构建系统等**不经 g++ 驱动**的工程直接链接，
这类工程不会自动带上 GCC 的 C++ 运行时。因此移植层额外做了三件事：

1. **`-fno-exceptions -fno-unwind-tables`**
   上游全仓库没有 `try` / `catch` / `throw`，本身不会抛异常。
   关掉异常后 GCC 不再生成异常清理表，产物就不会引用
   `__gxx_personality_seh0` / `_Unwind_Resume`，也就不需要 `-lstdc++`。
   （不开这个选项时，链接方会在 `.xdata` 上撞到 `__gxx_personality_seh0` 未定义。）

2. **`GUID_NULL` 弱定义**
   MinGW 的 `guiddef.h` 只给声明，定义在 `libuuid.a`；而上游 `shell32.hpp`
   按 const 引用使用它（ODR-use），于是链接方被迫额外链接 `uuid`。
   移植层在 `yy_thunks_postlude.h` 里给出**弱定义**，使用者或 `libuuid.a`
   若提供强定义仍以强定义为准。

3. **链接需求清单**
   上游的 `#pragma comment(lib, ...)` 在 GCC 下被忽略。系统导入库清单集中在
   `CMakeLists.txt` 的 `YY_THUNKS_SYSTEM_LIBS`，并在配置阶段生成
   `build/<preset>/yythunks-link-requirements.txt` 供非 CMake 工程读取。

剩余必需的额外链接项只有 `libgcc`（`__chkstk_ms`、`__popcountdi2` 等），
`g++` 驱动会自动带上。

---

## 4. 已知限制（汇总）

1. **SEH 降级**（§2.5）：异常路径不再被捕获。
2. **11 个模块未启用**：全部是 MinGW-w64 SDK 头缺失/缺陷，逐条见 `portrule/exclude.txt`。
3. **`DllMainCRTStartup.hpp` 未启用**：依赖 MSVC `/alternatename` 弱别名
   （`YY_ThunksOriginalDllMainCRTStartup` → `_DllMainCRTStartup`），
   且逻辑仅在目标 < Vista 时生效。本移植的最低受支持目标就是 Vista，
   因此不存在功能损失（XP 已明确不支持）。
4. **32 位（i686）不支持**：x86 分支依赖 MSVC 的 `__identifier` 定义带 `@N` 后缀的
   IAT 符号（`_imp__Api@N`），而 GCC 的 `##` 拼不出含 `@` 的记号（实测 1000+ 错误）。
   需要改用汇编 `.set` 或 `-Wl,--defsym` 生成等价符号后才能支持；configure 阶段已
   明确拒绝该配置。
5. **`#pragma comment(lib, ...)` 被忽略**：所需的系统库已在 `CMakeLists.txt` 里
   显式列出。上游若新增依赖库，需要同步补上。
