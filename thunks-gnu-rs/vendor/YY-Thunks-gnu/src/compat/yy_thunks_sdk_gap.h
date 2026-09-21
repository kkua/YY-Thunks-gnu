/*
 * yy_thunks_sdk_gap.h —— 补齐 MinGW-w64 相对 Windows SDK 缺失的少量声明
 *
 * YY-Thunks 是按"用最新 Windows SDK 编译、在旧系统上运行"的思路写的，
 * 因此会直接使用 SDK 里的较新类型/枚举/函数原型。
 * MinGW-w64 的 SDK 头覆盖度略低，这里集中补齐这些缺口。
 *
 * 约定：
 *   - 每一项都单独 #ifndef 保护，一旦 MinGW 将来补上就会自动让位；
 *   - 只补齐"声明/类型"，绝不放实现，避免与 YY-Thunks 自身定义冲突；
 *   - 每项都注明来源，便于随上游 SDK 变化复核。
 */
#pragma once

#ifndef YY_THUNKS_PORT_GW
#error "请先包含 yy_thunks_prelude.h"
#endif

#include <iphlpapi.h>
#include <lm.h>
#include <lmjoin.h>

/* ------------------------------------------------------------------------ *
 * winnt.h：UFIELD_OFFSET
 *   用 __builtin_offsetof 而不是 &((T*)0)->field：
 *   后者在 GCC 的常量求值里会因"对空指针解引用"而拒绝，上游拿它做 static_assert。
 * ------------------------------------------------------------------------ */
#ifndef UFIELD_OFFSET
#define UFIELD_OFFSET(_Type, _Field) ((ULONG)__builtin_offsetof(_Type, _Field))
#endif

/* ------------------------------------------------------------------------ *
 * ws2tcpip.h：GetAddrInfoExCancel（Windows 8+ 的异步解析取消）
 * ------------------------------------------------------------------------ */
#include <ws2tcpip.h>
#if !defined(__YY_SDKGAP_GetAddrInfoExCancel)
#define __YY_SDKGAP_GetAddrInfoExCancel
INT WSAAPI GetAddrInfoExCancel(_In_ HANDLE* lpHandle);
#endif

/* ------------------------------------------------------------------------ *
 * cfgmgr32.h：SPCRP_* （设备类属性）
 * ------------------------------------------------------------------------ */
#ifndef SPCRP_UPPERFILTERS
#define SPCRP_UPPERFILTERS 0x00000011
#endif
#ifndef SPCRP_LOWERFILTERS
#define SPCRP_LOWERFILTERS 0x00000012
#endif

/* ------------------------------------------------------------------------ *
 * powrprof.h / winnt.h：电源通知
 *   DEVICE_NOTIFY_CALLBACK 在上游代码里是"注册类型"的取值（不是类型名）。
 *   DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS 是回调注册的参数结构。
 * ------------------------------------------------------------------------ */
#ifndef DEVICE_NOTIFY_CALLBACK
#define DEVICE_NOTIFY_CALLBACK 0x00000002
#endif

#ifndef _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS_DEFINED
#define _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS_DEFINED
typedef ULONG(WINAPI* PDEVICE_NOTIFY_CALLBACK_ROUTINE)(PVOID Context, ULONG Type, PVOID Setting);
typedef PDEVICE_NOTIFY_CALLBACK_ROUTINE DEVICE_NOTIFY_CALLBACK_ROUTINE;
typedef struct _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS
{
    PDEVICE_NOTIFY_CALLBACK_ROUTINE Callback;
    PVOID Context;
} DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS, *PDEVICE_NOTIFY_SUBSCRIBE_PARAMETERS;
#endif

/* ------------------------------------------------------------------------ *
 * iphlpapi.h：GetIfEntry2Ex（Windows 10 1709+）
 * ------------------------------------------------------------------------ */
#ifndef _MIB_IF_ENTRY_LEVEL_DEFINED
#define _MIB_IF_ENTRY_LEVEL_DEFINED
typedef enum _MIB_IF_ENTRY_LEVEL
{
    MibIfEntryNormal = 0,
    MibIfEntryNormalWithoutStatistics = 2
} MIB_IF_ENTRY_LEVEL;
#endif

/*
 * 函数原型也在这里补一份：MinGW 的 iphlpapi.h 完全没有 GetIfEntry2Ex，
 * 而上游 Thunks/Iphlpapi.hpp 直接把它当作已声明使用。
 * 声明放在 namespace YY::Thunks 内，与上游 __DEFINE_THUNK 的定义同作用域；
 * 返回类型/参数刻意与上游 __DEFINE_THUNK 的实参完全一致（见生成的
 * YY_Thunks_Declarations.hpp），避免变成"冲突声明"。
 */
namespace YY::Thunks
{
#if !defined(__YY_SDKGAP_GetIfEntry2Ex)
#define __YY_SDKGAP_GetIfEntry2Ex
// 必须写 EXTERN_C（C 链接）：上游 __DEFINE_THUNK 用 EXTERN_C 定义该函数，
// 若这里以 C++ 链接声明会与之冲突。
EXTERN_C DWORD NETIOAPI_API_ GetIfEntry2Ex(_In_ MIB_IF_ENTRY_LEVEL _eLevel, _Inout_ PMIB_IF_ROW2 _pRow);
#endif
} // namespace YY::Thunks

/* ------------------------------------------------------------------------ *
 * dsreg.h / lmjoin.h：Azure AD 加入信息（Windows 10+）
 * ------------------------------------------------------------------------ */
#if !defined(__YY_SDKGAP_PDSREG_JOIN_INFO)
#define __YY_SDKGAP_PDSREG_JOIN_INFO
typedef struct _DSREG_JOIN_INFO* PDSREG_JOIN_INFO;
#endif

/* 函数原型同上：这里只补 PDSREG_JOIN_INFO 类型。 */

/* ------------------------------------------------------------------------ *
 * fileapi.h：CreateFile3（Windows 11 24H2+）
 * ------------------------------------------------------------------------ */
#ifndef _CREATEFILE3_EXTENDED_PARAMETERS_DEFINED
#define _CREATEFILE3_EXTENDED_PARAMETERS_DEFINED
// 上游用一串 static_assert 证明 CreateFile3 的扩展参数结构与 CreateFile2 完全一致
// （见 api-ms-win-core-file.hpp），因此这里直接复用 CreateFile2 的结构体定义。
typedef CREATEFILE2_EXTENDED_PARAMETERS CREATEFILE3_EXTENDED_PARAMETERS;
typedef CREATEFILE2_EXTENDED_PARAMETERS* PCREATEFILE3_EXTENDED_PARAMETERS;
typedef CREATEFILE2_EXTENDED_PARAMETERS* LPCREATEFILE3_EXTENDED_PARAMETERS;
#endif

namespace YY::Thunks
{
#if !defined(__YY_SDKGAP_CreateFile3)
#define __YY_SDKGAP_CreateFile3
EXTERN_C HANDLE WINAPI CreateFile3(
    _In_z_ LPCWSTR _szFileName,
    _In_ DWORD _uDesiredAccess,
    _In_ DWORD _uShareMode,
    _In_ DWORD _uCreationDisposition,
    _In_opt_ LPCREATEFILE3_EXTENDED_PARAMETERS _pCreateExParams);
#endif
} // namespace YY::Thunks
