/*
 * ncryptprotect.h —— MinGW-w64 未提供的 SDK 头文件替代实现
 *
 * 背景：上游 Thunks/ncrypy.hpp 里 CNG 数据保护 API（NCryptProtectSecret 等）
 * 的兜底实现需要这个头文件，而 MinGW-w64 的 include 目录里没有它。
 *
 * 本文件放在 src/compat/ 下，而 compat 在编译器的 include 搜索路径中排在
 * 系统目录之前（见 CMakeLists 的 YY_THUNKS_INCLUDE_DIRS 与 Rust crate 的
 * build.include），因此 `#include <ncryptprotect.h>` 会解析到本文件。
 *
 * 声明与上游 __DEFINE_THUNK 的参数表逐字对齐，避免变成"冲突声明"；
 * 参数名沿用 Windows SDK 的命名。
 */
#ifndef __YY_COMPAT_NCRYPTPROTECT_H
#define __YY_COMPAT_NCRYPTPROTECT_H

/* SECURITY_STATUS、NCRYPT_ALLOC_PARA 等类型由 ncrypt.h 提供
 * （上游在包含本文件之前已包含 ncrypt.h）。 */
#include <ncrypt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SDK: DECLARE_HANDLE(NCRYPT_DESCRIPTOR_HANDLE) —— 指针大小的不透明句柄。
 * 上游用 reinterpret_cast 把自己的内部对象指针转成它，故必须是指针类型。 */
typedef void* NCRYPT_DESCRIPTOR_HANDLE;

/* SDK 中的三个标志位（均为 0x1），ncrypt.h 不提供。 */
#ifndef NCRYPT_NAMED_DESCRIPTOR_FLAG
#define NCRYPT_NAMED_DESCRIPTOR_FLAG 0x00000001
#endif

#ifndef NCRYPT_PROTECTION_INFO_TYPE_DESCRIPTOR_STRING
#define NCRYPT_PROTECTION_INFO_TYPE_DESCRIPTOR_STRING 0x00000001
#endif

#ifndef NCRYPT_UNPROTECT_NO_DECRYPT
#define NCRYPT_UNPROTECT_NO_DECRYPT 0x00000001
#endif

SECURITY_STATUS WINAPI NCryptCreateProtectionDescriptor(
    _In_opt_ LPCWSTR pwszDescriptorString,
    _In_opt_ DWORD dwFlags,
    _Out_ NCRYPT_DESCRIPTOR_HANDLE* phDescriptor);

SECURITY_STATUS WINAPI NCryptCloseProtectionDescriptor(
    _In_ NCRYPT_DESCRIPTOR_HANDLE hDescriptor);

SECURITY_STATUS WINAPI NCryptGetProtectionDescriptorInfo(
    _In_ NCRYPT_DESCRIPTOR_HANDLE hDescriptor,
    _In_opt_ const NCRYPT_ALLOC_PARA* pMemPara,
    _In_ DWORD dwInfoType,
    _Outptr_ void** ppvInfo);

SECURITY_STATUS WINAPI NCryptProtectSecret(
    _In_ NCRYPT_DESCRIPTOR_HANDLE hDescriptor,
    _In_ DWORD dwFlags,
    _In_reads_bytes_(cbData) const BYTE* pbData,
    _In_ ULONG cbData,
    _In_opt_ const NCRYPT_ALLOC_PARA* pMemPara,
    _In_opt_ HWND hWnd,
    _Out_ PBYTE* ppbProtectedBlob,
    _Out_ ULONG* pcbProtectedBlob);

SECURITY_STATUS WINAPI NCryptUnprotectSecret(
    _Out_opt_ NCRYPT_DESCRIPTOR_HANDLE* phDescriptor,
    _In_opt_ DWORD dwFlags,
    _In_reads_bytes_(cbProtectedBlob) const BYTE* pbProtectedBlob,
    _In_ ULONG cbProtectedBlob,
    _In_opt_ const NCRYPT_ALLOC_PARA* pMemPara,
    _In_opt_ HWND hWnd,
    _Out_ PBYTE* ppbData,
    _Out_ ULONG* pcbData);

#ifdef __cplusplus
}
#endif

#endif // __YY_COMPAT_NCRYPTPROTECT_H
