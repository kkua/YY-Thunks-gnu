/*
 * roerrorapi.h —— MinGW-w64 未提供的 SDK 头文件替代实现
 *
 * 背景：上游 Thunks/api-ms-win-core-winrt-error.hpp 需要这个头文件，
 * 而 MinGW-w64 的 include 目录里没有它（combase.dll 上的 WinRT 错误 API）。
 *
 * 同 ncryptprotect.h：本文件放在 src/compat/ 下，靠 include 搜索路径的
 * 优先级接管 `#include <roerrorapi.h>`。
 *
 * 声明与上游 __DEFINE_THUNK 的参数表逐字对齐。
 */
#ifndef __YY_COMPAT_ROERRORAPI_H
#define __YY_COMPAT_ROERRORAPI_H

/* HSTRING 来自 winstring.h；IUnknown 来自 unknwn.h。
 * 两者 MinGW-w64 均有提供，这里只做转接。 */
#include <winstring.h>
#include <unknwn.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SDK 中的错误消息最大字符数（RoOriginateErrorW / RoTransformErrorW 用）。 */
#ifndef MAX_ERROR_MESSAGE_CHARS
#define MAX_ERROR_MESSAGE_CHARS 512
#endif

BOOL WINAPI RoOriginateError(
    _In_ HRESULT hrError,
    _In_opt_ HSTRING szMessage);

BOOL WINAPI RoOriginateErrorW(
    _In_ HRESULT hrError,
    _In_ UINT cchMax,
    _When_(cchMax == 0, _In_reads_or_z_opt_(MAX_ERROR_MESSAGE_CHARS))
    _When_(cchMax > 0 && cchMax < MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(cchMax))
    _When_(cchMax >= MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(MAX_ERROR_MESSAGE_CHARS))
    PCWSTR szMessage);

void WINAPI RoFailFastWithErrorContext(
    _In_ HRESULT hrError);

BOOL WINAPI RoOriginateLanguageException(
    _In_ HRESULT hrError,
    _In_opt_ HSTRING szMessage,
    _In_opt_ IUnknown* pLanguageException);

BOOL WINAPI RoTransformError(
    _In_ HRESULT hrOldError,
    _In_ HRESULT hrNewError,
    _In_opt_ HSTRING szMessage);

BOOL WINAPI RoTransformErrorW(
    _In_ HRESULT hrOldError,
    _In_ HRESULT hrNewError,
    _In_ UINT cchMax,
    _When_(cchMax == 0, _In_reads_or_z_opt_(MAX_ERROR_MESSAGE_CHARS))
    _When_(cchMax > 0 && cchMax < MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(cchMax))
    _When_(cchMax >= MAX_ERROR_MESSAGE_CHARS, _In_reads_or_z_(MAX_ERROR_MESSAGE_CHARS))
    PCWSTR szMessage);

HRESULT WINAPI GetRestrictedErrorInfo(
    _Out_ IUnknown** ppRestrictedErrorInfo);

#ifdef __cplusplus
}
#endif

#endif // __YY_COMPAT_ROERRORAPI_H
