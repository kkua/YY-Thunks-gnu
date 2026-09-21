/*
 * yy_thunks_postlude.h —— YY-Thunks MinGW 移植层的"后置垫片"
 *
 * 必须在 <Windows.h>（以及上游的 YY_Thunks.h）之后包含。
 *
 * 职责：补充上游依赖 MSVC CRT / LibMaker.exe 才能得到的"可选弱符号"。
 * 上游把这些符号声明为 weak extern（MSVC 侧由 LibMaker FixObj 处理），
 * 使用者可以在自己的工程里强定义它们来定制行为。
 * GCC 侧改为"弱定义 + 默认值"，语义等价：用户强定义会覆盖弱定义。
 */
#pragma once

#ifndef YY_THUNKS_PORT_GW
#error "请先包含 yy_thunks_prelude.h"
#endif

/* 补齐 MinGW-w64 相对 Windows SDK 缺失的少量声明 */
#include <yy_thunks_sdk_gap.h>

/*
 * GUID_NULL
 *
 * MinGW 的 guiddef.h 只给出 `EXTERN_C const GUID GUID_NULL;` 声明，
 * 定义在 libuuid.a 里。上游 shell32.hpp 会把它按 const 引用使用（ODR-use），
 * 于是最终链接必须额外带上 uuid。
 *
 * YY-Thunks 的产物定位是"塞进去就能用"，不应额外要求使用者链接 uuid，
 * 所以在库内提供一个 **弱定义**：使用者或 libuuid.a 若也提供强定义，以强定义为准。
 */
extern "C"
{
    extern __attribute__((weak)) const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
}

/*
 * MinGW 的 winnt.h 把 UNREFERENCED_PARAMETER 定义为 `{(P) = (P);}`，
 * 一旦 P 是 const 或函数指针就会编译失败。上游大量用于"消除未使用参数告警"，
 * 这里换成等价且安全的写法。
 */
#ifdef UNREFERENCED_PARAMETER
#undef UNREFERENCED_PARAMETER
#endif
#define UNREFERENCED_PARAMETER(_Param) ((void)(_Param))

extern "C"
{
    /*
     * MSVC CRT 的栈保护 Cookie。上游会读取它来派生自己的指针加密密钥。
     * MinGW 的 CRT 不导出该符号（用的是 __stack_chk_guard），
     * 这里给出弱定义；若链接进了强定义（例如 libssp），以强定义为准。
     */
    __attribute__((weak)) UINT_PTR __security_cookie = 0;

    /*
     * UCRT 的 atexit 表。上游用它判断"当前 CRT 是否为静态链接"。
     * MinGW 没有该符号，恒为 nullptr，等价于"非 UCRT"，上游会走 atexit 分支。
     */
    __attribute__((weak)) void* __acrt_atexit_table = nullptr;

    /*
     * 是否关闭"DLL 预载"。上游注释见 YY_Thunks.h。
     * 使用者可以强定义 __YY_Thunks_Disable_Rreload_Dlls = TRUE 来关闭预载。
     */
    __attribute__((weak)) BOOL __YY_Thunks_Disable_Rreload_Dlls = FALSE;

    /*
     * 自定义 DLL 加载器。返回 nullptr 表示回落到 YY-Thunks 默认加载流程。
     * 注意：声明处带 const（指针本身 const），必须再加 extern 才能获得外部链接，
     * 否则 GCC 不允许对内部链接符号使用 weak 属性。
     */
    extern __attribute__((weak))
    HMODULE(__fastcall* const __pfnYY_Thunks_CustomLoadLibrary)(const wchar_t* _szModuleName, DWORD _fFlags) = nullptr;
}

/*
 * InterlockedCompareExchange128
 *
 * MSVC 在 winnt.h 中提供该固有函数，MinGW 完全没有。
 * 上游 Thunks/api-ms-win-core-kernel32-legacy.hpp 在 _WIN64 下使用它，
 * 这里用内联汇编实现与之完全一致的语义：
 *
 *   ZF = (RDX:RAX == [Destination])
 *   相等   -> [Destination] = RCX:RBX
 *   不相等 -> RDX:RAX = [Destination]
 *   若失败，把实际值写回 ComparandResult
 *
 * 注意：cmpxchg16b 要求 Destination 16 字节对齐（与 MSVC 的约定一致）。
 */
#if defined(_WIN64) && defined(__x86_64__)
extern "C" __attribute__((always_inline)) inline BOOLEAN
InterlockedCompareExchange128(
    volatile INT64* _pDestination,
    INT64 _ExchangeHigh,
    INT64 _ExchangeLow,
    INT64* _pComparandResult) noexcept
{
    unsigned char _fSucceed = 0;
    INT64 _uHigh = _pComparandResult[1];
    INT64 _uLow = _pComparandResult[0];

    __asm__ __volatile__(
        "lock cmpxchg16b %1\n\t"
        "sete %0"
        : "=q"(_fSucceed), "+m"(*_pDestination), "+a"(_uLow), "+d"(_uHigh)
        : "b"(_ExchangeLow), "c"(_ExchangeHigh)
        : "cc", "memory");

    if (!_fSucceed)
    {
        _pComparandResult[0] = _uLow;
        _pComparandResult[1] = _uHigh;
    }

    return _fSucceed;
}
#endif
