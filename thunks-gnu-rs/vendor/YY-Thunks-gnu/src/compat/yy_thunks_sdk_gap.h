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
#include <windns.h>

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
 *
 * 必须用 EXTERN_C：上游 Thunks/WS2_32.hpp 的 thunk 以 extern "C" 定义，
 * 这里若用 C++ 链接声明，二者是不同的函数，`__imp_*` 初始化会指向
 * mangled 符号并在链接期报 undefined reference。
 * ------------------------------------------------------------------------ */
#include <ws2tcpip.h>
#if !defined(__YY_SDKGAP_GetAddrInfoExCancel)
#define __YY_SDKGAP_GetAddrInfoExCancel
EXTERN_C INT WSAAPI GetAddrInfoExCancel(_In_ HANDLE* lpHandle);
#endif

/* ------------------------------------------------------------------------ *
 * winstring.h：E_STRING_NOT_NULL_TERMINATED（WinRT 字符串错误码）
 *
 * MinGW 的 winstring.h 只声明函数，没有这个 HRESULT 常量
 * （SDK 中定义在 winstring.h，值 0x80000017）。
 * 上游 api-ms-win-core-winrt-string.hpp 的 WindowsCreateStringReference
 * 兜底会返回它。
 * ------------------------------------------------------------------------ */
#ifndef E_STRING_NOT_NULL_TERMINATED
#define E_STRING_NOT_NULL_TERMINATED ((HRESULT)0x80000017L)
#endif

/* ------------------------------------------------------------------------ *
 * ws2tcpip.h：GetAddrInfoExOverlappedResult（Windows 8+ 的异步解析结果查询）
 *
 * MinGW 的 ws2tcpip.h 只声明到 GetAddrInfoExCancel，没有这一个；
 * 上游 Thunks/WS2_32.hpp 直接把它当作已声明使用（__DEFINE_THUNK 的参数表
 * 依赖这里的原型），缺失时报 "was not declared in this scope"。
 * 签名与上游 __DEFINE_THUNK 的实参一致，并用 EXTERN_C 匹配 thunk 的链接。
 * ------------------------------------------------------------------------ */
#if !defined(__YY_SDKGAP_GetAddrInfoExOverlappedResult)
#define __YY_SDKGAP_GetAddrInfoExOverlappedResult
EXTERN_C INT WSAAPI GetAddrInfoExOverlappedResult(_In_ LPOVERLAPPED lpOverlapped);
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
 * windns.h：DnsQueryEx / DnsCancelQuery（Windows 8+ 的异步 DNS 查询）
 *
 * MinGW 的 windns.h 有 DNS_QUERY_RESULT / DNS_QUERY_COMPLETION_ROUTINE /
 * DNS_ADDR，但缺 DNS_QUERY_REQUEST、DNS_QUERY_CANCEL、DNS_ADDR_ARRAY
 * 以及这两个函数。
 *
 * 结构体布局必须与 Windows SDK 逐字一致：调用方按 SDK 布局填充，
 * 新系统上 thunk 会原样交给真实的 DnsQueryEx。这里以上游的实际用法
 * 交叉校验过：
 *   - pQueryCompletionCallback / QueryName / QueryType / QueryOptions /
 *     pDnsServerList / Version —— 对应 _DNS_QUERY_REQUEST 的字段序；
 *   - sizeof(_DnsAddrArray) + (AddrCount - 1) * sizeof(DNS_ADDR)
 *     —— 与 AddrArray[ANYSIZE_ARRAY]（SDK 中 ANYSIZE_ARRAY == 1）自洽；
 *   - DnsQueryCancel 联合体只要求 >= sizeof(LSTATUS)，SDK 的 32 字节满足。
 * ------------------------------------------------------------------------ */
#ifndef __YY_SDKGAP_DNS_ADDR
#define __YY_SDKGAP_DNS_ADDR
/* SDK: MaxSa 在最前且必须容纳最大的 SOCKADDR（sockaddr_in6 为 28 字节），
 * 上游直接 reinterpret_cast<SOCKADDR_IN*>(AddrArray[i].MaxSa)。 */
#define DNS_ADDR_MAX_SOCKADDR_LENGTH 32
typedef struct _DNS_ADDR
{
    CHAR MaxSa[DNS_ADDR_MAX_SOCKADDR_LENGTH];
    DWORD DnsAddrUserDword;
} DNS_ADDR;
#endif

#ifndef __YY_SDKGAP_DNS_ADDR_ARRAY
#define __YY_SDKGAP_DNS_ADDR_ARRAY
typedef struct _DnsAddrArray
{
    DWORD MaxCount;
    DWORD AddrCount;
    DWORD Tag;
    USHORT Family;
    USHORT Reserved;
    DWORD Flags;
    DWORD MatchFlag;
    DWORD Reserved1;
    DWORD Reserved2;
    DNS_ADDR AddrArray[ANYSIZE_ARRAY];
} DNS_ADDR_ARRAY, *PDNS_ADDR_ARRAY;
#endif

#ifndef __YY_SDKGAP_DNS_QUERY_REQUEST
#define __YY_SDKGAP_DNS_QUERY_REQUEST
typedef struct _DNS_QUERY_REQUEST
{
    ULONG Version;
    PCWSTR QueryName;
    USHORT QueryType;
    ULONG64 QueryOptions;
    PDNS_ADDR_ARRAY pDnsServerList;
    ULONG InterfaceIndex;
    PDNS_QUERY_COMPLETION_ROUTINE pQueryCompletionCallback;
    PVOID pQueryContext;
} DNS_QUERY_REQUEST, *PDNS_QUERY_REQUEST;
#endif

#ifndef __YY_SDKGAP_DNS_QUERY_CANCEL
#define __YY_SDKGAP_DNS_QUERY_CANCEL
typedef struct _DNS_QUERY_CANCEL
{
    CHAR Reserved[32];
} DNS_QUERY_CANCEL, *PDNS_QUERY_CANCEL;
#endif

#if !defined(__YY_SDKGAP_DnsQueryEx)
#define __YY_SDKGAP_DnsQueryEx
EXTERN_C DNS_STATUS WINAPI DnsQueryEx(
    _In_ PDNS_QUERY_REQUEST pQueryRequest,
    _Inout_ PDNS_QUERY_RESULT pQueryResults,
    _Inout_opt_ PDNS_QUERY_CANCEL pCancelHandle);
#endif

#if !defined(__YY_SDKGAP_DnsCancelQuery)
#define __YY_SDKGAP_DnsCancelQuery
EXTERN_C DNS_STATUS WINAPI DnsCancelQuery(
    _In_ PDNS_QUERY_CANCEL pCancelHandle);
#endif

/* ------------------------------------------------------------------------ *
 * lmjoin.h：NetGetAadJoinInformation / NetFreeAadJoinInformation（Windows 10+）
 *
 * MinGW 的 lmjoin.h 只到 NetGetJoinableOUs；PDSREG_JOIN_INFO 已由本文件
 * 前面的 dsreg/lmjoin 一节补齐。
 * ------------------------------------------------------------------------ */
#if !defined(__YY_SDKGAP_NetGetAadJoinInformation)
#define __YY_SDKGAP_NetGetAadJoinInformation
EXTERN_C HRESULT WINAPI NetGetAadJoinInformation(
    _In_opt_ LPCWSTR szTenantId,
    _Outptr_result_maybenull_ PDSREG_JOIN_INFO* ppJoinInfo);
#endif

#if !defined(__YY_SDKGAP_NetFreeAadJoinInformation)
#define __YY_SDKGAP_NetFreeAadJoinInformation
EXTERN_C VOID WINAPI NetFreeAadJoinInformation(
    _In_opt_ PDSREG_JOIN_INFO pJoinInfo);
#endif

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
