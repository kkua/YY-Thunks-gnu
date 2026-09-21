# -*- coding: utf-8 -*-
"""YY-Thunks 上游源码 -> MinGW 可编译源码 的确定性增量规则。

设计原则
--------
1. **不复制整份源码**：只有需要改动的文件才会被物化到 `src/port/` 覆盖目录，
   其余文件仍然直接从 `upstream/` 参与编译（通过 include 搜索顺序实现）。
2. **规则必须可校验**：literal 规则如果在目标文件里找不到，`portgen.py` 会报错，
   以此作为"上游已漂移"的告警，避免静默失效。
3. **能用垫片宏解决的绝不改源码**：见 `src/compat/yy_thunks_prelude.h`。

`kind` 取值
-----------
  "literal"       pattern/repl 为原样字符串（推荐：可读、可审查、漂移可检测）
  "regex"         pattern/repl 为正则（仅在需要批量替换时使用）
  "seh"           把 MSVC 的 __try/__except 降级为普通语句（见 docs/PORTING.md）
  "force_thunk"   pattern 为 API 名；把其 __DEFINE_THUNK 外层守卫改为恒真，
                  以避免"兜底直接调用 SDK"造成的可选 DLL 静态导入

`__if_exists` 的替代
--------------------
上游用 MSVC 专有的 `__if_exists(YY::Thunks::try_get_X)` 判断某个 thunk 是否
参与编译。`portgen.py` 从上游源码提取每个 thunk 的守卫表达式，产出
`YY_Thunks_EnableTable.hpp`：

    #define YY_THUNK_GUARD_try_get_X (YY_Thunks_Target < __WindowsNT6_2)

因为 __Version 是预处理器算术表达式，所以这里统一改写为
`#if YY_THUNK_GUARD_try_get_X` —— 该表达式在**编译期**按 `-DYY_Thunks_Target`
求值决定启用与否，于是这份生成物**与目标无关**，所有目标共用一份。

正确性由两道校验保证：
  - 生成时：guard 表在同一宏环境下逐条求值，必须与真实预处理探测的启用集合一致；
  - 开发时：tools/target_matrix.py 在全部合法目标上做同样的比对。
"""

TRANSFORMS = [
    # ------------------------------------------------------------------ #
    # 0a. SEH 降级（__try/__except -> 普通语句）
    #
    #     不使用宏替换的原因是 libstdc++ 的 bits/exception_defines.h 已经把
    #     __try 定义成 `try` 的别名，宏覆盖会破坏 STL 的异常处理。
    #     这里只在这些确实用到 SEH 的文件上做文本改写。
    #     正常路径语义不变；异常路径不再被捕获（见 docs/PORTING.md 已知限制）。
    # ------------------------------------------------------------------ #
    dict(
        files=[
            "Thunks/YY_Thunks.h",
            "Thunks/YY_Thunks.cpp",
            "Thunks/DllMainCRTStartup.hpp",
            "Thunks/api-ms-win-core-fibers.hpp",
            "Thunks/api-ms-win-core-libraryloader.hpp",
            "Thunks/Iphlpapi.hpp",
            "Thunks/ntdll.hpp",
        ],
        kind="seh",
        pattern="",
        repl="",
        required=False,
        note="SEH 降级",
    ),

    # ------------------------------------------------------------------ #
    # 0c. 强制启用若干 thunk，消除"可选 DLL 的静态导入"
    #
    #     上游用 `#if (YY_Thunks_Target < __WindowsNT6)` 表示"Vista 起原生就有，
    #     只有更低版本才需要兜底"。但当 target 恰好是 Vista 时，同文件里守卫为
    #     `< __WindowsNT6_2` 的兄弟 thunk 仍会编译，其兜底分支会**直接调用**
    #     这些原生 API → 产物静态导入了 mfplat.dll / dxgi.dll / iphlpapi.dll。
    #
    #     后果很严重：缺少该 DLL 的系统（如不含 Media Foundation 的 Server Core）
    #     会在**进程启动阶段**失败，哪怕应用根本没调用相关 API。
    #
    #     强制这些 thunk 参与编译后，调用会落到本库的 thunk（延迟加载），
    #     静态导入消失，DLL 只在真正需要时才被 LoadLibrary。
    #
    #     回归守护：tools/check_static_imports.py（已接入 CTest）
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/mfplat.hpp"],
        kind="force_thunk",
        pattern="MFPutWorkItem",
        repl="",
        note="MFPutWorkItem2 的兜底会直接调用它，导致静态导入 MFPlat.DLL",
    ),
    dict(
        files=["Thunks/dxgi.hpp"],
        kind="force_thunk",
        pattern="CreateDXGIFactory",
        repl="",
        note="CreateDXGIFactory1/2 的兜底会直接调用它，导致静态导入 dxgi.dll",
    ),
    # 注：Iphlpapi.hpp 的 GetIfEntry2 同理（GetIfEntry2Ex 的兜底会直接调用它），
    # 但它的 thunk 体依赖同文件另一个被禁用块里的助手函数
    # （MIB_IFROW_To_MIB_IF_ROW2），强行启用会编译失败。
    # 权衡：IPHLPAPI.DLL 是系统基础组件，Server Core 也带；
    # 因此保留该静态导入，由 check_static_imports.py 归入"低风险"级别。

    # ------------------------------------------------------------------ #
    # 0b. Shared/km.h：MSVC 版本号门控在 GCC 下恒为真，需要补齐 _MSC_VER 判断
    # ------------------------------------------------------------------ #
    dict(
        files=["Shared/km.h"],
        kind="literal",
        pattern="#if _MSC_VER <= 1500",
        repl="#if defined(_MSC_VER) && _MSC_VER <= 1500",
        note="GCC 下 _MSC_VER 未定义即视为 0，会误判为老 MSVC 而重复定义 FIRMWARE_TYPE",
    ),

    # ------------------------------------------------------------------ #
    # 0b. GCC 不接受 `extern "C" extern <var>;`
    #
    #     实测报 `invalid use of 'extern' in linkage specification`。
    #
    #     **不能只把 `extern` 删掉** —— 在 C++ 里那会让变量**声明变成定义**：
    #       - 与后置垫片（提供弱定义）的关系变成依赖包含顺序；
    #       - 使用者再想强定义覆盖这些钩子，就会撞上重复定义。
    #     GCC 接受的是花括号形式，语义与上游逐字一致：
    #       extern "C" { extern BOOL __YY_Thunks_Disable_Rreload_Dlls; }
    #
    #     逐处列出而不是用宽泛的 regex：宽泛规则会把任何 `EXTERN_C extern`
    #     都变成定义，属于"静默改变语义"。将来上游新增同类写法时，
    #     编译会直接报错（比静默改语义好），照着这里再加一条即可。
    #
    #     注：Thunks/api-ms-win-core-fibers.hpp:133 也有 `extern "C" extern
    #     bool _tls_used;`，但它位于 `YY_Thunks_Target < __WindowsNT6` 分支内，
    #     在当前的受支持范围（Vista+）下不会被编译。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="EXTERN_C extern BOOL __YY_Thunks_Disable_Rreload_Dlls /* = FALSE*/;",
        repl='extern "C" { extern BOOL __YY_Thunks_Disable_Rreload_Dlls /* = FALSE*/; }',
        note="保留 extern，改成花括号形式（避免声明变定义）",
    ),
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="EXTERN_C extern HMODULE (__fastcall * const __pfnYY_Thunks_CustomLoadLibrary)(const wchar_t* _szModuleName, DWORD _fFlags);",
        repl='extern "C" { extern HMODULE (__fastcall * const __pfnYY_Thunks_CustomLoadLibrary)(const wchar_t* _szModuleName, DWORD _fFlags); }',
        note="同上",
    ),
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="EXTERN_C extern void* __acrt_atexit_table;",
        repl='extern "C" { extern void* __acrt_atexit_table; }',
        note="同上",
    ),
    dict(
        files=["Thunks/DllMainCRTStartup.hpp"],
        kind="literal",
        pattern='extern "C" extern decltype(_DllMainCRTStartup)* const __pfnDllMainCRTStartupForYY_Thunks;',
        repl='extern "C" { extern decltype(_DllMainCRTStartup)* const __pfnDllMainCRTStartupForYY_Thunks; }',
        required=False,
        note="同上（该文件当前未启用）",
    ),

    # ------------------------------------------------------------------ #
    # 1. __declspec(allocate(...)) -> __attribute__((section(...)))
    # ------------------------------------------------------------------ #
    dict(
        files=[
            "Thunks/YY_Thunks.cpp",
            "Thunks/YY_Thunks.h",
            "Thunks/DllMainCRTStartup.hpp",
            "Thunks/api-ms-win-core-fibers.hpp",
            "Thunks/api-ms-win-core-processthreads.hpp",
            "Thunks/api-ms-win-core-sysinfo.hpp",
            "Thunks/api-ms-win-core-winrt.hpp",
            "Shared/HookThunk.h",
        ],
        kind="regex",
        pattern=r'__declspec\(\s*allocate\(\s*"([^"]+)"\s*\)\s*\)',
        repl=r'YY_THUNKS_ALLOCATE("\1")',
        required=False,
        note="MSVC 的 __declspec(allocate) 在 GCC 下被忽略（仅告警），映射为 __attribute__((section))",
    ),

    # ------------------------------------------------------------------ #
    # 1b. 不再用 SDK 头定义 Known Folder GUID
    #
    #     上游在目标 < Vista 时定义 INITKNOWNFOLDERS，让 knownfolders.h 直接
    #     **定义** FOLDERID_* 变量（老 SDK 的 uuid.lib 里没有这些符号）。
    #     MinGW-w64 的 knownfolders.h 没有 include guard，而 shell32.hpp 同时
    #     包含 <Knownfolders.h> 与 <ShlGuid.h>（后者又包含前者），
    #     在“定义”模式下第二次包含就会重复定义 GUID。
    #     MinGW-w64 的 libuuid.a 已提供这些 GUID，因此改为不定义 + 链接 uuid。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern=(
            "#if (YY_Thunks_Target < __WindowsNT6)\n"
            "#define INITKNOWNFOLDERS\n"
            "#endif"
        ),
        repl=(
            "// 移植：GUID 由 MinGW-w64 的 libuuid.a 提供，不再让 knownfolders.h 定义\n"
            "// （其无 include guard，重复定义会与 <ShlGuid.h> 冲突，见 port/transforms.py）"
        ),
        note="改用 libuuid.a 提供 FOLDERID_*，避免 knownfolders.h 重复定义",
    ),

    # ------------------------------------------------------------------ #
    # 1c. shell32.hpp：去掉重复包含的 <Knownfolders.h>
    #     文件里先 #include <Knownfolders.h>，随后 #include <ShlGuid.h>，
    #     而 ShlGuid.h 内部又会包含 knownfolders.h。MSVC 的 knownfolders.h
    #     有 include guard，MinGW-w64 的没有，于是 GUID 被定义两次。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/shell32.hpp"],
        kind="literal",
        pattern="#include <Knownfolders.h>\n",
        repl="// 移植：已知目录 GUID 由下面的 <ShlGuid.h> 间接引入（knownfolders.h 无 include guard）\n",
        note="避免 knownfolders.h 被包含两次",
    ),

    # ------------------------------------------------------------------ #
    # 2. YY_Thunks.cpp：垫片注入
    # ------------------------------------------------------------------ #
    # 注：上游第一遍的 `#include "Thunks\YY_Thunks_List.hpp"` 已被上面第 4 条
    #     整段替换掉；第二遍的 `#include "YY_Thunks_List.hpp"` 路径本身可用，
    #     会经 include 搜索路径解析到 src/port/YY_Thunks_List.hpp。
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern='#include "YY_Thunks.h"',
        repl='#include "YY_Thunks.h"\n#include <yy_thunks_postlude.h>',
        note="注入移植层后置垫片（弱符号定义，替代 LibMaker FixObj）",
    ),

    # ------------------------------------------------------------------ #
    # 3. YY_Thunks.h：引入生成的特性/启用集合头
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern='#include "Shared/List.h"',
        repl='#include "Shared/List.h"\n#include "YY_Thunks_EnableTable.hpp"\n#include "YY_Thunks_Features.hpp"',
        note="引入生成的守卫表（目标无关）与 Fallback 解析器映射表",
    ),

    # ------------------------------------------------------------------ #
    # 4. 用"生成的声明头"取代上游的第一遍（声明）包含
    # ------------------------------------------------------------------ #
    # 上游是一套"两遍包含"：第一遍只声明，第二遍才定义；两遍之间的"只声明"
    # 依赖 MSVC 专有的 __if_not_exists 来跳过函数体。GCC 无该扩展，
    # 且若直接编译其函数体会因为 internal:: 助手尚未声明而报错。
    #
    # 移植层改为：由 portgen 的预处理探测一次性生成 YY_Thunks_Declarations.hpp
    # （内容与上游第一遍完全等价），这里整段替换掉第一遍的宏与包含。
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="regex",
        pattern=r"//展开函数的所有的 声明 以及 try_get_ 函数[\s\S]*?#undef __DEFINE_THUNK",
        repl=(
            "// 移植：上游的第 1 遍（声明）改由预处理探测一次性生成，见下方头文件。\n"
            "// 这样就不再需要 MSVC 专有的 __if_exists / __if_not_exists。\n"
            "#include \"YY_Thunks_Declarations.hpp\""
        ),
        note="以生成的声明头取代上游第一遍包含",
    ),

    # ------------------------------------------------------------------ #
    # 5. Fallback 解析器映射（替代 __if_exists(YY::Thunks::Fallback::try_get_X)）
    # ------------------------------------------------------------------ #
    # 上游整段是 4 行宏续行：
    #   __if_exists(YY::Thunks::Fallback::try_get_X) \
    #   {                                            \
    #       &YY::Thunks::Fallback::try_get_X         \
    #   }                                            \
    # 移植层改为直接展开成映射宏（值为解析器地址或 nullptr）。
    dict(
        files=["Thunks/YY_Thunks.cpp", "Thunks/YY_Thunks.h"],
        kind="regex",
        pattern=(
            r"__if_exists\(YY::Thunks::Fallback::_CRT_CONCATENATE\(try_get_, _FUNCTION\)\)\s*\\\n"
            r"\s*\{\s*\\\n"
            r"\s*&YY::Thunks::Fallback::_CRT_CONCATENATE\(try_get_, _FUNCTION\)\s*\\\n"
            r"\s*\}\s*\\"
        ),
        repl=r"_CRT_CONCATENATE(YY_THUNK_RESOLVER_, _FUNCTION)                                                       \\",
        note="改用生成器产出的解析器映射宏（整段替换，含花括号）",
    ),

    # ------------------------------------------------------------------ #
    # 5b. MSVC 的 64 位整型字面量后缀 i64 -> LL
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.h", "Thunks/YY_Thunks.cpp"],
        kind="regex",
        pattern=r"(?<=[0-9A-Fa-f])i64\b",
        repl="LL",
        required=False,
        note="GCC 不认 MSVC 的 i64 后缀",
    ),

    # ------------------------------------------------------------------ #
    # 5c. Shared/km.h 中 __if_not_exists(NtCurrentTeb)
    #     MinGW 的 winnt.h 已提供 NtCurrentTeb（宏或函数），无需重复声明。
    # ------------------------------------------------------------------ #
    dict(
        files=["Shared/km.h"],
        kind="regex",
        pattern=(
            r"__if_not_exists\(NtCurrentTeb\)\s*\n"
            r"\s*\{\s*\n"
            r"\s*PTEB NTAPI NtCurrentTeb\(void\);\s*\n"
            r"\s*\}"
        ),
        repl="// 移植：MinGW 的 winnt.h 已提供 NtCurrentTeb，无需重复声明",
        note="GCC 无 __if_not_exists",
    ),

    # ------------------------------------------------------------------ #
    # 5d. 用生成的函数类型 typedef 取代 decltype(函数名)
    #     起因：MinGW 的 pathcch.h 会为 PathIsUNCEx/PathCchSkipRoot/
    #     PathCchFindExtension 额外声明 C++ 内联重载（PWSTR 版本），
    #     与 extern "C" 版本构成重载集，导致 decltype / 取址二义。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern=(
            "    static decltype(_CRT_CONCATENATE_(_PREFIX, _FUNCTION))* __cdecl "
            "_CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept"
        ),
        repl=(
            "    static _CRT_CONCATENATE(YY_THUNK_FNTYPE_, _FUNCTION) __cdecl "
            "_CRT_CONCATENATE(try_get_, _FUNCTION)() noexcept"
        ),
        note="try_get_* 返回类型改用 typedef",
    ),
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern="return reinterpret_cast<decltype(_CRT_CONCATENATE_(_PREFIX, _FUNCTION))*>(try_get_function(",
        repl="return reinterpret_cast<_CRT_CONCATENATE(YY_THUNK_FNTYPE_, _FUNCTION)>(try_get_function(",
        note="返回转换改用 typedef",
    ),
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern=(
            "#define _DEFINE_IAT_SYMBOL_PREFIX(_PREFIX, _FUNCTION, _SIZE) "
            "_LCRT_DEFINE_IAT_SYMBOL(_PREFIX ## _FUNCTION, _SIZE)"
        ),
        repl=(
            "#define _DEFINE_IAT_SYMBOL_PREFIX(_PREFIX, _FUNCTION, _SIZE) "
            "_LCRT_DEFINE_IAT_SYMBOL(_PREFIX ## _FUNCTION, _SIZE, "
            "_CRT_CONCATENATE(YY_THUNK_FNTYPE_, _FUNCTION))"
        ),
        note="IAT 符号定义需要显式类型以消除重载二义",
    ),
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="#define _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION, _SIZE)",
        repl="#define _LCRT_DEFINE_IAT_SYMBOL(_FUNCTION, _SIZE, _FN_TYPE)",
        note="为 IAT 符号宏增加函数类型参数（x86/x64 两个分支都会命中）",
    ),
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="= reinterpret_cast<void const*>(_FUNCTION)",
        repl="= reinterpret_cast<void const*>(static_cast<_FN_TYPE>(_FUNCTION))",
        note="用 static_cast 从重载集中挑选目标函数",
    ),

    # ------------------------------------------------------------------ #
    # 5d-2. __if_exists(try_get_X) 的"遮蔽式替换"（appmodel / user32）
    #
    #   语义：若 X 也被 Thunk，则用解析到的原生指针并屏蔽 SDK 导入；
    #         否则直接使用 SDK 导入符号。
    #   判定依据来自 portgen 的预处理探测（YY_THUNK_HAS_try_get_X）。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "        __if_exists(try_get_OpenPackageInfoByFullName)\n"
            "        {\n"
            "            const auto OpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName();\n"
            "            if (!OpenPackageInfoByFullName)\n"
            "            {\n"
            "                return ERROR_NOT_FOUND;\n"
            "            }\n"
            "        }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_OpenPackageInfoByFullName\n"
            "        const auto OpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName();\n"
            "        if (!OpenPackageInfoByFullName)\n"
            "        {\n"
            "            return ERROR_NOT_FOUND;\n"
            "        }\n"
            "#endif"
        ),
        note="OpenPackageInfoByFullName 遮蔽替换",
    ),
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "        __if_exists(try_get_ClosePackageInfo)\n"
            "        {\n"
            "            const auto ClosePackageInfo = try_get_ClosePackageInfo();\n"
            "            if (!ClosePackageInfo)\n"
            "            {\n"
            "                return ERROR_NOT_FOUND;\n"
            "            }\n"
            "        }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_ClosePackageInfo\n"
            "        const auto ClosePackageInfo = try_get_ClosePackageInfo();\n"
            "        if (!ClosePackageInfo)\n"
            "        {\n"
            "            return ERROR_NOT_FOUND;\n"
            "        }\n"
            "#endif"
        ),
        note="ClosePackageInfo 遮蔽替换",
    ),
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "        __if_exists(try_get_GetPackageInfo)\n"
            "        {\n"
            "            const auto GetPackageInfo = try_get_GetPackageInfo();\n"
            "            if (!GetPackageInfo)\n"
            "            {\n"
            "                return ERROR_NOT_FOUND;\n"
            "            }\n"
            "        }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_GetPackageInfo\n"
            "        const auto GetPackageInfo = try_get_GetPackageInfo();\n"
            "        if (!GetPackageInfo)\n"
            "        {\n"
            "            return ERROR_NOT_FOUND;\n"
            "        }\n"
            "#endif"
        ),
        note="GetPackageInfo 遮蔽替换",
    ),
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "            __if_exists(try_get_GetPackagesByPackageFamily)\n"
            "            {\n"
            "                const auto GetPackagesByPackageFamily = try_get_GetPackagesByPackageFamily();\n"
            "                if (!GetPackagesByPackageFamily)\n"
            "                {\n"
            "                    break;\n"
            "                }\n"
            "            }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_GetPackagesByPackageFamily\n"
            "            const auto GetPackagesByPackageFamily = try_get_GetPackagesByPackageFamily();\n"
            "            if (!GetPackagesByPackageFamily)\n"
            "            {\n"
            "                break;\n"
            "            }\n"
            "#endif"
        ),
        note="GetPackagesByPackageFamily 遮蔽替换",
    ),
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "            __if_exists(try_get_OpenPackageInfoByFullName)\n"
            "            {\n"
            "                const auto OpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName();\n"
            "                if (!OpenPackageInfoByFullName)\n"
            "                {\n"
            "                    break;\n"
            "                }\n"
            "            }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_OpenPackageInfoByFullName\n"
            "            const auto OpenPackageInfoByFullName = try_get_OpenPackageInfoByFullName();\n"
            "            if (!OpenPackageInfoByFullName)\n"
            "            {\n"
            "                break;\n"
            "            }\n"
            "#endif"
        ),
        note="OpenPackageInfoByFullName 遮蔽替换（FindPackagesByPackageFamily）",
    ),
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "            __if_exists(try_get_ClosePackageInfo)\n"
            "            {\n"
            "                const auto ClosePackageInfo = try_get_ClosePackageInfo();\n"
            "                if (!ClosePackageInfo)\n"
            "                {\n"
            "                    break;\n"
            "                }\n"
            "            }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_ClosePackageInfo\n"
            "            const auto ClosePackageInfo = try_get_ClosePackageInfo();\n"
            "            if (!ClosePackageInfo)\n"
            "            {\n"
            "                break;\n"
            "            }\n"
            "#endif"
        ),
        note="ClosePackageInfo 遮蔽替换（FindPackagesByPackageFamily）",
    ),
    dict(
        files=["Thunks/api-ms-win-appmodel-runtime.hpp"],
        kind="literal",
        pattern=(
            "            __if_exists(try_get_GetPackageInfo)\n"
            "            {\n"
            "                const auto GetPackageInfo = try_get_GetPackageInfo();\n"
            "                if (!GetPackageInfo)\n"
            "                {\n"
            "                    break;\n"
            "                }\n"
            "            }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_GetPackageInfo\n"
            "            const auto GetPackageInfo = try_get_GetPackageInfo();\n"
            "            if (!GetPackageInfo)\n"
            "            {\n"
            "                break;\n"
            "            }\n"
            "#endif"
        ),
        note="GetPackageInfo 遮蔽替换（FindPackagesByPackageFamily）",
    ),
    dict(
        files=["Thunks/user32.hpp"],
        kind="literal",
        pattern=(
            "                __if_exists(try_get_LoadLibraryA)\n"
            "                {\n"
            "                    ShellCodeParameter.pfnLoadLibraryA = try_get_LoadLibraryA();\n"
            "                }\n"
            "                __if_not_exists(try_get_LoadLibraryA)\n"
            "                {\n"
            "                    ShellCodeParameter.pfnLoadLibraryA = (decltype(LoadLibraryA)*)GetProcAddress(try_get_module_kernel32(), \"LoadLibraryA\");\n"
            "                }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_LoadLibraryA\n"
            "                ShellCodeParameter.pfnLoadLibraryA = try_get_LoadLibraryA();\n"
            "#else\n"
            "                ShellCodeParameter.pfnLoadLibraryA = (decltype(LoadLibraryA)*)GetProcAddress(try_get_module_kernel32(), \"LoadLibraryA\");\n"
            "#endif"
        ),
        note="LoadLibraryA 遮蔽/回退替换",
    ),
    dict(
        files=["Thunks/user32.hpp"],
        kind="literal",
        pattern=(
            "                __if_exists(try_get_SetProcessDPIAware)\n"
            "                {\n"
            "                    ShellCodeParameter.pfnSetProcessDPIAware = try_get_SetProcessDPIAware();\n"
            "                }\n"
            "                __if_not_exists(try_get_SetProcessDPIAware)\n"
            "                {\n"
            "                    ShellCodeParameter.pfnSetProcessDPIAware = (decltype(SetProcessDPIAware)*)GetProcAddress(_hUser32Module, \"SetProcessDPIAware\");\n"
            "                }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_SetProcessDPIAware\n"
            "                ShellCodeParameter.pfnSetProcessDPIAware = try_get_SetProcessDPIAware();\n"
            "#else\n"
            "                ShellCodeParameter.pfnSetProcessDPIAware = (decltype(SetProcessDPIAware)*)GetProcAddress(_hUser32Module, \"SetProcessDPIAware\");\n"
            "#endif"
        ),
        note="SetProcessDPIAware 遮蔽/回退替换",
    ),
    dict(
        files=["Thunks/user32.hpp"],
        kind="literal",
        pattern=(
            "                __if_exists(try_get_GetDpiForSystem)\n"
            "                {\n"
            "                    ShellCodeParameter.pfnGetDpiForSystem = try_get_GetDpiForSystem();\n"
            "                }\n"
            "                __if_not_exists(try_get_GetDpiForSystem)\n"
            "                {\n"
            "                    ShellCodeParameter.pfnGetDpiForSystem = (decltype(GetDpiForSystem)*)GetProcAddress(_hUser32Module, \"GetDpiForSystem\");\n"
            "                }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_GetDpiForSystem\n"
            "                ShellCodeParameter.pfnGetDpiForSystem = try_get_GetDpiForSystem();\n"
            "#else\n"
            "                ShellCodeParameter.pfnGetDpiForSystem = (decltype(GetDpiForSystem)*)GetProcAddress(_hUser32Module, \"GetDpiForSystem\");\n"
            "#endif"
        ),
        note="GetDpiForSystem 遮蔽/回退替换",
    ),
    dict(
        files=["Thunks/user32.hpp"],
        kind="literal",
        pattern=(
            "            __if_exists(try_get_GetProcessDpiAwareness)\n"
            "            {\n"
            "                const auto GetProcessDpiAwareness = try_get_GetProcessDpiAwareness();\n"
            "            }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_GetProcessDpiAwareness\n"
            "            const auto GetProcessDpiAwareness = try_get_GetProcessDpiAwareness();\n"
            "#endif"
        ),
        note="GetProcessDpiAwareness 遮蔽替换",
    ),

    # ------------------------------------------------------------------ #
    # 5d-3. 让少数 thunk 的签名与 MinGW-w64 SDK 头保持一致
    #
    #   extern "C" 的函数名不参与命名空间区分，若签名不同就会被 GCC 视为
    #   重载集，导致 decltype/取址二义。MSVC 的 SDK 与 MinGW-w64 在个别
    #   函数上的签名有差异，这里按 MinGW-w64 的声明对齐（语义等价）。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/ntdll.hpp"],
        kind="literal",
        pattern=(
            "    __DEFINE_THUNK(\n"
            "    ntdll,\n"
            "    24,\n"
            "    NTSTATUS,\n"
            "    NTAPI,\n"
            "    RtlAddGrowableFunctionTable,"
        ),
        repl=(
            "    __DEFINE_THUNK(\n"
            "    ntdll,\n"
            "    24,\n"
            "    DWORD,\n"
            "    NTAPI,\n"
            "    RtlAddGrowableFunctionTable,"
        ),
        note="MinGW-w64 的 winnt.h 把 RtlAddGrowableFunctionTable 声明为 DWORD 返回值",
    ),
    dict(
        files=["Thunks/api-ms-win-core-memory.hpp"],
        kind="literal",
        pattern="        _In_reads_(_uSize) void const* _pVirtualAddress,",
        repl="        _In_reads_(_uSize) PVOID _pVirtualAddress,",
        note="MinGW-w64 的 memoryapi.h 把 Offer/ReclaimVirtualMemory 的地址参数声明为 PVOID",
    ),

    # ------------------------------------------------------------------ #
    # 5d-4. 撤销 MinGW esent.h 的一处 A/W 别名缺陷
    #
    #   MinGW-w64 的 <esent.h> 里有：
    #       #define JetGetTableColumnInfoW __MINGW_NAME_AW(JetGetTableColumnInfo)
    #   而 __MINGW_NAME_AW(X) 在**未定义 UNICODE** 时展开为 X##A。
    #   于是上游源码里的 `JetGetTableColumnInfoW`（第 5 个宏实参，即 thunk 名）
    #   被宏替换成 JetGetTableColumnInfoA：
    #       - W 版 thunk 实际定义的却成了 A 入口；
    #       - 真正的 W API 完全没被 thunk。
    #   这是语义错误，不只是命名问题。上游用 MSVC 时 esent.h 没有该别名，
    #   所以只有 MinGW 侧需要撤销。
    #
    #   发现途径：tools/target_matrix.py 的"守卫表自校验"——
    #   守卫表按源码字面量记录为 ...W，真实探测得到的却是 ...A，两者对不上。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/esent.hpp"],
        kind="literal",
        pattern="#include <Esent.h>",
        repl=(
            "#include <Esent.h>\n"
            "// 移植：撤销 MinGW esent.h 的 JetGetTableColumnInfoW -> ...A 别名。\n"
            "// __MINGW_NAME_AW(X) 在未定义 UNICODE 时展开为 X##A，会把上游写的 W 版\n"
            "// thunk 改名成 A 入口（真正的 W API 就没被 thunk 了）。详见 port/transforms.py\n"
            "#undef JetGetTableColumnInfoW"
        ),
        note="撤销 MinGW esent.h 对 JetGetTableColumnInfoW 的 A/W 别名（否则 W 版 thunk 会被改名成 A）",
    ),

    # ------------------------------------------------------------------ #
    # 5e. .YYThr$AAA 节内 const / 非 const 混放导致节属性冲突
    #     该节的边界哨兵是非 const（可写）的，GCC 不允许把 const 变量
    #     放进可写节；把 pInit_* 的 const 去掉即可（它只被读取）。
    #     注：YY_Thunks.cpp 里本来就已是非 const，只有 YY_Thunks.h 需要改。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="static void* const _CRT_CONCATENATE(pInit_ ,_FUNCTION)",
        repl="static void* _CRT_CONCATENATE(pInit_ ,_FUNCTION)",
        note="避免 .YYThr$AAA 节类型冲突（const 的只读放置 vs 非 const 哨兵）",
    ),

    # ------------------------------------------------------------------ #
    # 5f. YY_Thunks.h 需要在使用 __security_cookie 之前看到后置垫片
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern="#include <Windows.h>",
        repl="#include <Windows.h>\n#include <yy_thunks_postlude.h>",
        note="__security_cookie 等弱符号必须在 YY_Thunks.h 使用点之前可见",
    ),

    # ------------------------------------------------------------------ #
    # 6. YY_Thunks.cpp 内部的三处"阴影替换"
    #    上游语义：若该 API 也被 Thunk，则用解析到的原生指针，避免自递归/死锁；
    #    未被 Thunk 时直接使用 SDK 导入即可。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern=(
            "        __if_exists(YY::Thunks::try_get_LoadLibraryExW)\n"
            "        {\n"
            "            const auto LoadLibraryExW = YY::Thunks::try_get_LoadLibraryExW();\n"
            "            if (!LoadLibraryExW)\n"
            "                return nullptr;\n"
            "        }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_LoadLibraryExW\n"
            "        const auto LoadLibraryExW = YY::Thunks::try_get_LoadLibraryExW();\n"
            "        if (!LoadLibraryExW)\n"
            "            return nullptr;\n"
            "#endif"
        ),
        note="LoadLibraryExW 阴影替换",
    ),
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern=(
            "    __if_exists(YY::Thunks::try_get_GetProcAddress)\n"
            "    {\n"
            "        const auto GetProcAddress = YY::Thunks::try_get_GetProcAddress();\n"
            "    }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_GetProcAddress\n"
            "    const auto GetProcAddress = YY::Thunks::try_get_GetProcAddress();\n"
            "#endif"
        ),
        note="GetProcAddress 阴影替换",
    ),
    dict(
        files=["Thunks/YY_Thunks.cpp"],
        kind="literal",
        pattern=(
            "            __if_exists(YY::Thunks::try_get_CloseHandle)\n"
            "            {\n"
            "                const auto CloseHandle = YY::Thunks::try_get_CloseHandle();\n"
            "            }"
        ),
        repl=(
            "#if YY_THUNK_GUARD_try_get_CloseHandle\n"
            "            const auto CloseHandle = YY::Thunks::try_get_CloseHandle();\n"
            "#endif"
        ),
        note="CloseHandle 阴影替换（避免 GetYY_ThunksSharedData 自递归）",
    ),

    # ------------------------------------------------------------------ #
    # 7. __if_exists(__YY_Thunks_Process_Terminating)：该变量恒有定义，恒为真
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/YY_Thunks.h"],
        kind="literal",
        pattern=(
            "    __if_exists(__YY_Thunks_Process_Terminating)\n"
            "    {\n"
            "        if (__YY_Thunks_Process_Terminating != 0)\n"
            "            return __YY_Thunks_Process_Terminating == -1;\n"
            "    }"
        ),
        repl=(
            "    if (__YY_Thunks_Process_Terminating != 0)\n"
            "        return __YY_Thunks_Process_Terminating == -1;"
        ),
        note="__YY_Thunks_Process_Terminating 在 YY_Thunks.h 中恒有定义",
    ),
    dict(
        files=["Thunks/DllMainCRTStartup.hpp"],
        kind="literal",
        pattern=(
            "            __if_exists(__YY_Thunks_Process_Terminating)\n"
            "            {\n"
            "                __YY_Thunks_Process_Terminating = _pReserved != nullptr ? -1 : 1;\n"
            "            }"
        ),
        repl=(
            "            __YY_Thunks_Process_Terminating = _pReserved != nullptr ? -1 : 1;"
        ),
        required=False,
        note="同上；DllMainCRTStartup.hpp 未启用时可忽略",
    ),

    # ------------------------------------------------------------------ #
    # 5g. 消除对 -fpermissive 的依赖
    #
    #   上游是 MSVC 风格的窄化转换：
    #       (ULONG)NtCurrentTeb()->ClientId.UniqueProcess
    #   把 HANDLE（void*）直接截断成 ULONG。MSVC 只报 warning（C4312/C4302），
    #   而 GCC/Clang 都视为**错误**。
    #
    #   移植原先靠 `-fpermissive` 全局把这类错误降级为警告，代价太大 ——
    #   它会把真正的类型错误一起放过去，等于放弃了类型检查。
    #   改为显式经 ULONG_PTR 中转：语义与 MSVC 完全一致（同样是截断到 32 位，
    #   64 位下先无损转为 ULONG_PTR 再截断），但不再需要任何宽容开关。
    #
    #   实测（去掉 -fpermissive 后）：Vista / Win8 / Win10 目标仅剩这 3 处报错，
    #   修掉即可完全去掉该选项。
    # ------------------------------------------------------------------ #
    dict(
        files=["Thunks/user32.hpp", "Thunks/shcore.hpp"],
        kind="literal",
        pattern="(ULONG)NtCurrentTeb()->ClientId.UniqueProcess",
        repl="(ULONG)(ULONG_PTR)NtCurrentTeb()->ClientId.UniqueProcess",
        note="显式经 ULONG_PTR 中转，消除对 -fpermissive 的依赖",
    ),
]
