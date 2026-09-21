// 本文件由 tools/portgen.py 自动生成，请勿手工修改！
//
// 上游 Thunks/YY_Thunks.cpp 会先以"只声明不定义"的方式把 YY_Thunks_List.hpp
// 包含一遍（第一遍），以便任意 thunk 头都能调用其它 thunk 的 try_get_*。
// 本文件就是那一遍的产物：所有参与编译的 thunk 的
//     - API 函数声明
//     - 函数类型 typedef（YY_THUNK_FNTYPE_<Api>）
//     - static try_get_<Api>() 前置声明
//     - 单元测试用的探针变量
// 全部由预处理探测一次性生成，顺序与上游登记顺序一致。
//
// 关于 typedef：上游用 decltype(函数名) 取函数类型。这在 MSVC 下成立，
// 但 MinGW 的 pathcch.h 等头会额外提供同名的 C++ 内联重载
// （如 PathIsUNCEx(PWSTR, PWSTR*)），与 extern "C" 版本共同构成重载集，
// 使 decltype / 取址产生二义。改为显式 typedef 后取址与类型推导都不再依赖重载解析。
//
// 注意：因为由预处理器展开，这里的类型/调用约定已经展开为最终形式；
// 生成时使用的宏环境必须与真实编译一致（CMake 已保证）。

#pragma once

// ---- 各 thunk 头依赖的 SDK / 共享头（保证声明中的类型可见）----
#include <appmodel.h>
#include <combaseapi.h>
#include <processtopologyapi.h>
#include <werapi.h>
#include <evntprov.h>
#include <evntrace.h>
#include <powrprof.h>
#include <userenv.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <bluetoothleapis.h>
#include <cfgmgr32.h>
#include <SetupAPI.h>
#include <wincred.h>
#include <dpapi.h>
#include <d3d12.h>
#include <d3d9.h>
#include <dbghelp.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <dxva2api.h>
#include <Esent.h>
#include <winuser.h>
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <mmdeviceapi.h>
#include <pdh.h>
#include <propvarutil.h>
#include <uiautomation.h>
#include <uxtheme.h>
#include <winevt.h>
#include <winhttp.h>
#include <winusb.h>
#include <pathcch.h>
#include <strsafe.h>
#include <processthreadsapi.h>
#include <Shared/SmBios.h>
#include <threadpoolapiset.h>
#include <timezoneapi.h>
#include <psapi.h>
#include <setupapi.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <shellscalingapi.h>
#include <ShellScalingAPI.h>
#include <shcore.h>

namespace YY::Thunks
{
#if YY_THUNK_GUARD_try_get_GetCurrentPackageFullName
__APPLY_UNIT_TEST_BOOL(GetCurrentPackageFullName); typedef LONG(WINAPI* YY_THUNK_FNTYPE_GetCurrentPackageFullName)(_Inout_ UINT32* _pcPackageFullNameLength, PWSTR _szPackageFullName); extern "C" LONG WINAPI __FALLBACK_PREFIXGetCurrentPackageFullName(_Inout_ UINT32* _pcPackageFullNameLength, PWSTR _szPackageFullName); static YY_THUNK_FNTYPE_GetCurrentPackageFullName __attribute__((__cdecl__)) try_get_GetCurrentPackageFullName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPackageFullName
__APPLY_UNIT_TEST_BOOL(GetPackageFullName); typedef LONG(WINAPI* YY_THUNK_FNTYPE_GetPackageFullName)(_In_ HANDLE _hProcess, _Inout_ UINT32* _pcchPackageFullNameLength, PWSTR _szPackageFullName); extern "C" LONG WINAPI __FALLBACK_PREFIXGetPackageFullName(_In_ HANDLE _hProcess, _Inout_ UINT32* _pcchPackageFullNameLength, PWSTR _szPackageFullName); static YY_THUNK_FNTYPE_GetPackageFullName __attribute__((__cdecl__)) try_get_GetPackageFullName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPackageFamilyName
__APPLY_UNIT_TEST_BOOL(GetPackageFamilyName); typedef LONG(WINAPI* YY_THUNK_FNTYPE_GetPackageFamilyName)(_In_ HANDLE _hProcess, _Inout_ UINT32* _pcchPackageFamilyNameLength, PWSTR _szPackageFamilyName); extern "C" LONG WINAPI __FALLBACK_PREFIXGetPackageFamilyName(_In_ HANDLE _hProcess, _Inout_ UINT32* _pcchPackageFamilyNameLength, PWSTR _szPackageFamilyName); static YY_THUNK_FNTYPE_GetPackageFamilyName __attribute__((__cdecl__)) try_get_GetPackageFamilyName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_OpenPackageInfoByFullName
__APPLY_UNIT_TEST_BOOL(OpenPackageInfoByFullName); typedef LONG(WINAPI* YY_THUNK_FNTYPE_OpenPackageInfoByFullName)(_In_ PCWSTR _szPackageFullName, const UINT32 _uReserved, _Out_ PACKAGE_INFO_REFERENCE* _pPackageInfoReference); extern "C" LONG WINAPI __FALLBACK_PREFIXOpenPackageInfoByFullName(_In_ PCWSTR _szPackageFullName, const UINT32 _uReserved, _Out_ PACKAGE_INFO_REFERENCE* _pPackageInfoReference); static YY_THUNK_FNTYPE_OpenPackageInfoByFullName __attribute__((__cdecl__)) try_get_OpenPackageInfoByFullName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ClosePackageInfo
__APPLY_UNIT_TEST_BOOL(ClosePackageInfo); typedef LONG(WINAPI* YY_THUNK_FNTYPE_ClosePackageInfo)(_In_ PACKAGE_INFO_REFERENCE _PackageInfoReference); extern "C" LONG WINAPI __FALLBACK_PREFIXClosePackageInfo(_In_ PACKAGE_INFO_REFERENCE _PackageInfoReference); static YY_THUNK_FNTYPE_ClosePackageInfo __attribute__((__cdecl__)) try_get_ClosePackageInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPackageInfo
__APPLY_UNIT_TEST_BOOL(GetPackageInfo); typedef LONG(WINAPI* YY_THUNK_FNTYPE_GetPackageInfo)(_In_ PACKAGE_INFO_REFERENCE _PackageInfoReference, _In_ const UINT32 _fFlags, _Inout_ UINT32* _pcbBufferLength, BYTE* _pBuffer, _Out_opt_ UINT32* _pcCount); extern "C" LONG WINAPI __FALLBACK_PREFIXGetPackageInfo(_In_ PACKAGE_INFO_REFERENCE _PackageInfoReference, _In_ const UINT32 _fFlags, _Inout_ UINT32* _pcbBufferLength, BYTE* _pBuffer, _Out_opt_ UINT32* _pcCount); static YY_THUNK_FNTYPE_GetPackageInfo __attribute__((__cdecl__)) try_get_GetPackageInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPackagePathByFullName
__APPLY_UNIT_TEST_BOOL(GetPackagePathByFullName); typedef LONG(WINAPI* YY_THUNK_FNTYPE_GetPackagePathByFullName)(_In_ PCWSTR _szPackageFullName, _Inout_ UINT32* _pcchPathLength, PWSTR _szPath); extern "C" LONG WINAPI __FALLBACK_PREFIXGetPackagePathByFullName(_In_ PCWSTR _szPackageFullName, _Inout_ UINT32* _pcchPathLength, PWSTR _szPath); static YY_THUNK_FNTYPE_GetPackagePathByFullName __attribute__((__cdecl__)) try_get_GetPackagePathByFullName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPackagesByPackageFamily
__APPLY_UNIT_TEST_BOOL(GetPackagesByPackageFamily); typedef LONG(WINAPI* YY_THUNK_FNTYPE_GetPackagesByPackageFamily)(_In_ PCWSTR _szPackageFamilyName, _Inout_ UINT32* _pcCount, PWSTR* _pszPackageFullNames, _Inout_ UINT32* _pcchBufferLength, WCHAR* _szBuffer); extern "C" LONG WINAPI __FALLBACK_PREFIXGetPackagesByPackageFamily(_In_ PCWSTR _szPackageFamilyName, _Inout_ UINT32* _pcCount, PWSTR* _pszPackageFullNames, _Inout_ UINT32* _pcchBufferLength, WCHAR* _szBuffer); static YY_THUNK_FNTYPE_GetPackagesByPackageFamily __attribute__((__cdecl__)) try_get_GetPackagesByPackageFamily() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_FindPackagesByPackageFamily
__APPLY_UNIT_TEST_BOOL(FindPackagesByPackageFamily); typedef LONG(WINAPI* YY_THUNK_FNTYPE_FindPackagesByPackageFamily)(_In_ PCWSTR _szPackageFamilyName, _In_ UINT32 _uPackageFilters, _Inout_ UINT32* _pcCount, PWSTR* _pszPackageFullNames, _Inout_ UINT32* _pcchBufferLength, WCHAR* _szBuffer, UINT32* _pcPackageProperties); extern "C" LONG WINAPI __FALLBACK_PREFIXFindPackagesByPackageFamily(_In_ PCWSTR _szPackageFamilyName, _In_ UINT32 _uPackageFilters, _Inout_ UINT32* _pcCount, PWSTR* _pszPackageFullNames, _Inout_ UINT32* _pcchBufferLength, WCHAR* _szBuffer, UINT32* _pcPackageProperties); static YY_THUNK_FNTYPE_FindPackagesByPackageFamily __attribute__((__cdecl__)) try_get_FindPackagesByPackageFamily() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CoGetApartmentType
__APPLY_UNIT_TEST_BOOL(CoGetApartmentType); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CoGetApartmentType)( APTTYPE* _pAptType, APTTYPEQUALIFIER* _pAptQualifier); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCoGetApartmentType( APTTYPE* _pAptType, APTTYPEQUALIFIER* _pAptQualifier); static YY_THUNK_FNTYPE_CoGetApartmentType __attribute__((__cdecl__)) try_get_CoGetApartmentType() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RoGetAgileReference
__APPLY_UNIT_TEST_BOOL(RoGetAgileReference); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RoGetAgileReference)( enum AgileReferenceOptions _eOptions, const IID & _Id, IUnknown* pUnk, IAgileReference** _ppAgileReference); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXRoGetAgileReference( enum AgileReferenceOptions _eOptions, const IID & _Id, IUnknown* pUnk, IAgileReference** _ppAgileReference); static YY_THUNK_FNTYPE_RoGetAgileReference __attribute__((__cdecl__)) try_get_RoGetAgileReference() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CoIncrementMTAUsage
__APPLY_UNIT_TEST_BOOL(CoIncrementMTAUsage); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CoIncrementMTAUsage)( CO_MTA_USAGE_COOKIE* _pCookie); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCoIncrementMTAUsage( CO_MTA_USAGE_COOKIE* _pCookie); static YY_THUNK_FNTYPE_CoIncrementMTAUsage __attribute__((__cdecl__)) try_get_CoIncrementMTAUsage() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CoDecrementMTAUsage
__APPLY_UNIT_TEST_BOOL(CoDecrementMTAUsage); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CoDecrementMTAUsage)( CO_MTA_USAGE_COOKIE _hCookie); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCoDecrementMTAUsage( CO_MTA_USAGE_COOKIE _hCookie); static YY_THUNK_FNTYPE_CoDecrementMTAUsage __attribute__((__cdecl__)) try_get_CoDecrementMTAUsage() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CloseHandle
__APPLY_UNIT_TEST_BOOL(CloseHandle); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CloseHandle)( HANDLE _hObject); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXCloseHandle( HANDLE _hObject); static YY_THUNK_FNTYPE_CloseHandle __attribute__((__cdecl__)) try_get_CloseHandle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DuplicateHandle
__APPLY_UNIT_TEST_BOOL(DuplicateHandle); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DuplicateHandle)( HANDLE _hSourceProcessHandle, HANDLE _hSourceHandle, HANDLE _hTargetProcessHandle, LPHANDLE _phTargetHandle, DWORD _fDesiredAccess, BOOL _bInheritHandle, DWORD _uOptions); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXDuplicateHandle( HANDLE _hSourceProcessHandle, HANDLE _hSourceHandle, HANDLE _hTargetProcessHandle, LPHANDLE _phTargetHandle, DWORD _fDesiredAccess, BOOL _bInheritHandle, DWORD _uOptions); static YY_THUNK_FNTYPE_DuplicateHandle __attribute__((__cdecl__)) try_get_DuplicateHandle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CompareObjectHandles
__APPLY_UNIT_TEST_BOOL(CompareObjectHandles); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CompareObjectHandles)( HANDLE _hFirstObjectHandle, HANDLE _hSecondObjectHandle); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXCompareObjectHandles( HANDLE _hFirstObjectHandle, HANDLE _hSecondObjectHandle); static YY_THUNK_FNTYPE_CompareObjectHandles __attribute__((__cdecl__)) try_get_CompareObjectHandles() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PowerCreateRequest
__APPLY_UNIT_TEST_BOOL(PowerCreateRequest); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PowerCreateRequest)( PREASON_CONTEXT _pContext); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXPowerCreateRequest( PREASON_CONTEXT _pContext); static YY_THUNK_FNTYPE_PowerCreateRequest __attribute__((__cdecl__)) try_get_PowerCreateRequest() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PowerSetRequest
__APPLY_UNIT_TEST_BOOL(PowerSetRequest); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PowerSetRequest)( HANDLE _hPowerRequest, POWER_REQUEST_TYPE _eRequestType); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXPowerSetRequest( HANDLE _hPowerRequest, POWER_REQUEST_TYPE _eRequestType); static YY_THUNK_FNTYPE_PowerSetRequest __attribute__((__cdecl__)) try_get_PowerSetRequest() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PowerClearRequest
__APPLY_UNIT_TEST_BOOL(PowerClearRequest); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PowerClearRequest)( HANDLE _hPowerRequest, POWER_REQUEST_TYPE _eRequestType); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXPowerClearRequest( HANDLE _hPowerRequest, POWER_REQUEST_TYPE _eRequestType); static YY_THUNK_FNTYPE_PowerClearRequest __attribute__((__cdecl__)) try_get_PowerClearRequest() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetFileCompletionNotificationModes
__APPLY_UNIT_TEST_BOOL(SetFileCompletionNotificationModes); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetFileCompletionNotificationModes)( HANDLE _hFileHandle, UCHAR _uFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetFileCompletionNotificationModes( HANDLE _hFileHandle, UCHAR _uFlags); static YY_THUNK_FNTYPE_SetFileCompletionNotificationModes __attribute__((__cdecl__)) try_get_SetFileCompletionNotificationModes() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetProcessGroupAffinity
__APPLY_UNIT_TEST_BOOL(GetProcessGroupAffinity); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetProcessGroupAffinity)( HANDLE _hProcess, PUSHORT _pGroupCount, PUSHORT _pGroupArray); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetProcessGroupAffinity( HANDLE _hProcess, PUSHORT _pGroupCount, PUSHORT _pGroupArray); static YY_THUNK_FNTYPE_GetProcessGroupAffinity __attribute__((__cdecl__)) try_get_GetProcessGroupAffinity() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WerRegisterRuntimeExceptionModule
__APPLY_UNIT_TEST_BOOL(WerRegisterRuntimeExceptionModule); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WerRegisterRuntimeExceptionModule)( PCWSTR _szOutOfProcessCallbackDll, PVOID _pContext); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXWerRegisterRuntimeExceptionModule( PCWSTR _szOutOfProcessCallbackDll, PVOID _pContext); static YY_THUNK_FNTYPE_WerRegisterRuntimeExceptionModule __attribute__((__cdecl__)) try_get_WerRegisterRuntimeExceptionModule() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WerUnregisterRuntimeExceptionModule
__APPLY_UNIT_TEST_BOOL(WerUnregisterRuntimeExceptionModule); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WerUnregisterRuntimeExceptionModule)( PCWSTR _szOutOfProcessCallbackDll, PVOID _pContext); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXWerUnregisterRuntimeExceptionModule( PCWSTR _szOutOfProcessCallbackDll, PVOID _pContext); static YY_THUNK_FNTYPE_WerUnregisterRuntimeExceptionModule __attribute__((__cdecl__)) try_get_WerUnregisterRuntimeExceptionModule() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetEnabledXStateFeatures
__APPLY_UNIT_TEST_BOOL(GetEnabledXStateFeatures); typedef DWORD64(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetEnabledXStateFeatures)(void); extern "C" DWORD64 __attribute__((__stdcall__)) __FALLBACK_PREFIXGetEnabledXStateFeatures(void); static YY_THUNK_FNTYPE_GetEnabledXStateFeatures __attribute__((__cdecl__)) try_get_GetEnabledXStateFeatures() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetXStateFeaturesMask
__APPLY_UNIT_TEST_BOOL(SetXStateFeaturesMask); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetXStateFeaturesMask)( PCONTEXT Context, DWORD64 FeatureMask); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetXStateFeaturesMask( PCONTEXT Context, DWORD64 FeatureMask); static YY_THUNK_FNTYPE_SetXStateFeaturesMask __attribute__((__cdecl__)) try_get_SetXStateFeaturesMask() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_LocateXStateFeature
__APPLY_UNIT_TEST_BOOL(LocateXStateFeature); typedef PVOID(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_LocateXStateFeature)( PCONTEXT Context, DWORD FeatureId, PDWORD Length); extern "C" PVOID __attribute__((__stdcall__)) __FALLBACK_PREFIXLocateXStateFeature( PCONTEXT Context, DWORD FeatureId, PDWORD Length); static YY_THUNK_FNTYPE_LocateXStateFeature __attribute__((__cdecl__)) try_get_LocateXStateFeature() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_InitializeContext
__APPLY_UNIT_TEST_BOOL(InitializeContext); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_InitializeContext)( PVOID Buffer, DWORD ContextFlags, PCONTEXT* Context, PDWORD ContextLength); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXInitializeContext( PVOID Buffer, DWORD ContextFlags, PCONTEXT* Context, PDWORD ContextLength); static YY_THUNK_FNTYPE_InitializeContext __attribute__((__cdecl__)) try_get_InitializeContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_InitializeContext2
__APPLY_UNIT_TEST_BOOL(InitializeContext2); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_InitializeContext2)( PVOID Buffer, DWORD ContextFlags, PCONTEXT* Context, PDWORD ContextLength, ULONG64 XStateCompactionMask); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXInitializeContext2( PVOID Buffer, DWORD ContextFlags, PCONTEXT* Context, PDWORD ContextLength, ULONG64 XStateCompactionMask); static YY_THUNK_FNTYPE_InitializeContext2 __attribute__((__cdecl__)) try_get_InitializeContext2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CopyContext
__APPLY_UNIT_TEST_BOOL(CopyContext); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CopyContext)( PCONTEXT _pDestination, DWORD _uContextFlags, PCONTEXT _pSource); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXCopyContext( PCONTEXT _pDestination, DWORD _uContextFlags, PCONTEXT _pSource); static YY_THUNK_FNTYPE_CopyContext __attribute__((__cdecl__)) try_get_CopyContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_EventSetInformation
__APPLY_UNIT_TEST_BOOL(EventSetInformation); typedef ULONG(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_EventSetInformation)( REGHANDLE _hRegHandle, EVENT_INFO_CLASS _eInformationClass, PVOID _pEventInformation, ULONG _cbInformationLength); extern "C" ULONG __attribute__((__stdcall__)) __FALLBACK_PREFIXEventSetInformation( REGHANDLE _hRegHandle, EVENT_INFO_CLASS _eInformationClass, PVOID _pEventInformation, ULONG _cbInformationLength); static YY_THUNK_FNTYPE_EventSetInformation __attribute__((__cdecl__)) try_get_EventSetInformation() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_EventWriteEx
__APPLY_UNIT_TEST_BOOL(EventWriteEx); typedef ULONG(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_EventWriteEx)( REGHANDLE RegHandle, PCEVENT_DESCRIPTOR EventDescriptor, ULONG64 Filter, ULONG Flags, LPCGUID ActivityId, LPCGUID RelatedActivityId, ULONG UserDataCount, PEVENT_DATA_DESCRIPTOR UserData); extern "C" ULONG __attribute__((__stdcall__)) __FALLBACK_PREFIXEventWriteEx( REGHANDLE RegHandle, PCEVENT_DESCRIPTOR EventDescriptor, ULONG64 Filter, ULONG Flags, LPCGUID ActivityId, LPCGUID RelatedActivityId, ULONG UserDataCount, PEVENT_DATA_DESCRIPTOR UserData); static YY_THUNK_FNTYPE_EventWriteEx __attribute__((__cdecl__)) try_get_EventWriteEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PowerDeterminePlatformRoleEx
__APPLY_UNIT_TEST_BOOL(PowerDeterminePlatformRoleEx); typedef POWER_PLATFORM_ROLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PowerDeterminePlatformRoleEx)( ULONG _uVersion); extern "C" POWER_PLATFORM_ROLE __attribute__((__stdcall__)) __FALLBACK_PREFIXPowerDeterminePlatformRoleEx( ULONG _uVersion); static YY_THUNK_FNTYPE_PowerDeterminePlatformRoleEx __attribute__((__cdecl__)) try_get_PowerDeterminePlatformRoleEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PowerRegisterSuspendResumeNotification
__APPLY_UNIT_TEST_BOOL(PowerRegisterSuspendResumeNotification); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PowerRegisterSuspendResumeNotification)( DWORD _fFlags, HANDLE _hRecipient, PHPOWERNOTIFY _phRegistrationHandle); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXPowerRegisterSuspendResumeNotification( DWORD _fFlags, HANDLE _hRecipient, PHPOWERNOTIFY _phRegistrationHandle); static YY_THUNK_FNTYPE_PowerRegisterSuspendResumeNotification __attribute__((__cdecl__)) try_get_PowerRegisterSuspendResumeNotification() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PowerUnregisterSuspendResumeNotification
__APPLY_UNIT_TEST_BOOL(PowerUnregisterSuspendResumeNotification); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PowerUnregisterSuspendResumeNotification)( HPOWERNOTIFY _hRegistrationHandle); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXPowerUnregisterSuspendResumeNotification( HPOWERNOTIFY _hRegistrationHandle); static YY_THUNK_FNTYPE_PowerUnregisterSuspendResumeNotification __attribute__((__cdecl__)) try_get_PowerUnregisterSuspendResumeNotification() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BCryptDeriveKeyPBKDF2
__APPLY_UNIT_TEST_BOOL(BCryptDeriveKeyPBKDF2); typedef NTSTATUS(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BCryptDeriveKeyPBKDF2)( BCRYPT_ALG_HANDLE hPrf, PUCHAR pbPassword, ULONG cbPassword, PUCHAR pbSalt, ULONG cbSalt, ULONGLONG cIterations, PUCHAR pbDerivedKey, ULONG cbDerivedKey, ULONG dwFlags); extern "C" NTSTATUS __attribute__((__stdcall__)) __FALLBACK_PREFIXBCryptDeriveKeyPBKDF2( BCRYPT_ALG_HANDLE hPrf, PUCHAR pbPassword, ULONG cbPassword, PUCHAR pbSalt, ULONG cbSalt, ULONGLONG cIterations, PUCHAR pbDerivedKey, ULONG cbDerivedKey, ULONG dwFlags); static YY_THUNK_FNTYPE_BCryptDeriveKeyPBKDF2 __attribute__((__cdecl__)) try_get_BCryptDeriveKeyPBKDF2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BCryptDeriveKeyCapi
__APPLY_UNIT_TEST_BOOL(BCryptDeriveKeyCapi); typedef NTSTATUS(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BCryptDeriveKeyCapi)( BCRYPT_HASH_HANDLE hHash, BCRYPT_ALG_HANDLE hTargetAlg, PUCHAR pbDerivedKey, ULONG cbDerivedKey, ULONG dwFlags); extern "C" NTSTATUS __attribute__((__stdcall__)) __FALLBACK_PREFIXBCryptDeriveKeyCapi( BCRYPT_HASH_HANDLE hHash, BCRYPT_ALG_HANDLE hTargetAlg, PUCHAR pbDerivedKey, ULONG cbDerivedKey, ULONG dwFlags); static YY_THUNK_FNTYPE_BCryptDeriveKeyCapi __attribute__((__cdecl__)) try_get_BCryptDeriveKeyCapi() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ProcessPrng
__APPLY_UNIT_TEST_BOOL(ProcessPrng); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_ProcessPrng)( PUCHAR _pbBuffer, ULONG _cbBuffer); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXProcessPrng( PUCHAR _pbBuffer, ULONG _cbBuffer); static YY_THUNK_FNTYPE_ProcessPrng __attribute__((__cdecl__)) try_get_ProcessPrng() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTGetCharacteristicValue
__APPLY_UNIT_TEST_BOOL(BluetoothGATTGetCharacteristicValue); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTGetCharacteristicValue)( HANDLE _hDevice, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic, ULONG _cbCharacteristicValueDataSize, PBTH_LE_GATT_CHARACTERISTIC_VALUE _pCharacteristicValue, USHORT* _pCharacteristicValueSizeRequired, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTGetCharacteristicValue( HANDLE _hDevice, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic, ULONG _cbCharacteristicValueDataSize, PBTH_LE_GATT_CHARACTERISTIC_VALUE _pCharacteristicValue, USHORT* _pCharacteristicValueSizeRequired, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTGetCharacteristicValue __attribute__((__cdecl__)) try_get_BluetoothGATTGetCharacteristicValue() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTGetCharacteristics
__APPLY_UNIT_TEST_BOOL(BluetoothGATTGetCharacteristics); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTGetCharacteristics)( HANDLE _hDevice, PBTH_LE_GATT_SERVICE _pService, USHORT _uCharacteristicsBufferCount, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristicsBuffer, USHORT* _puCharacteristicsBufferActual, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTGetCharacteristics( HANDLE _hDevice, PBTH_LE_GATT_SERVICE _pService, USHORT _uCharacteristicsBufferCount, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristicsBuffer, USHORT* _puCharacteristicsBufferActual, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTGetCharacteristics __attribute__((__cdecl__)) try_get_BluetoothGATTGetCharacteristics() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTGetDescriptors
__APPLY_UNIT_TEST_BOOL(BluetoothGATTGetDescriptors); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTGetDescriptors)( HANDLE _hDevice, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic, USHORT _uDescriptorsBufferCount, PBTH_LE_GATT_DESCRIPTOR _pDescriptorsBuffer, USHORT* _puDescriptorsBufferActual, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTGetDescriptors( HANDLE _hDevice, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic, USHORT _uDescriptorsBufferCount, PBTH_LE_GATT_DESCRIPTOR _pDescriptorsBuffer, USHORT* _puDescriptorsBufferActual, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTGetDescriptors __attribute__((__cdecl__)) try_get_BluetoothGATTGetDescriptors() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTGetServices
__APPLY_UNIT_TEST_BOOL(BluetoothGATTGetServices); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTGetServices)( HANDLE _hDevice, USHORT _uServicesBufferCount, PBTH_LE_GATT_SERVICE _pServicesBuffer, USHORT* _puServicesBufferActual, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTGetServices( HANDLE _hDevice, USHORT _uServicesBufferCount, PBTH_LE_GATT_SERVICE _pServicesBuffer, USHORT* _puServicesBufferActual, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTGetServices __attribute__((__cdecl__)) try_get_BluetoothGATTGetServices() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTRegisterEvent
__APPLY_UNIT_TEST_BOOL(BluetoothGATTRegisterEvent); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTRegisterEvent)( HANDLE _hService, BTH_LE_GATT_EVENT_TYPE _eEventType, PVOID _pEventParameterIn, PFNBLUETOOTH_GATT_EVENT_CALLBACK _pfnCallback, PVOID _pCallbackContext, BLUETOOTH_GATT_EVENT_HANDLE * _pEventHandle, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTRegisterEvent( HANDLE _hService, BTH_LE_GATT_EVENT_TYPE _eEventType, PVOID _pEventParameterIn, PFNBLUETOOTH_GATT_EVENT_CALLBACK _pfnCallback, PVOID _pCallbackContext, BLUETOOTH_GATT_EVENT_HANDLE * _pEventHandle, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTRegisterEvent __attribute__((__cdecl__)) try_get_BluetoothGATTRegisterEvent() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTSetCharacteristicValue
__APPLY_UNIT_TEST_BOOL(BluetoothGATTSetCharacteristicValue); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTSetCharacteristicValue)( HANDLE _hDevice, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic, PBTH_LE_GATT_CHARACTERISTIC_VALUE _pCharacteristicValue, BTH_LE_GATT_RELIABLE_WRITE_CONTEXT _ReliableWriteContext, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTSetCharacteristicValue( HANDLE _hDevice, PBTH_LE_GATT_CHARACTERISTIC _pCharacteristic, PBTH_LE_GATT_CHARACTERISTIC_VALUE _pCharacteristicValue, BTH_LE_GATT_RELIABLE_WRITE_CONTEXT _ReliableWriteContext, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTSetCharacteristicValue __attribute__((__cdecl__)) try_get_BluetoothGATTSetCharacteristicValue() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTSetDescriptorValue
__APPLY_UNIT_TEST_BOOL(BluetoothGATTSetDescriptorValue); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTSetDescriptorValue)( HANDLE _hDevice, PBTH_LE_GATT_DESCRIPTOR _pDescriptor, PBTH_LE_GATT_DESCRIPTOR_VALUE _pDescriptorValue, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTSetDescriptorValue( HANDLE _hDevice, PBTH_LE_GATT_DESCRIPTOR _pDescriptor, PBTH_LE_GATT_DESCRIPTOR_VALUE _pDescriptorValue, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTSetDescriptorValue __attribute__((__cdecl__)) try_get_BluetoothGATTSetDescriptorValue() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_BluetoothGATTUnregisterEvent
__APPLY_UNIT_TEST_BOOL(BluetoothGATTUnregisterEvent); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_BluetoothGATTUnregisterEvent)( BLUETOOTH_GATT_EVENT_HANDLE _hEventHandle, ULONG _fFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXBluetoothGATTUnregisterEvent( BLUETOOTH_GATT_EVENT_HANDLE _hEventHandle, ULONG _fFlags); static YY_THUNK_FNTYPE_BluetoothGATTUnregisterEvent __attribute__((__cdecl__)) try_get_BluetoothGATTUnregisterEvent() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CM_Get_DevNode_Property_ExW
__APPLY_UNIT_TEST_BOOL(CM_Get_DevNode_Property_ExW); typedef CONFIGRET(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CM_Get_DevNode_Property_ExW)( DEVINST dnDevInst, const DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, PBYTE PropertyBuffer, PULONG PropertyBufferSize, ULONG ulFlags, HMACHINE hMachine); extern "C" CONFIGRET __attribute__((__stdcall__)) __FALLBACK_PREFIXCM_Get_DevNode_Property_ExW( DEVINST dnDevInst, const DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, PBYTE PropertyBuffer, PULONG PropertyBufferSize, ULONG ulFlags, HMACHINE hMachine); static YY_THUNK_FNTYPE_CM_Get_DevNode_Property_ExW __attribute__((__cdecl__)) try_get_CM_Get_DevNode_Property_ExW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CM_Set_DevNode_Property_ExW
__APPLY_UNIT_TEST_BOOL(CM_Set_DevNode_Property_ExW); typedef CONFIGRET(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CM_Set_DevNode_Property_ExW)( DEVINST dnDevInst, const DEVPROPKEY* PropertyKey, DEVPROPTYPE PropertyType, PBYTE PropertyBuffer, ULONG PropertyBufferSize, ULONG ulFlags, HMACHINE hMachine); extern "C" CONFIGRET __attribute__((__stdcall__)) __FALLBACK_PREFIXCM_Set_DevNode_Property_ExW( DEVINST dnDevInst, const DEVPROPKEY* PropertyKey, DEVPROPTYPE PropertyType, PBYTE PropertyBuffer, ULONG PropertyBufferSize, ULONG ulFlags, HMACHINE hMachine); static YY_THUNK_FNTYPE_CM_Set_DevNode_Property_ExW __attribute__((__cdecl__)) try_get_CM_Set_DevNode_Property_ExW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CM_Get_DevNode_PropertyW
__APPLY_UNIT_TEST_BOOL(CM_Get_DevNode_PropertyW); typedef CONFIGRET(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CM_Get_DevNode_PropertyW)( DEVINST dnDevInst, const DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, PBYTE PropertyBuffer, PULONG PropertyBufferSize, ULONG ulFlags); extern "C" CONFIGRET __attribute__((__stdcall__)) __FALLBACK_PREFIXCM_Get_DevNode_PropertyW( DEVINST dnDevInst, const DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, PBYTE PropertyBuffer, PULONG PropertyBufferSize, ULONG ulFlags); static YY_THUNK_FNTYPE_CM_Get_DevNode_PropertyW __attribute__((__cdecl__)) try_get_CM_Get_DevNode_PropertyW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CM_Set_DevNode_PropertyW
__APPLY_UNIT_TEST_BOOL(CM_Set_DevNode_PropertyW); typedef CONFIGRET(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CM_Set_DevNode_PropertyW)( DEVINST dnDevInst, const DEVPROPKEY *PropertyKey, DEVPROPTYPE PropertyType, PBYTE PropertyBuffer, ULONG PropertyBufferSize, ULONG ulFlags); extern "C" CONFIGRET __attribute__((__stdcall__)) __FALLBACK_PREFIXCM_Set_DevNode_PropertyW( DEVINST dnDevInst, const DEVPROPKEY *PropertyKey, DEVPROPTYPE PropertyType, PBYTE PropertyBuffer, ULONG PropertyBufferSize, ULONG ulFlags); static YY_THUNK_FNTYPE_CM_Set_DevNode_PropertyW __attribute__((__cdecl__)) try_get_CM_Set_DevNode_PropertyW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_D3D12CreateDevice
__APPLY_UNIT_TEST_BOOL(D3D12CreateDevice); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_D3D12CreateDevice)( IUnknown* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, const IID & riid, void** ppDevice); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXD3D12CreateDevice( IUnknown* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, const IID & riid, void** ppDevice); static YY_THUNK_FNTYPE_D3D12CreateDevice __attribute__((__cdecl__)) try_get_D3D12CreateDevice() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_D3D12GetDebugInterface
__APPLY_UNIT_TEST_BOOL(D3D12GetDebugInterface); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_D3D12GetDebugInterface)( const IID & _oId, void** _ppvDebug); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXD3D12GetDebugInterface( const IID & _oId, void** _ppvDebug); static YY_THUNK_FNTYPE_D3D12GetDebugInterface __attribute__((__cdecl__)) try_get_D3D12GetDebugInterface() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_D3D12SerializeVersionedRootSignature
__APPLY_UNIT_TEST_BOOL(D3D12SerializeVersionedRootSignature); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_D3D12SerializeVersionedRootSignature)( const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* _pRootSignature, ID3DBlob** _ppBlob, ID3DBlob** _ppErrorBlob); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXD3D12SerializeVersionedRootSignature( const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* _pRootSignature, ID3DBlob** _ppBlob, ID3DBlob** _ppErrorBlob); static YY_THUNK_FNTYPE_D3D12SerializeVersionedRootSignature __attribute__((__cdecl__)) try_get_D3D12SerializeVersionedRootSignature() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DCompositionCreateDevice
__APPLY_UNIT_TEST_BOOL(DCompositionCreateDevice); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DCompositionCreateDevice)( IDXGIDevice* _pDxgiDevice, const IID & iid, void** _pDcompositionDevice); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXDCompositionCreateDevice( IDXGIDevice* _pDxgiDevice, const IID & iid, void** _pDcompositionDevice); static YY_THUNK_FNTYPE_DCompositionCreateDevice __attribute__((__cdecl__)) try_get_DCompositionCreateDevice() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DCompositionCreateDevice2
__APPLY_UNIT_TEST_BOOL(DCompositionCreateDevice2); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DCompositionCreateDevice2)( IUnknown* _pRenderingDevice, const IID & iid, void** _pDcompositionDevice); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXDCompositionCreateDevice2( IUnknown* _pRenderingDevice, const IID & iid, void** _pDcompositionDevice); static YY_THUNK_FNTYPE_DCompositionCreateDevice2 __attribute__((__cdecl__)) try_get_DCompositionCreateDevice2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DCompositionCreateDevice3
__APPLY_UNIT_TEST_BOOL(DCompositionCreateDevice3); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DCompositionCreateDevice3)( IUnknown* _pRenderingDevice, const IID & iid, void** _pDcompositionDevice); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXDCompositionCreateDevice3( IUnknown* _pRenderingDevice, const IID & iid, void** _pDcompositionDevice); static YY_THUNK_FNTYPE_DCompositionCreateDevice3 __attribute__((__cdecl__)) try_get_DCompositionCreateDevice3() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateDXGIFactory
__APPLY_UNIT_TEST_BOOL(CreateDXGIFactory); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateDXGIFactory)(const IID & _IID, void** _ppFactory); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateDXGIFactory(const IID & _IID, void** _ppFactory); static YY_THUNK_FNTYPE_CreateDXGIFactory __attribute__((__cdecl__)) try_get_CreateDXGIFactory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateDXGIFactory1
__APPLY_UNIT_TEST_BOOL(CreateDXGIFactory1); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateDXGIFactory1)(const IID & _IID, void ** _ppFactory); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateDXGIFactory1(const IID & _IID, void ** _ppFactory); static YY_THUNK_FNTYPE_CreateDXGIFactory1 __attribute__((__cdecl__)) try_get_CreateDXGIFactory1() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateDXGIFactory2
__APPLY_UNIT_TEST_BOOL(CreateDXGIFactory2); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateDXGIFactory2)(UINT _fFlags, const IID & _IID, void** _ppFactory); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateDXGIFactory2(UINT _fFlags, const IID & _IID, void** _ppFactory); static YY_THUNK_FNTYPE_CreateDXGIFactory2 __attribute__((__cdecl__)) try_get_CreateDXGIFactory2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ChangeWindowMessageFilterEx
__APPLY_UNIT_TEST_BOOL(ChangeWindowMessageFilterEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_ChangeWindowMessageFilterEx)( HWND _hWnd, UINT _uMessage, DWORD _uAction, PCHANGEFILTERSTRUCT _pChangeFilterStruct); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXChangeWindowMessageFilterEx( HWND _hWnd, UINT _uMessage, DWORD _uAction, PCHANGEFILTERSTRUCT _pChangeFilterStruct); static YY_THUNK_FNTYPE_ChangeWindowMessageFilterEx __attribute__((__cdecl__)) try_get_ChangeWindowMessageFilterEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RegisterSuspendResumeNotification
__APPLY_UNIT_TEST_BOOL(RegisterSuspendResumeNotification); typedef HPOWERNOTIFY(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RegisterSuspendResumeNotification)( HANDLE _hRecipient, DWORD _fFlags); extern "C" HPOWERNOTIFY __attribute__((__stdcall__)) __FALLBACK_PREFIXRegisterSuspendResumeNotification( HANDLE _hRecipient, DWORD _fFlags); static YY_THUNK_FNTYPE_RegisterSuspendResumeNotification __attribute__((__cdecl__)) try_get_RegisterSuspendResumeNotification() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_UnregisterSuspendResumeNotification
__APPLY_UNIT_TEST_BOOL(UnregisterSuspendResumeNotification); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_UnregisterSuspendResumeNotification)( HPOWERNOTIFY _hHandle); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXUnregisterSuspendResumeNotification( HPOWERNOTIFY _hHandle); static YY_THUNK_FNTYPE_UnregisterSuspendResumeNotification __attribute__((__cdecl__)) try_get_UnregisterSuspendResumeNotification() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetCoalescableTimer
__APPLY_UNIT_TEST_BOOL(SetCoalescableTimer); typedef UINT_PTR(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetCoalescableTimer)( HWND _hWnd, UINT_PTR _nIDEvent, UINT _uElapse, TIMERPROC _lpTimerFunc, ULONG _uToleranceDelay); extern "C" UINT_PTR __attribute__((__stdcall__)) __FALLBACK_PREFIXSetCoalescableTimer( HWND _hWnd, UINT_PTR _nIDEvent, UINT _uElapse, TIMERPROC _lpTimerFunc, ULONG _uToleranceDelay); static YY_THUNK_FNTYPE_SetCoalescableTimer __attribute__((__cdecl__)) try_get_SetCoalescableTimer() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PhysicalToLogicalPointForPerMonitorDPI
__APPLY_UNIT_TEST_BOOL(PhysicalToLogicalPointForPerMonitorDPI); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PhysicalToLogicalPointForPerMonitorDPI)( HWND _hWnd, LPPOINT _pPoint); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXPhysicalToLogicalPointForPerMonitorDPI( HWND _hWnd, LPPOINT _pPoint); static YY_THUNK_FNTYPE_PhysicalToLogicalPointForPerMonitorDPI __attribute__((__cdecl__)) try_get_PhysicalToLogicalPointForPerMonitorDPI() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_LogicalToPhysicalPointForPerMonitorDPI
__APPLY_UNIT_TEST_BOOL(LogicalToPhysicalPointForPerMonitorDPI); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_LogicalToPhysicalPointForPerMonitorDPI)( HWND _hWnd, LPPOINT _pPoint); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXLogicalToPhysicalPointForPerMonitorDPI( HWND _hWnd, LPPOINT _pPoint); static YY_THUNK_FNTYPE_LogicalToPhysicalPointForPerMonitorDPI __attribute__((__cdecl__)) try_get_LogicalToPhysicalPointForPerMonitorDPI() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WindowFromPhysicalPoint
__APPLY_UNIT_TEST_BOOL(WindowFromPhysicalPoint); typedef HWND(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WindowFromPhysicalPoint)( POINT _oPoint); extern "C" HWND __attribute__((__stdcall__)) __FALLBACK_PREFIXWindowFromPhysicalPoint( POINT _oPoint); static YY_THUNK_FNTYPE_WindowFromPhysicalPoint __attribute__((__cdecl__)) try_get_WindowFromPhysicalPoint() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RegisterPointerDeviceNotifications
__APPLY_UNIT_TEST_BOOL(RegisterPointerDeviceNotifications); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RegisterPointerDeviceNotifications)( HWND _hWindow, BOOL _bNotifyRange); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXRegisterPointerDeviceNotifications( HWND _hWindow, BOOL _bNotifyRange); static YY_THUNK_FNTYPE_RegisterPointerDeviceNotifications __attribute__((__cdecl__)) try_get_RegisterPointerDeviceNotifications() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerDevices
__APPLY_UNIT_TEST_BOOL(GetPointerDevices); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerDevices)( UINT32* _uDeviceCount, POINTER_DEVICE_INFO* _pPointerDevices); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerDevices( UINT32* _uDeviceCount, POINTER_DEVICE_INFO* _pPointerDevices); static YY_THUNK_FNTYPE_GetPointerDevices __attribute__((__cdecl__)) try_get_GetPointerDevices() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerDevice
__APPLY_UNIT_TEST_BOOL(GetPointerDevice); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerDevice)( HANDLE _hDevice, POINTER_DEVICE_INFO* _pPointerDevice); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerDevice( HANDLE _hDevice, POINTER_DEVICE_INFO* _pPointerDevice); static YY_THUNK_FNTYPE_GetPointerDevice __attribute__((__cdecl__)) try_get_GetPointerDevice() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerPenInfo
__APPLY_UNIT_TEST_BOOL(GetPointerPenInfo); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerPenInfo)( UINT32 _uPointerId, POINTER_PEN_INFO* _pPenInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerPenInfo( UINT32 _uPointerId, POINTER_PEN_INFO* _pPenInfo); static YY_THUNK_FNTYPE_GetPointerPenInfo __attribute__((__cdecl__)) try_get_GetPointerPenInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerType
__APPLY_UNIT_TEST_BOOL(GetPointerType); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerType)( UINT32 _uPointerId, POINTER_INPUT_TYPE* _pPointerType); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerType( UINT32 _uPointerId, POINTER_INPUT_TYPE* _pPointerType); static YY_THUNK_FNTYPE_GetPointerType __attribute__((__cdecl__)) try_get_GetPointerType() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_InitializeTouchInjection
__APPLY_UNIT_TEST_BOOL(InitializeTouchInjection); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_InitializeTouchInjection)( UINT32 _uMaxCount, DWORD _udwMode); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXInitializeTouchInjection( UINT32 _uMaxCount, DWORD _udwMode); static YY_THUNK_FNTYPE_InitializeTouchInjection __attribute__((__cdecl__)) try_get_InitializeTouchInjection() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_InjectTouchInput
__APPLY_UNIT_TEST_BOOL(InjectTouchInput); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_InjectTouchInput)( UINT32 _cCount, const POINTER_TOUCH_INFO* _pContacts); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXInjectTouchInput( UINT32 _cCount, const POINTER_TOUCH_INFO* _pContacts); static YY_THUNK_FNTYPE_InjectTouchInput __attribute__((__cdecl__)) try_get_InjectTouchInput() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerFrameTouchInfo
__APPLY_UNIT_TEST_BOOL(GetPointerFrameTouchInfo); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerFrameTouchInfo)( UINT32 _uPointerId, UINT32* _pcPointerCount, POINTER_TOUCH_INFO* _pTouchInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerFrameTouchInfo( UINT32 _uPointerId, UINT32* _pcPointerCount, POINTER_TOUCH_INFO* _pTouchInfo); static YY_THUNK_FNTYPE_GetPointerFrameTouchInfo __attribute__((__cdecl__)) try_get_GetPointerFrameTouchInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerFrameTouchInfoHistory
__APPLY_UNIT_TEST_BOOL(GetPointerFrameTouchInfoHistory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerFrameTouchInfoHistory)( UINT32 _uPointerId, UINT32* _pcEntriesCount, UINT32* _pcPointerCount, POINTER_TOUCH_INFO* _pTouchInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerFrameTouchInfoHistory( UINT32 _uPointerId, UINT32* _pcEntriesCount, UINT32* _pcPointerCount, POINTER_TOUCH_INFO* _pTouchInfo); static YY_THUNK_FNTYPE_GetPointerFrameTouchInfoHistory __attribute__((__cdecl__)) try_get_GetPointerFrameTouchInfoHistory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerInfo
__APPLY_UNIT_TEST_BOOL(GetPointerInfo); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerInfo)( UINT32 _uPointerId, POINTER_INFO* _pPointerInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerInfo( UINT32 _uPointerId, POINTER_INFO* _pPointerInfo); static YY_THUNK_FNTYPE_GetPointerInfo __attribute__((__cdecl__)) try_get_GetPointerInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerPenInfoHistory
__APPLY_UNIT_TEST_BOOL(GetPointerPenInfoHistory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerPenInfoHistory)( UINT32 _uPointerId, UINT32* _pcEntriesCount, POINTER_PEN_INFO* _pPenInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerPenInfoHistory( UINT32 _uPointerId, UINT32* _pcEntriesCount, POINTER_PEN_INFO* _pPenInfo); static YY_THUNK_FNTYPE_GetPointerPenInfoHistory __attribute__((__cdecl__)) try_get_GetPointerPenInfoHistory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SkipPointerFrameMessages
__APPLY_UNIT_TEST_BOOL(SkipPointerFrameMessages); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SkipPointerFrameMessages)( UINT32 _uPointerId); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSkipPointerFrameMessages( UINT32 _uPointerId); static YY_THUNK_FNTYPE_SkipPointerFrameMessages __attribute__((__cdecl__)) try_get_SkipPointerFrameMessages() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerInfoHistory
__APPLY_UNIT_TEST_BOOL(GetPointerInfoHistory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerInfoHistory)( UINT32 _uPointerId, UINT32* _pcEntriesCount, POINTER_INFO* _pPointerInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerInfoHistory( UINT32 _uPointerId, UINT32* _pcEntriesCount, POINTER_INFO* _pPointerInfo); static YY_THUNK_FNTYPE_GetPointerInfoHistory __attribute__((__cdecl__)) try_get_GetPointerInfoHistory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerTouchInfo
__APPLY_UNIT_TEST_BOOL(GetPointerTouchInfo); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerTouchInfo)( UINT32 _uPointerId, POINTER_TOUCH_INFO* _pTouchInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerTouchInfo( UINT32 _uPointerId, POINTER_TOUCH_INFO* _pTouchInfo); static YY_THUNK_FNTYPE_GetPointerTouchInfo __attribute__((__cdecl__)) try_get_GetPointerTouchInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerTouchInfoHistory
__APPLY_UNIT_TEST_BOOL(GetPointerTouchInfoHistory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerTouchInfoHistory)( UINT32 _uPointerId, UINT32* _pcEntriesCount, POINTER_TOUCH_INFO* _pTouchInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerTouchInfoHistory( UINT32 _uPointerId, UINT32* _pcEntriesCount, POINTER_TOUCH_INFO* _pTouchInfo); static YY_THUNK_FNTYPE_GetPointerTouchInfoHistory __attribute__((__cdecl__)) try_get_GetPointerTouchInfoHistory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_IsMouseInPointerEnabled
__APPLY_UNIT_TEST_BOOL(IsMouseInPointerEnabled); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_IsMouseInPointerEnabled)(); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXIsMouseInPointerEnabled(); static YY_THUNK_FNTYPE_IsMouseInPointerEnabled __attribute__((__cdecl__)) try_get_IsMouseInPointerEnabled() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_EnableMouseInPointer
__APPLY_UNIT_TEST_BOOL(EnableMouseInPointer); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_EnableMouseInPointer)( BOOL _bEnable); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXEnableMouseInPointer( BOOL _bEnable); static YY_THUNK_FNTYPE_EnableMouseInPointer __attribute__((__cdecl__)) try_get_EnableMouseInPointer() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPointerDeviceRects
__APPLY_UNIT_TEST_BOOL(GetPointerDeviceRects); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPointerDeviceRects)( HANDLE _hDevice, RECT* _pPointerDeviceRect, RECT* _pDisplayRect); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPointerDeviceRects( HANDLE _hDevice, RECT* _pPointerDeviceRect, RECT* _pDisplayRect); static YY_THUNK_FNTYPE_GetPointerDeviceRects __attribute__((__cdecl__)) try_get_GetPointerDeviceRects() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetIfEntry2Ex
__APPLY_UNIT_TEST_BOOL(GetIfEntry2Ex); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetIfEntry2Ex)( MIB_IF_ENTRY_LEVEL _eLevel, PMIB_IF_ROW2 _pRow); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetIfEntry2Ex( MIB_IF_ENTRY_LEVEL _eLevel, PMIB_IF_ROW2 _pRow); static YY_THUNK_FNTYPE_GetIfEntry2Ex __attribute__((__cdecl__)) try_get_GetIfEntry2Ex() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFCreateDeviceSource
__APPLY_UNIT_TEST_BOOL(MFCreateDeviceSource); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFCreateDeviceSource)( IMFAttributes* pAttributes, IMFMediaSource** ppSource); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFCreateDeviceSource( IMFAttributes* pAttributes, IMFMediaSource** ppSource); static YY_THUNK_FNTYPE_MFCreateDeviceSource __attribute__((__cdecl__)) try_get_MFCreateDeviceSource() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFEnumDeviceSources
__APPLY_UNIT_TEST_BOOL(MFEnumDeviceSources); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFEnumDeviceSources)( IMFAttributes* pAttributes, IMFActivate*** pppSourceActivate, UINT32* pcSourceActivate); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFEnumDeviceSources( IMFAttributes* pAttributes, IMFActivate*** pppSourceActivate, UINT32* pcSourceActivate); static YY_THUNK_FNTYPE_MFEnumDeviceSources __attribute__((__cdecl__)) try_get_MFEnumDeviceSources() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFCreateDXGIDeviceManager
__APPLY_UNIT_TEST_BOOL(MFCreateDXGIDeviceManager); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFCreateDXGIDeviceManager)( UINT* _puResetToken, IMFDXGIDeviceManager** _ppDeviceManager); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFCreateDXGIDeviceManager( UINT* _puResetToken, IMFDXGIDeviceManager** _ppDeviceManager); static YY_THUNK_FNTYPE_MFCreateDXGIDeviceManager __attribute__((__cdecl__)) try_get_MFCreateDXGIDeviceManager() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFCreateDXGISurfaceBuffer
__APPLY_UNIT_TEST_BOOL(MFCreateDXGISurfaceBuffer); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFCreateDXGISurfaceBuffer)( const IID & _id, IUnknown* _punkSurface, UINT _uSubresourceIndex, BOOL _bBottomUpWhenLinear, IMFMediaBuffer** _ppBuffer); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFCreateDXGISurfaceBuffer( const IID & _id, IUnknown* _punkSurface, UINT _uSubresourceIndex, BOOL _bBottomUpWhenLinear, IMFMediaBuffer** _ppBuffer); static YY_THUNK_FNTYPE_MFCreateDXGISurfaceBuffer __attribute__((__cdecl__)) try_get_MFCreateDXGISurfaceBuffer() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFLockDXGIDeviceManager
__APPLY_UNIT_TEST_BOOL(MFLockDXGIDeviceManager); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFLockDXGIDeviceManager)( UINT* _puResetToken, IMFDXGIDeviceManager** _ppManager); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFLockDXGIDeviceManager( UINT* _puResetToken, IMFDXGIDeviceManager** _ppManager); static YY_THUNK_FNTYPE_MFLockDXGIDeviceManager __attribute__((__cdecl__)) try_get_MFLockDXGIDeviceManager() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFUnlockDXGIDeviceManager
__APPLY_UNIT_TEST_BOOL(MFUnlockDXGIDeviceManager); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFUnlockDXGIDeviceManager)(); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFUnlockDXGIDeviceManager(); static YY_THUNK_FNTYPE_MFUnlockDXGIDeviceManager __attribute__((__cdecl__)) try_get_MFUnlockDXGIDeviceManager() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFTEnumEx
__APPLY_UNIT_TEST_BOOL(MFTEnumEx); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFTEnumEx)( GUID guidCategory, UINT32 Flags, const MFT_REGISTER_TYPE_INFO* pInputType, const MFT_REGISTER_TYPE_INFO* pOutputType, IMFActivate*** pppMFTActivate, UINT32* pnumMFTActivate); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFTEnumEx( GUID guidCategory, UINT32 Flags, const MFT_REGISTER_TYPE_INFO* pInputType, const MFT_REGISTER_TYPE_INFO* pOutputType, IMFActivate*** pppMFTActivate, UINT32* pnumMFTActivate); static YY_THUNK_FNTYPE_MFTEnumEx __attribute__((__cdecl__)) try_get_MFTEnumEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFLockSharedWorkQueue
__APPLY_UNIT_TEST_BOOL(MFLockSharedWorkQueue); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFLockSharedWorkQueue)( PCWSTR _szClass, LONG _nBasePriority, DWORD* _puTaskId, DWORD* _puId); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFLockSharedWorkQueue( PCWSTR _szClass, LONG _nBasePriority, DWORD* _puTaskId, DWORD* _puId); static YY_THUNK_FNTYPE_MFLockSharedWorkQueue __attribute__((__cdecl__)) try_get_MFLockSharedWorkQueue() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFPutWorkItem
__APPLY_UNIT_TEST_BOOL(MFPutWorkItem); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFPutWorkItem)(DWORD _uQueue, IMFAsyncCallback* _pCallback, IUnknown* _pState); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFPutWorkItem(DWORD _uQueue, IMFAsyncCallback* _pCallback, IUnknown* _pState); static YY_THUNK_FNTYPE_MFPutWorkItem __attribute__((__cdecl__)) try_get_MFPutWorkItem() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFPutWorkItem2
__APPLY_UNIT_TEST_BOOL(MFPutWorkItem2); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFPutWorkItem2)(DWORD _uQueue, LONG _nPriority, IMFAsyncCallback* _pCallback, IUnknown* _pState); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFPutWorkItem2(DWORD _uQueue, LONG _nPriority, IMFAsyncCallback* _pCallback, IUnknown* _pState); static YY_THUNK_FNTYPE_MFPutWorkItem2 __attribute__((__cdecl__)) try_get_MFPutWorkItem2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFPutWaitingWorkItem
__APPLY_UNIT_TEST_BOOL(MFPutWaitingWorkItem); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFPutWaitingWorkItem)(HANDLE _hEvent, LONG _nPriority, IMFAsyncResult* _pResult, MFWORKITEM_KEY* _pKey); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFPutWaitingWorkItem(HANDLE _hEvent, LONG _nPriority, IMFAsyncResult* _pResult, MFWORKITEM_KEY* _pKey); static YY_THUNK_FNTYPE_MFPutWaitingWorkItem __attribute__((__cdecl__)) try_get_MFPutWaitingWorkItem() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFUnlockWorkQueue
__APPLY_UNIT_TEST_BOOL(MFUnlockWorkQueue); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFUnlockWorkQueue)( DWORD _uWorkQueue); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFUnlockWorkQueue( DWORD _uWorkQueue); static YY_THUNK_FNTYPE_MFUnlockWorkQueue __attribute__((__cdecl__)) try_get_MFUnlockWorkQueue() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MFCreateSourceReaderFromMediaSource
__APPLY_UNIT_TEST_BOOL(MFCreateSourceReaderFromMediaSource); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MFCreateSourceReaderFromMediaSource)( IMFMediaSource* pMediaSource, IMFAttributes* pAttributes, IMFSourceReader** ppSourceReader); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXMFCreateSourceReaderFromMediaSource( IMFMediaSource* pMediaSource, IMFAttributes* pAttributes, IMFSourceReader** ppSourceReader); static YY_THUNK_FNTYPE_MFCreateSourceReaderFromMediaSource __attribute__((__cdecl__)) try_get_MFCreateSourceReaderFromMediaSource() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ActivateAudioInterfaceAsync
__APPLY_UNIT_TEST_BOOL(ActivateAudioInterfaceAsync); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_ActivateAudioInterfaceAsync)( LPCWSTR _szDeviceInterfacePath, const IID & riid, PROPVARIANT* _pActivationParams, IActivateAudioInterfaceCompletionHandler* _pCompletionHandler, IActivateAudioInterfaceAsyncOperation** _ppActivationOperation); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXActivateAudioInterfaceAsync( LPCWSTR _szDeviceInterfacePath, const IID & riid, PROPVARIANT* _pActivationParams, IActivateAudioInterfaceCompletionHandler* _pCompletionHandler, IActivateAudioInterfaceAsyncOperation** _ppActivationOperation); static YY_THUNK_FNTYPE_ActivateAudioInterfaceAsync __attribute__((__cdecl__)) try_get_ActivateAudioInterfaceAsync() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_NtOpenKeyEx
__APPLY_UNIT_TEST_BOOL(NtOpenKeyEx); typedef NTSTATUS(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_NtOpenKeyEx)( PHANDLE _phKeyHandle, ACCESS_MASK _fDesiredAccess, POBJECT_ATTRIBUTES _pObjectAttributes, ULONG _fOpenOptions); extern "C" NTSTATUS __attribute__((__stdcall__)) __FALLBACK_PREFIXNtOpenKeyEx( PHANDLE _phKeyHandle, ACCESS_MASK _fDesiredAccess, POBJECT_ATTRIBUTES _pObjectAttributes, ULONG _fOpenOptions); static YY_THUNK_FNTYPE_NtOpenKeyEx __attribute__((__cdecl__)) try_get_NtOpenKeyEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RtlAddGrowableFunctionTable
__APPLY_UNIT_TEST_BOOL(RtlAddGrowableFunctionTable); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RtlAddGrowableFunctionTable)( PVOID* _pDynamicTable, PRUNTIME_FUNCTION _pFunctionTable, DWORD _uEntryCount, DWORD _uMaximumEntryCount, ULONG_PTR _uRangeBase, ULONG_PTR _uRangeEnd); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXRtlAddGrowableFunctionTable( PVOID* _pDynamicTable, PRUNTIME_FUNCTION _pFunctionTable, DWORD _uEntryCount, DWORD _uMaximumEntryCount, ULONG_PTR _uRangeBase, ULONG_PTR _uRangeEnd); static YY_THUNK_FNTYPE_RtlAddGrowableFunctionTable __attribute__((__cdecl__)) try_get_RtlAddGrowableFunctionTable() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RtlDeleteGrowableFunctionTable
__APPLY_UNIT_TEST_BOOL(RtlDeleteGrowableFunctionTable); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RtlDeleteGrowableFunctionTable)( PVOID _pDynamicTable); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXRtlDeleteGrowableFunctionTable( PVOID _pDynamicTable); static YY_THUNK_FNTYPE_RtlDeleteGrowableFunctionTable __attribute__((__cdecl__)) try_get_RtlDeleteGrowableFunctionTable() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_UiaRaiseNotificationEvent
__APPLY_UNIT_TEST_BOOL(UiaRaiseNotificationEvent); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_UiaRaiseNotificationEvent)( IRawElementProviderSimple* _pProvider, enum NotificationKind _eNotificationKind, enum NotificationProcessing _eNotificationProcessing, BSTR _szDisplayString, BSTR _szActivityId); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXUiaRaiseNotificationEvent( IRawElementProviderSimple* _pProvider, enum NotificationKind _eNotificationKind, enum NotificationProcessing _eNotificationProcessing, BSTR _szDisplayString, BSTR _szActivityId); static YY_THUNK_FNTYPE_UiaRaiseNotificationEvent __attribute__((__cdecl__)) try_get_UiaRaiseNotificationEvent() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateAppContainerProfile
__APPLY_UNIT_TEST_BOOL(CreateAppContainerProfile); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateAppContainerProfile)( PCWSTR _szAppContainerName, PCWSTR _szDisplayName, PCWSTR _szDescription, PSID_AND_ATTRIBUTES _pCapabilities, DWORD _uCapabilityCount, PSID* _ppSidAppContainerSid); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateAppContainerProfile( PCWSTR _szAppContainerName, PCWSTR _szDisplayName, PCWSTR _szDescription, PSID_AND_ATTRIBUTES _pCapabilities, DWORD _uCapabilityCount, PSID* _ppSidAppContainerSid); static YY_THUNK_FNTYPE_CreateAppContainerProfile __attribute__((__cdecl__)) try_get_CreateAppContainerProfile() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DeleteAppContainerProfile
__APPLY_UNIT_TEST_BOOL(DeleteAppContainerProfile); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DeleteAppContainerProfile)( PCWSTR _szAppContainerName); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXDeleteAppContainerProfile( PCWSTR _szAppContainerName); static YY_THUNK_FNTYPE_DeleteAppContainerProfile __attribute__((__cdecl__)) try_get_DeleteAppContainerProfile() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DeriveAppContainerSidFromAppContainerName
__APPLY_UNIT_TEST_BOOL(DeriveAppContainerSidFromAppContainerName); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DeriveAppContainerSidFromAppContainerName)( PCWSTR _szAppContainerName, PSID* _ppsidAppContainerSid); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXDeriveAppContainerSidFromAppContainerName( PCWSTR _szAppContainerName, PSID* _ppsidAppContainerSid); static YY_THUNK_FNTYPE_DeriveAppContainerSidFromAppContainerName __attribute__((__cdecl__)) try_get_DeriveAppContainerSidFromAppContainerName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetAppContainerFolderPath
__APPLY_UNIT_TEST_BOOL(GetAppContainerFolderPath); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetAppContainerFolderPath)( PCWSTR _szAppContainerSid, PWSTR* _ppszPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetAppContainerFolderPath( PCWSTR _szAppContainerSid, PWSTR* _ppszPath); static YY_THUNK_FNTYPE_GetAppContainerFolderPath __attribute__((__cdecl__)) try_get_GetAppContainerFolderPath() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetAppContainerRegistryLocation
__APPLY_UNIT_TEST_BOOL(GetAppContainerRegistryLocation); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetAppContainerRegistryLocation)( REGSAM _DesiredAccess, PHKEY _phAppContainerKey); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetAppContainerRegistryLocation( REGSAM _DesiredAccess, PHKEY _phAppContainerKey); static YY_THUNK_FNTYPE_GetAppContainerRegistryLocation __attribute__((__cdecl__)) try_get_GetAppContainerRegistryLocation() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WinHttpCreateProxyResolver
__APPLY_UNIT_TEST_BOOL(WinHttpCreateProxyResolver); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WinHttpCreateProxyResolver)( HINTERNET _hSession, HINTERNET* _phResolver); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXWinHttpCreateProxyResolver( HINTERNET _hSession, HINTERNET* _phResolver); static YY_THUNK_FNTYPE_WinHttpCreateProxyResolver __attribute__((__cdecl__)) try_get_WinHttpCreateProxyResolver() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WinHttpCloseHandle
__APPLY_UNIT_TEST_BOOL(WinHttpCloseHandle); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WinHttpCloseHandle)( HINTERNET _hInternet); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXWinHttpCloseHandle( HINTERNET _hInternet); static YY_THUNK_FNTYPE_WinHttpCloseHandle __attribute__((__cdecl__)) try_get_WinHttpCloseHandle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WinHttpSetStatusCallback
__APPLY_UNIT_TEST_BOOL(WinHttpSetStatusCallback); typedef WINHTTP_STATUS_CALLBACK(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WinHttpSetStatusCallback)( HINTERNET _hInternet, WINHTTP_STATUS_CALLBACK _pfnInternetCallback, DWORD _fNotificationFlags, DWORD_PTR _uReserved); extern "C" WINHTTP_STATUS_CALLBACK __attribute__((__stdcall__)) __FALLBACK_PREFIXWinHttpSetStatusCallback( HINTERNET _hInternet, WINHTTP_STATUS_CALLBACK _pfnInternetCallback, DWORD _fNotificationFlags, DWORD_PTR _uReserved); static YY_THUNK_FNTYPE_WinHttpSetStatusCallback __attribute__((__cdecl__)) try_get_WinHttpSetStatusCallback() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WinHttpGetProxyForUrlEx
__APPLY_UNIT_TEST_BOOL(WinHttpGetProxyForUrlEx); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WinHttpGetProxyForUrlEx)( HINTERNET _hResolver, PCWSTR _szUrl, WINHTTP_AUTOPROXY_OPTIONS* _pAutoProxyOptions, DWORD_PTR _pContext); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXWinHttpGetProxyForUrlEx( HINTERNET _hResolver, PCWSTR _szUrl, WINHTTP_AUTOPROXY_OPTIONS* _pAutoProxyOptions, DWORD_PTR _pContext); static YY_THUNK_FNTYPE_WinHttpGetProxyForUrlEx __attribute__((__cdecl__)) try_get_WinHttpGetProxyForUrlEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WinHttpGetProxyResult
__APPLY_UNIT_TEST_BOOL(WinHttpGetProxyResult); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WinHttpGetProxyResult)( HINTERNET _hResolver, WINHTTP_PROXY_RESULT* _pProxyResult); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXWinHttpGetProxyResult( HINTERNET _hResolver, WINHTTP_PROXY_RESULT* _pProxyResult); static YY_THUNK_FNTYPE_WinHttpGetProxyResult __attribute__((__cdecl__)) try_get_WinHttpGetProxyResult() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WinHttpFreeProxyResult
__APPLY_UNIT_TEST_BOOL(WinHttpFreeProxyResult); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WinHttpFreeProxyResult)( WINHTTP_PROXY_RESULT* _pProxyResult); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXWinHttpFreeProxyResult( WINHTTP_PROXY_RESULT* _pProxyResult); static YY_THUNK_FNTYPE_WinHttpFreeProxyResult __attribute__((__cdecl__)) try_get_WinHttpFreeProxyResult() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RaiseFailFastException
__APPLY_UNIT_TEST_BOOL(RaiseFailFastException); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RaiseFailFastException)( PEXCEPTION_RECORD pExceptionRecord, PCONTEXT pContextRecord, DWORD dwFlags); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXRaiseFailFastException( PEXCEPTION_RECORD pExceptionRecord, PCONTEXT pContextRecord, DWORD dwFlags); static YY_THUNK_FNTYPE_RaiseFailFastException __attribute__((__cdecl__)) try_get_RaiseFailFastException() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetThreadErrorMode
__APPLY_UNIT_TEST_BOOL(SetThreadErrorMode); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetThreadErrorMode)( DWORD dwNewMode, LPDWORD lpOldMode); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetThreadErrorMode( DWORD dwNewMode, LPDWORD lpOldMode); static YY_THUNK_FNTYPE_SetThreadErrorMode __attribute__((__cdecl__)) try_get_SetThreadErrorMode() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetThreadErrorMode
__APPLY_UNIT_TEST_BOOL(GetThreadErrorMode); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetThreadErrorMode)(void); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetThreadErrorMode(void); static YY_THUNK_FNTYPE_GetThreadErrorMode __attribute__((__cdecl__)) try_get_GetThreadErrorMode() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetFileInformationByHandleEx
__APPLY_UNIT_TEST_BOOL(GetFileInformationByHandleEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetFileInformationByHandleEx)( HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetFileInformationByHandleEx( HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize); static YY_THUNK_FNTYPE_GetFileInformationByHandleEx __attribute__((__cdecl__)) try_get_GetFileInformationByHandleEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateFile2
__APPLY_UNIT_TEST_BOOL(CreateFile2); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateFile2)( LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateFile2( LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams); static YY_THUNK_FNTYPE_CreateFile2 __attribute__((__cdecl__)) try_get_CreateFile2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateFile3
__APPLY_UNIT_TEST_BOOL(CreateFile3); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateFile3)( LPCWSTR _szFileName, DWORD _uDesiredAccess, DWORD _uShareMode, DWORD _uCreationDisposition, LPCREATEFILE3_EXTENDED_PARAMETERS _pCreateExParams); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateFile3( LPCWSTR _szFileName, DWORD _uDesiredAccess, DWORD _uShareMode, DWORD _uCreationDisposition, LPCREATEFILE3_EXTENDED_PARAMETERS _pCreateExParams); static YY_THUNK_FNTYPE_CreateFile3 __attribute__((__cdecl__)) try_get_CreateFile3() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_OpenFileById
__APPLY_UNIT_TEST_BOOL(OpenFileById); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_OpenFileById)( HANDLE hVolumeHint, LPFILE_ID_DESCRIPTOR lpFileId, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwFlagsAndAttributes); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXOpenFileById( HANDLE hVolumeHint, LPFILE_ID_DESCRIPTOR lpFileId, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwFlagsAndAttributes); static YY_THUNK_FNTYPE_OpenFileById __attribute__((__cdecl__)) try_get_OpenFileById() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_FindFirstFileExW
__APPLY_UNIT_TEST_BOOL(FindFirstFileExW); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_FindFirstFileExW)( LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXFindFirstFileExW( LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags); static YY_THUNK_FNTYPE_FindFirstFileExW __attribute__((__cdecl__)) try_get_FindFirstFileExW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_FindFirstFileExA
__APPLY_UNIT_TEST_BOOL(FindFirstFileExA); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_FindFirstFileExA)( LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXFindFirstFileExA( LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags); static YY_THUNK_FNTYPE_FindFirstFileExA __attribute__((__cdecl__)) try_get_FindFirstFileExA() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WriteFile
__APPLY_UNIT_TEST_BOOL(WriteFile); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WriteFile)( HANDLE _hFile, LPCVOID _pBuffer, DWORD _uNumberOfBytesToWrite, LPDWORD _puNumberOfBytesWritten, LPOVERLAPPED _pOverlapped); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXWriteFile( HANDLE _hFile, LPCVOID _pBuffer, DWORD _uNumberOfBytesToWrite, LPDWORD _puNumberOfBytesWritten, LPOVERLAPPED _pOverlapped); static YY_THUNK_FNTYPE_WriteFile __attribute__((__cdecl__)) try_get_WriteFile() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ReadFile
__APPLY_UNIT_TEST_BOOL(ReadFile); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_ReadFile)( HANDLE _hFile, LPVOID _pBuffer, DWORD _uNumberOfBytesToRead, LPDWORD _puNumberOfBytesRead, LPOVERLAPPED _pOverlapped); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXReadFile( HANDLE _hFile, LPVOID _pBuffer, DWORD _uNumberOfBytesToRead, LPDWORD _puNumberOfBytesRead, LPOVERLAPPED _pOverlapped); static YY_THUNK_FNTYPE_ReadFile __attribute__((__cdecl__)) try_get_ReadFile() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CopyFile2
__APPLY_UNIT_TEST_BOOL(CopyFile2); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CopyFile2)( PCWSTR _szExistingFileName, PCWSTR _szNewFileName, COPYFILE2_EXTENDED_PARAMETERS* _pExtendedParameters); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCopyFile2( PCWSTR _szExistingFileName, PCWSTR _szNewFileName, COPYFILE2_EXTENDED_PARAMETERS* _pExtendedParameters); static YY_THUNK_FNTYPE_CopyFile2 __attribute__((__cdecl__)) try_get_CopyFile2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetTempPath2W
__APPLY_UNIT_TEST_BOOL(GetTempPath2W); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetTempPath2W)( DWORD BufferLength, LPWSTR Buffer); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetTempPath2W( DWORD BufferLength, LPWSTR Buffer); static YY_THUNK_FNTYPE_GetTempPath2W __attribute__((__cdecl__)) try_get_GetTempPath2W() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetTempPath2A
__APPLY_UNIT_TEST_BOOL(GetTempPath2A); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetTempPath2A)( DWORD BufferLength, LPSTR Buffer); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetTempPath2A( DWORD BufferLength, LPSTR Buffer); static YY_THUNK_FNTYPE_GetTempPath2A __attribute__((__cdecl__)) try_get_GetTempPath2A() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetOverlappedResultEx
__APPLY_UNIT_TEST_BOOL(GetOverlappedResultEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetOverlappedResultEx)( HANDLE _hFile, LPOVERLAPPED _pOverlapped, LPDWORD _puNumberOfBytesTransferred, DWORD _uMilliseconds, BOOL _bAlertable); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetOverlappedResultEx( HANDLE _hFile, LPOVERLAPPED _pOverlapped, LPDWORD _puNumberOfBytesTransferred, DWORD _uMilliseconds, BOOL _bAlertable); static YY_THUNK_FNTYPE_GetOverlappedResultEx __attribute__((__cdecl__)) try_get_GetOverlappedResultEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_LoadLibraryExW
__APPLY_UNIT_TEST_BOOL(LoadLibraryExW); typedef HMODULE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_LoadLibraryExW)( LPCWSTR _szLibFileName, HANDLE _hFile, DWORD _fFlags); extern "C" HMODULE __attribute__((__stdcall__)) __FALLBACK_PREFIXLoadLibraryExW( LPCWSTR _szLibFileName, HANDLE _hFile, DWORD _fFlags); static YY_THUNK_FNTYPE_LoadLibraryExW __attribute__((__cdecl__)) try_get_LoadLibraryExW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_LoadLibraryExA
__APPLY_UNIT_TEST_BOOL(LoadLibraryExA); typedef HMODULE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_LoadLibraryExA)( LPCSTR _szLibFileName, HANDLE _hFile, DWORD _fFlags); extern "C" HMODULE __attribute__((__stdcall__)) __FALLBACK_PREFIXLoadLibraryExA( LPCSTR _szLibFileName, HANDLE _hFile, DWORD _fFlags); static YY_THUNK_FNTYPE_LoadLibraryExA __attribute__((__cdecl__)) try_get_LoadLibraryExA() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_FindStringOrdinal
__APPLY_UNIT_TEST_BOOL(FindStringOrdinal); typedef int(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_FindStringOrdinal)( DWORD _uFindStringOrdinalFlags, LPCWSTR _pStringSource, int _cchSource, LPCWSTR _pStringValue, int _cchValue, BOOL _bIgnoreCase); extern "C" int __attribute__((__stdcall__)) __FALLBACK_PREFIXFindStringOrdinal( DWORD _uFindStringOrdinalFlags, LPCWSTR _pStringSource, int _cchSource, LPCWSTR _pStringValue, int _cchValue, BOOL _bIgnoreCase); static YY_THUNK_FNTYPE_FindStringOrdinal __attribute__((__cdecl__)) try_get_FindStringOrdinal() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_LoadLibraryW
__APPLY_UNIT_TEST_BOOL(LoadLibraryW); typedef HMODULE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_LoadLibraryW)( LPCWSTR _szLibFileName); extern "C" HMODULE __attribute__((__stdcall__)) __FALLBACK_PREFIXLoadLibraryW( LPCWSTR _szLibFileName); static YY_THUNK_FNTYPE_LoadLibraryW __attribute__((__cdecl__)) try_get_LoadLibraryW() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_LoadLibraryA
__APPLY_UNIT_TEST_BOOL(LoadLibraryA); typedef HMODULE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_LoadLibraryA)( LPCSTR _szLibFileName); extern "C" HMODULE __attribute__((__stdcall__)) __FALLBACK_PREFIXLoadLibraryA( LPCSTR _szLibFileName); static YY_THUNK_FNTYPE_LoadLibraryA __attribute__((__cdecl__)) try_get_LoadLibraryA() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetDefaultDllDirectories
__APPLY_UNIT_TEST_BOOL(SetDefaultDllDirectories); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetDefaultDllDirectories)( DWORD _fDirectoryFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetDefaultDllDirectories( DWORD _fDirectoryFlags); static YY_THUNK_FNTYPE_SetDefaultDllDirectories __attribute__((__cdecl__)) try_get_SetDefaultDllDirectories() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_AddDllDirectory
__APPLY_UNIT_TEST_BOOL(AddDllDirectory); typedef DLL_DIRECTORY_COOKIE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_AddDllDirectory)( PCWSTR _szNewDirectory); extern "C" DLL_DIRECTORY_COOKIE __attribute__((__stdcall__)) __FALLBACK_PREFIXAddDllDirectory( PCWSTR _szNewDirectory); static YY_THUNK_FNTYPE_AddDllDirectory __attribute__((__cdecl__)) try_get_AddDllDirectory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RemoveDllDirectory
__APPLY_UNIT_TEST_BOOL(RemoveDllDirectory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RemoveDllDirectory)( DLL_DIRECTORY_COOKIE _pCookie); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXRemoveDllDirectory( DLL_DIRECTORY_COOKIE _pCookie); static YY_THUNK_FNTYPE_RemoveDllDirectory __attribute__((__cdecl__)) try_get_RemoveDllDirectory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ResolveLocaleName
__APPLY_UNIT_TEST_BOOL(ResolveLocaleName); typedef int(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_ResolveLocaleName)( LPCWSTR lpNameToResolve, LPWSTR lpLocaleName, int cchLocaleName); extern "C" int __attribute__((__stdcall__)) __FALLBACK_PREFIXResolveLocaleName( LPCWSTR lpNameToResolve, LPWSTR lpLocaleName, int cchLocaleName); static YY_THUNK_FNTYPE_ResolveLocaleName __attribute__((__cdecl__)) try_get_ResolveLocaleName() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetMemoryErrorHandlingCapabilities
__APPLY_UNIT_TEST_BOOL(GetMemoryErrorHandlingCapabilities); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetMemoryErrorHandlingCapabilities)( PULONG Capabilities); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetMemoryErrorHandlingCapabilities( PULONG Capabilities); static YY_THUNK_FNTYPE_GetMemoryErrorHandlingCapabilities __attribute__((__cdecl__)) try_get_GetMemoryErrorHandlingCapabilities() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_VirtualAllocFromApp
__APPLY_UNIT_TEST_BOOL(VirtualAllocFromApp); typedef PVOID(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_VirtualAllocFromApp)( PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG Protection); extern "C" PVOID __attribute__((__stdcall__)) __FALLBACK_PREFIXVirtualAllocFromApp( PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG Protection); static YY_THUNK_FNTYPE_VirtualAllocFromApp __attribute__((__cdecl__)) try_get_VirtualAllocFromApp() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_VirtualAlloc2
__APPLY_UNIT_TEST_BOOL(VirtualAlloc2); typedef PVOID(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_VirtualAlloc2)( HANDLE Process, PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG PageProtection, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount); extern "C" PVOID __attribute__((__stdcall__)) __FALLBACK_PREFIXVirtualAlloc2( HANDLE Process, PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG PageProtection, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount); static YY_THUNK_FNTYPE_VirtualAlloc2 __attribute__((__cdecl__)) try_get_VirtualAlloc2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_VirtualAlloc2FromApp
__APPLY_UNIT_TEST_BOOL(VirtualAlloc2FromApp); typedef PVOID(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_VirtualAlloc2FromApp)( HANDLE Process, PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG PageProtection, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount); extern "C" PVOID __attribute__((__stdcall__)) __FALLBACK_PREFIXVirtualAlloc2FromApp( HANDLE Process, PVOID BaseAddress, SIZE_T Size, ULONG AllocationType, ULONG PageProtection, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount); static YY_THUNK_FNTYPE_VirtualAlloc2FromApp __attribute__((__cdecl__)) try_get_VirtualAlloc2FromApp() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateFileMappingFromApp
__APPLY_UNIT_TEST_BOOL(CreateFileMappingFromApp); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateFileMappingFromApp)( HANDLE hFile, PSECURITY_ATTRIBUTES SecurityAttributes, ULONG PageProtection, ULONG64 MaximumSize, PCWSTR Name); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateFileMappingFromApp( HANDLE hFile, PSECURITY_ATTRIBUTES SecurityAttributes, ULONG PageProtection, ULONG64 MaximumSize, PCWSTR Name); static YY_THUNK_FNTYPE_CreateFileMappingFromApp __attribute__((__cdecl__)) try_get_CreateFileMappingFromApp() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateFileMapping2
__APPLY_UNIT_TEST_BOOL(CreateFileMapping2); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateFileMapping2)( HANDLE File, SECURITY_ATTRIBUTES* SecurityAttributes, ULONG DesiredAccess, ULONG PageProtection, ULONG AllocationAttributes, ULONG64 MaximumSize, PCWSTR Name, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateFileMapping2( HANDLE File, SECURITY_ATTRIBUTES* SecurityAttributes, ULONG DesiredAccess, ULONG PageProtection, ULONG AllocationAttributes, ULONG64 MaximumSize, PCWSTR Name, MEM_EXTENDED_PARAMETER* ExtendedParameters, ULONG ParameterCount); static YY_THUNK_FNTYPE_CreateFileMapping2 __attribute__((__cdecl__)) try_get_CreateFileMapping2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_MapViewOfFileFromApp
__APPLY_UNIT_TEST_BOOL(MapViewOfFileFromApp); typedef PVOID(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_MapViewOfFileFromApp)( HANDLE hFileMappingObject, ULONG DesiredAccess, ULONG64 FileOffset, SIZE_T NumberOfBytesToMap); extern "C" PVOID __attribute__((__stdcall__)) __FALLBACK_PREFIXMapViewOfFileFromApp( HANDLE hFileMappingObject, ULONG DesiredAccess, ULONG64 FileOffset, SIZE_T NumberOfBytesToMap); static YY_THUNK_FNTYPE_MapViewOfFileFromApp __attribute__((__cdecl__)) try_get_MapViewOfFileFromApp() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_UnmapViewOfFileEx
__APPLY_UNIT_TEST_BOOL(UnmapViewOfFileEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_UnmapViewOfFileEx)( PVOID BaseAddress, ULONG UnmapFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXUnmapViewOfFileEx( PVOID BaseAddress, ULONG UnmapFlags); static YY_THUNK_FNTYPE_UnmapViewOfFileEx __attribute__((__cdecl__)) try_get_UnmapViewOfFileEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_VirtualProtectFromApp
__APPLY_UNIT_TEST_BOOL(VirtualProtectFromApp); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_VirtualProtectFromApp)( PVOID Address, SIZE_T Size, ULONG NewProtection, PULONG OldProtection); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXVirtualProtectFromApp( PVOID Address, SIZE_T Size, ULONG NewProtection, PULONG OldProtection); static YY_THUNK_FNTYPE_VirtualProtectFromApp __attribute__((__cdecl__)) try_get_VirtualProtectFromApp() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_OpenFileMappingFromApp
__APPLY_UNIT_TEST_BOOL(OpenFileMappingFromApp); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_OpenFileMappingFromApp)( ULONG DesiredAccess, BOOL InheritHandle, PCWSTR Name); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXOpenFileMappingFromApp( ULONG DesiredAccess, BOOL InheritHandle, PCWSTR Name); static YY_THUNK_FNTYPE_OpenFileMappingFromApp __attribute__((__cdecl__)) try_get_OpenFileMappingFromApp() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DiscardVirtualMemory
__APPLY_UNIT_TEST_BOOL(DiscardVirtualMemory); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DiscardVirtualMemory)( PVOID _pVirtualAddress, SIZE_T _uSize); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXDiscardVirtualMemory( PVOID _pVirtualAddress, SIZE_T _uSize); static YY_THUNK_FNTYPE_DiscardVirtualMemory __attribute__((__cdecl__)) try_get_DiscardVirtualMemory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_OfferVirtualMemory
__APPLY_UNIT_TEST_BOOL(OfferVirtualMemory); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_OfferVirtualMemory)( PVOID _pVirtualAddress, SIZE_T _uSize, OFFER_PRIORITY _ePriority); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXOfferVirtualMemory( PVOID _pVirtualAddress, SIZE_T _uSize, OFFER_PRIORITY _ePriority); static YY_THUNK_FNTYPE_OfferVirtualMemory __attribute__((__cdecl__)) try_get_OfferVirtualMemory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_ReclaimVirtualMemory
__APPLY_UNIT_TEST_BOOL(ReclaimVirtualMemory); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_ReclaimVirtualMemory)( PVOID _pVirtualAddress, SIZE_T _uSize); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXReclaimVirtualMemory( PVOID _pVirtualAddress, SIZE_T _uSize); static YY_THUNK_FNTYPE_ReclaimVirtualMemory __attribute__((__cdecl__)) try_get_ReclaimVirtualMemory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PrefetchVirtualMemory
__APPLY_UNIT_TEST_BOOL(PrefetchVirtualMemory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PrefetchVirtualMemory)( HANDLE _hProcess, ULONG_PTR _uNumberOfEntries, PWIN32_MEMORY_RANGE_ENTRY _pVirtualAddresses, ULONG _fFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXPrefetchVirtualMemory( HANDLE _hProcess, ULONG_PTR _uNumberOfEntries, PWIN32_MEMORY_RANGE_ENTRY _pVirtualAddresses, ULONG _fFlags); static YY_THUNK_FNTYPE_PrefetchVirtualMemory __attribute__((__cdecl__)) try_get_PrefetchVirtualMemory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathIsUNCEx
__APPLY_UNIT_TEST_BOOL(PathIsUNCEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathIsUNCEx)( PCWSTR pszPath, PCWSTR* ppszServer); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXPathIsUNCEx( PCWSTR pszPath, PCWSTR* ppszServer); static YY_THUNK_FNTYPE_PathIsUNCEx __attribute__((__cdecl__)) try_get_PathIsUNCEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchIsRoot
__APPLY_UNIT_TEST_BOOL(PathCchIsRoot); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchIsRoot)( PCWSTR pszPath); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchIsRoot( PCWSTR pszPath); static YY_THUNK_FNTYPE_PathCchIsRoot __attribute__((__cdecl__)) try_get_PathCchIsRoot() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchAddBackslashEx
__APPLY_UNIT_TEST_BOOL(PathCchAddBackslashEx); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchAddBackslashEx)( PWSTR pszPath, size_t cchPath, PWSTR* ppszEnd, size_t* pcchRemaining); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchAddBackslashEx( PWSTR pszPath, size_t cchPath, PWSTR* ppszEnd, size_t* pcchRemaining); static YY_THUNK_FNTYPE_PathCchAddBackslashEx __attribute__((__cdecl__)) try_get_PathCchAddBackslashEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchAddBackslash
__APPLY_UNIT_TEST_BOOL(PathCchAddBackslash); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchAddBackslash)( PWSTR pszPath, size_t cchPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchAddBackslash( PWSTR pszPath, size_t cchPath); static YY_THUNK_FNTYPE_PathCchAddBackslash __attribute__((__cdecl__)) try_get_PathCchAddBackslash() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchRemoveBackslashEx
__APPLY_UNIT_TEST_BOOL(PathCchRemoveBackslashEx); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchRemoveBackslashEx)( PWSTR pszPath, size_t cchPath, PWSTR* ppszEnd, size_t* pcchRemaining); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchRemoveBackslashEx( PWSTR pszPath, size_t cchPath, PWSTR* ppszEnd, size_t* pcchRemaining); static YY_THUNK_FNTYPE_PathCchRemoveBackslashEx __attribute__((__cdecl__)) try_get_PathCchRemoveBackslashEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchRemoveBackslash
__APPLY_UNIT_TEST_BOOL(PathCchRemoveBackslash); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchRemoveBackslash)( PWSTR pszPath, size_t cchPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchRemoveBackslash( PWSTR pszPath, size_t cchPath); static YY_THUNK_FNTYPE_PathCchRemoveBackslash __attribute__((__cdecl__)) try_get_PathCchRemoveBackslash() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchSkipRoot
__APPLY_UNIT_TEST_BOOL(PathCchSkipRoot); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchSkipRoot)( PCWSTR pszPath, PCWSTR* ppszRootEnd); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchSkipRoot( PCWSTR pszPath, PCWSTR* ppszRootEnd); static YY_THUNK_FNTYPE_PathCchSkipRoot __attribute__((__cdecl__)) try_get_PathCchSkipRoot() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchStripToRoot
__APPLY_UNIT_TEST_BOOL(PathCchStripToRoot); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchStripToRoot)( PWSTR pszPath, size_t cchPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchStripToRoot( PWSTR pszPath, size_t cchPath); static YY_THUNK_FNTYPE_PathCchStripToRoot __attribute__((__cdecl__)) try_get_PathCchStripToRoot() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchRemoveFileSpec
__APPLY_UNIT_TEST_BOOL(PathCchRemoveFileSpec); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchRemoveFileSpec)( PWSTR pszPath, size_t cchPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchRemoveFileSpec( PWSTR pszPath, size_t cchPath); static YY_THUNK_FNTYPE_PathCchRemoveFileSpec __attribute__((__cdecl__)) try_get_PathCchRemoveFileSpec() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchFindExtension
__APPLY_UNIT_TEST_BOOL(PathCchFindExtension); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchFindExtension)( PCWSTR pszPath, size_t cchPath, PCWSTR* ppszExt); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchFindExtension( PCWSTR pszPath, size_t cchPath, PCWSTR* ppszExt); static YY_THUNK_FNTYPE_PathCchFindExtension __attribute__((__cdecl__)) try_get_PathCchFindExtension() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchAddExtension
__APPLY_UNIT_TEST_BOOL(PathCchAddExtension); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchAddExtension)( PWSTR pszPath, size_t cchPath, PCWSTR pszExt); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchAddExtension( PWSTR pszPath, size_t cchPath, PCWSTR pszExt); static YY_THUNK_FNTYPE_PathCchAddExtension __attribute__((__cdecl__)) try_get_PathCchAddExtension() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchRenameExtension
__APPLY_UNIT_TEST_BOOL(PathCchRenameExtension); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchRenameExtension)( PWSTR pszPath, size_t cchPath, PCWSTR pszExt); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchRenameExtension( PWSTR pszPath, size_t cchPath, PCWSTR pszExt); static YY_THUNK_FNTYPE_PathCchRenameExtension __attribute__((__cdecl__)) try_get_PathCchRenameExtension() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchRemoveExtension
__APPLY_UNIT_TEST_BOOL(PathCchRemoveExtension); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchRemoveExtension)( PWSTR pszPath, size_t cchPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchRemoveExtension( PWSTR pszPath, size_t cchPath); static YY_THUNK_FNTYPE_PathCchRemoveExtension __attribute__((__cdecl__)) try_get_PathCchRemoveExtension() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchCanonicalizeEx
__APPLY_UNIT_TEST_BOOL(PathCchCanonicalizeEx); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchCanonicalizeEx)( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn, ULONG dwFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchCanonicalizeEx( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn, ULONG dwFlags); static YY_THUNK_FNTYPE_PathCchCanonicalizeEx __attribute__((__cdecl__)) try_get_PathCchCanonicalizeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchCanonicalize
__APPLY_UNIT_TEST_BOOL(PathCchCanonicalize); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchCanonicalize)( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchCanonicalize( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn); static YY_THUNK_FNTYPE_PathCchCanonicalize __attribute__((__cdecl__)) try_get_PathCchCanonicalize() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchCombineEx
__APPLY_UNIT_TEST_BOOL(PathCchCombineEx); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchCombineEx)( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn, PCWSTR pszMore, ULONG dwFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchCombineEx( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn, PCWSTR pszMore, ULONG dwFlags); static YY_THUNK_FNTYPE_PathCchCombineEx __attribute__((__cdecl__)) try_get_PathCchCombineEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchCombine
__APPLY_UNIT_TEST_BOOL(PathCchCombine); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchCombine)( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn, PCWSTR pszMore); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchCombine( PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn, PCWSTR pszMore); static YY_THUNK_FNTYPE_PathCchCombine __attribute__((__cdecl__)) try_get_PathCchCombine() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchAppendEx
__APPLY_UNIT_TEST_BOOL(PathCchAppendEx); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchAppendEx)( PWSTR pszPath, size_t cchPath, PCWSTR pszMore, ULONG dwFlags); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchAppendEx( PWSTR pszPath, size_t cchPath, PCWSTR pszMore, ULONG dwFlags); static YY_THUNK_FNTYPE_PathCchAppendEx __attribute__((__cdecl__)) try_get_PathCchAppendEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchAppend
__APPLY_UNIT_TEST_BOOL(PathCchAppend); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchAppend)( PWSTR pszPath, size_t cchPath, PCWSTR pszMore); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchAppend( PWSTR pszPath, size_t cchPath, PCWSTR pszMore); static YY_THUNK_FNTYPE_PathCchAppend __attribute__((__cdecl__)) try_get_PathCchAppend() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathCchStripPrefix
__APPLY_UNIT_TEST_BOOL(PathCchStripPrefix); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathCchStripPrefix)( PWSTR pszPath, size_t cchPath); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathCchStripPrefix( PWSTR pszPath, size_t cchPath); static YY_THUNK_FNTYPE_PathCchStripPrefix __attribute__((__cdecl__)) try_get_PathCchStripPrefix() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathAllocCombine
__APPLY_UNIT_TEST_BOOL(PathAllocCombine); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathAllocCombine)( PCWSTR pszPathIn, PCWSTR pszMore, ULONG dwFlags, PWSTR* ppszPathOut); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathAllocCombine( PCWSTR pszPathIn, PCWSTR pszMore, ULONG dwFlags, PWSTR* ppszPathOut); static YY_THUNK_FNTYPE_PathAllocCombine __attribute__((__cdecl__)) try_get_PathAllocCombine() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_PathAllocCanonicalize
__APPLY_UNIT_TEST_BOOL(PathAllocCanonicalize); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_PathAllocCanonicalize)( PCWSTR pszPathIn, ULONG dwFlags, PWSTR* ppszPathOut); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXPathAllocCanonicalize( PCWSTR pszPathIn, ULONG dwFlags, PWSTR* ppszPathOut); static YY_THUNK_FNTYPE_PathAllocCanonicalize __attribute__((__cdecl__)) try_get_PathAllocCanonicalize() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetCurrentProcessorNumberEx
__APPLY_UNIT_TEST_BOOL(GetCurrentProcessorNumberEx); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetCurrentProcessorNumberEx)( PPROCESSOR_NUMBER ProcNumber); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXGetCurrentProcessorNumberEx( PPROCESSOR_NUMBER ProcNumber); static YY_THUNK_FNTYPE_GetCurrentProcessorNumberEx __attribute__((__cdecl__)) try_get_GetCurrentProcessorNumberEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetProcessMitigationPolicy
__APPLY_UNIT_TEST_BOOL(GetProcessMitigationPolicy); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetProcessMitigationPolicy)( HANDLE _hProcess, PROCESS_MITIGATION_POLICY _eMitigationPolicy, PVOID _pBuffer, SIZE_T _cbLength); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetProcessMitigationPolicy( HANDLE _hProcess, PROCESS_MITIGATION_POLICY _eMitigationPolicy, PVOID _pBuffer, SIZE_T _cbLength); static YY_THUNK_FNTYPE_GetProcessMitigationPolicy __attribute__((__cdecl__)) try_get_GetProcessMitigationPolicy() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetProcessMitigationPolicy
__APPLY_UNIT_TEST_BOOL(SetProcessMitigationPolicy); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetProcessMitigationPolicy)( PROCESS_MITIGATION_POLICY _eMitigationPolicy, PVOID _pBuffer, SIZE_T _cbLength); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetProcessMitigationPolicy( PROCESS_MITIGATION_POLICY _eMitigationPolicy, PVOID _pBuffer, SIZE_T _cbLength); static YY_THUNK_FNTYPE_SetProcessMitigationPolicy __attribute__((__cdecl__)) try_get_SetProcessMitigationPolicy() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetProcessInformation
__APPLY_UNIT_TEST_BOOL(SetProcessInformation); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetProcessInformation)( HANDLE _hProcess, PROCESS_INFORMATION_CLASS _eProcessInformationClass, LPVOID _pProcessInformation, DWORD _cbProcessInformationSize); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetProcessInformation( HANDLE _hProcess, PROCESS_INFORMATION_CLASS _eProcessInformationClass, LPVOID _pProcessInformation, DWORD _cbProcessInformationSize); static YY_THUNK_FNTYPE_SetProcessInformation __attribute__((__cdecl__)) try_get_SetProcessInformation() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetThreadInformation
__APPLY_UNIT_TEST_BOOL(SetThreadInformation); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetThreadInformation)( HANDLE _hThread, THREAD_INFORMATION_CLASS _eThreadInformationClass, LPVOID _pThreadInformation, DWORD _cbThreadInformationSize); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetThreadInformation( HANDLE _hThread, THREAD_INFORMATION_CLASS _eThreadInformationClass, LPVOID _pThreadInformation, DWORD _cbThreadInformationSize); static YY_THUNK_FNTYPE_SetThreadInformation __attribute__((__cdecl__)) try_get_SetThreadInformation() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetThreadInformation
__APPLY_UNIT_TEST_BOOL(GetThreadInformation); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetThreadInformation)( HANDLE _hThread, THREAD_INFORMATION_CLASS _eThreadInformationClass, LPVOID _pThreadInformation, DWORD _cbThreadInformationSize); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetThreadInformation( HANDLE _hThread, THREAD_INFORMATION_CLASS _eThreadInformationClass, LPVOID _pThreadInformation, DWORD _cbThreadInformationSize); static YY_THUNK_FNTYPE_GetThreadInformation __attribute__((__cdecl__)) try_get_GetThreadInformation() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetCurrentThreadStackLimits
__APPLY_UNIT_TEST_BOOL(GetCurrentThreadStackLimits); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetCurrentThreadStackLimits)( PULONG_PTR _pLowLimit, PULONG_PTR _pHighLimit); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXGetCurrentThreadStackLimits( PULONG_PTR _pLowLimit, PULONG_PTR _pHighLimit); static YY_THUNK_FNTYPE_GetCurrentThreadStackLimits __attribute__((__cdecl__)) try_get_GetCurrentThreadStackLimits() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetThreadIdealProcessorEx
__APPLY_UNIT_TEST_BOOL(SetThreadIdealProcessorEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetThreadIdealProcessorEx)( HANDLE _hThread, PPROCESSOR_NUMBER _pIdealProcessor, PPROCESSOR_NUMBER _pPreviousIdealProcessor); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetThreadIdealProcessorEx( HANDLE _hThread, PPROCESSOR_NUMBER _pIdealProcessor, PPROCESSOR_NUMBER _pPreviousIdealProcessor); static YY_THUNK_FNTYPE_SetThreadIdealProcessorEx __attribute__((__cdecl__)) try_get_SetThreadIdealProcessorEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetThreadIdealProcessorEx
__APPLY_UNIT_TEST_BOOL(GetThreadIdealProcessorEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetThreadIdealProcessorEx)( HANDLE _hThread, PPROCESSOR_NUMBER _pIdealProcessor); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetThreadIdealProcessorEx( HANDLE _hThread, PPROCESSOR_NUMBER _pIdealProcessor); static YY_THUNK_FNTYPE_GetThreadIdealProcessorEx __attribute__((__cdecl__)) try_get_GetThreadIdealProcessorEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateRemoteThreadEx
__APPLY_UNIT_TEST_BOOL(CreateRemoteThreadEx); typedef HANDLE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateRemoteThreadEx)( HANDLE _hProcess, LPSECURITY_ATTRIBUTES _pThreadAttributes, SIZE_T _uStackSize, LPTHREAD_START_ROUTINE _pStartAddress, LPVOID _pParameter, DWORD _fCreationFlags, LPPROC_THREAD_ATTRIBUTE_LIST _pAttributeList, LPDWORD _pThreadId); extern "C" HANDLE __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateRemoteThreadEx( HANDLE _hProcess, LPSECURITY_ATTRIBUTES _pThreadAttributes, SIZE_T _uStackSize, LPTHREAD_START_ROUTINE _pStartAddress, LPVOID _pParameter, DWORD _fCreationFlags, LPPROC_THREAD_ATTRIBUTE_LIST _pAttributeList, LPDWORD _pThreadId); static YY_THUNK_FNTYPE_CreateRemoteThreadEx __attribute__((__cdecl__)) try_get_CreateRemoteThreadEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetThreadDescription
__APPLY_UNIT_TEST_BOOL(GetThreadDescription); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetThreadDescription)( HANDLE _hThread, PWSTR* _pszThreadDescription); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetThreadDescription( HANDLE _hThread, PWSTR* _pszThreadDescription); static YY_THUNK_FNTYPE_GetThreadDescription __attribute__((__cdecl__)) try_get_GetThreadDescription() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetThreadDescription
__APPLY_UNIT_TEST_BOOL(SetThreadDescription); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetThreadDescription)( HANDLE _hThread, PCWSTR _szThreadDescription); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXSetThreadDescription( HANDLE _hThread, PCWSTR _szThreadDescription); static YY_THUNK_FNTYPE_SetThreadDescription __attribute__((__cdecl__)) try_get_SetThreadDescription() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetMaximumProcessorCount
__APPLY_UNIT_TEST_BOOL(GetMaximumProcessorCount); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetMaximumProcessorCount)( WORD GroupNumber); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetMaximumProcessorCount( WORD GroupNumber); static YY_THUNK_FNTYPE_GetMaximumProcessorCount __attribute__((__cdecl__)) try_get_GetMaximumProcessorCount() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetActiveProcessorCount
__APPLY_UNIT_TEST_BOOL(GetActiveProcessorCount); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetActiveProcessorCount)( WORD GroupNumber); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetActiveProcessorCount( WORD GroupNumber); static YY_THUNK_FNTYPE_GetActiveProcessorCount __attribute__((__cdecl__)) try_get_GetActiveProcessorCount() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetActiveProcessorGroupCount
__APPLY_UNIT_TEST_BOOL(GetActiveProcessorGroupCount); typedef WORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetActiveProcessorGroupCount)(void); extern "C" WORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetActiveProcessorGroupCount(void); static YY_THUNK_FNTYPE_GetActiveProcessorGroupCount __attribute__((__cdecl__)) try_get_GetActiveProcessorGroupCount() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetMaximumProcessorGroupCount
__APPLY_UNIT_TEST_BOOL(GetMaximumProcessorGroupCount); typedef WORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetMaximumProcessorGroupCount)(void); extern "C" WORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetMaximumProcessorGroupCount(void); static YY_THUNK_FNTYPE_GetMaximumProcessorGroupCount __attribute__((__cdecl__)) try_get_GetMaximumProcessorGroupCount() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_QueryUnbiasedInterruptTime
__APPLY_UNIT_TEST_BOOL(QueryUnbiasedInterruptTime); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_QueryUnbiasedInterruptTime)( PULONGLONG _puUnbiasedTime); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXQueryUnbiasedInterruptTime( PULONGLONG _puUnbiasedTime); static YY_THUNK_FNTYPE_QueryUnbiasedInterruptTime __attribute__((__cdecl__)) try_get_QueryUnbiasedInterruptTime() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_QueryUnbiasedInterruptTimePrecise
__APPLY_UNIT_TEST_BOOL(QueryUnbiasedInterruptTimePrecise); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_QueryUnbiasedInterruptTimePrecise)( PULONGLONG _puUnbiasedInterruptTimePrecise); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXQueryUnbiasedInterruptTimePrecise( PULONGLONG _puUnbiasedInterruptTimePrecise); static YY_THUNK_FNTYPE_QueryUnbiasedInterruptTimePrecise __attribute__((__cdecl__)) try_get_QueryUnbiasedInterruptTimePrecise() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_QueryInterruptTime
__APPLY_UNIT_TEST_BOOL(QueryInterruptTime); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_QueryInterruptTime)( PULONGLONG _puInterruptTime); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXQueryInterruptTime( PULONGLONG _puInterruptTime); static YY_THUNK_FNTYPE_QueryInterruptTime __attribute__((__cdecl__)) try_get_QueryInterruptTime() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_QueryInterruptTimePrecise
__APPLY_UNIT_TEST_BOOL(QueryInterruptTimePrecise); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_QueryInterruptTimePrecise)( PULONGLONG _puInterruptTimePrecise); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXQueryInterruptTimePrecise( PULONGLONG _puInterruptTimePrecise); static YY_THUNK_FNTYPE_QueryInterruptTimePrecise __attribute__((__cdecl__)) try_get_QueryInterruptTimePrecise() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_QueryIdleProcessorCycleTimeEx
__APPLY_UNIT_TEST_BOOL(QueryIdleProcessorCycleTimeEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_QueryIdleProcessorCycleTimeEx)( USHORT _uGroup, PULONG _puBufferLength, PULONG64 _puProcessorIdleCycleTime); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXQueryIdleProcessorCycleTimeEx( USHORT _uGroup, PULONG _puBufferLength, PULONG64 _puProcessorIdleCycleTime); static YY_THUNK_FNTYPE_QueryIdleProcessorCycleTimeEx __attribute__((__cdecl__)) try_get_QueryIdleProcessorCycleTimeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RegCloseKey
__APPLY_UNIT_TEST_BOOL(RegCloseKey); typedef LSTATUS(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RegCloseKey)( HKEY _hKey); extern "C" LSTATUS __attribute__((__stdcall__)) __FALLBACK_PREFIXRegCloseKey( HKEY _hKey); static YY_THUNK_FNTYPE_RegCloseKey __attribute__((__cdecl__)) try_get_RegCloseKey() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_TryAcquireSRWLockExclusive
__APPLY_UNIT_TEST_BOOL(TryAcquireSRWLockExclusive); typedef BOOLEAN(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_TryAcquireSRWLockExclusive)( PSRWLOCK SRWLock); extern "C" BOOLEAN __attribute__((__stdcall__)) __FALLBACK_PREFIXTryAcquireSRWLockExclusive( PSRWLOCK SRWLock); static YY_THUNK_FNTYPE_TryAcquireSRWLockExclusive __attribute__((__cdecl__)) try_get_TryAcquireSRWLockExclusive() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_TryAcquireSRWLockShared
__APPLY_UNIT_TEST_BOOL(TryAcquireSRWLockShared); typedef BOOLEAN(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_TryAcquireSRWLockShared)( PSRWLOCK SRWLock); extern "C" BOOLEAN __attribute__((__stdcall__)) __FALLBACK_PREFIXTryAcquireSRWLockShared( PSRWLOCK SRWLock); static YY_THUNK_FNTYPE_TryAcquireSRWLockShared __attribute__((__cdecl__)) try_get_TryAcquireSRWLockShared() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_InitializeSynchronizationBarrier
__APPLY_UNIT_TEST_BOOL(InitializeSynchronizationBarrier); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_InitializeSynchronizationBarrier)( LPSYNCHRONIZATION_BARRIER lpBarrier, LONG lTotalThreads, LONG lSpinCount); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXInitializeSynchronizationBarrier( LPSYNCHRONIZATION_BARRIER lpBarrier, LONG lTotalThreads, LONG lSpinCount); static YY_THUNK_FNTYPE_InitializeSynchronizationBarrier __attribute__((__cdecl__)) try_get_InitializeSynchronizationBarrier() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_EnterSynchronizationBarrier
__APPLY_UNIT_TEST_BOOL(EnterSynchronizationBarrier); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_EnterSynchronizationBarrier)( LPSYNCHRONIZATION_BARRIER lpBarrier, DWORD dwFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXEnterSynchronizationBarrier( LPSYNCHRONIZATION_BARRIER lpBarrier, DWORD dwFlags); static YY_THUNK_FNTYPE_EnterSynchronizationBarrier __attribute__((__cdecl__)) try_get_EnterSynchronizationBarrier() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_DeleteSynchronizationBarrier
__APPLY_UNIT_TEST_BOOL(DeleteSynchronizationBarrier); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_DeleteSynchronizationBarrier)( LPSYNCHRONIZATION_BARRIER lpBarrier); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXDeleteSynchronizationBarrier( LPSYNCHRONIZATION_BARRIER lpBarrier); static YY_THUNK_FNTYPE_DeleteSynchronizationBarrier __attribute__((__cdecl__)) try_get_DeleteSynchronizationBarrier() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WaitOnAddress
__APPLY_UNIT_TEST_BOOL(WaitOnAddress); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WaitOnAddress)( volatile void* Address, PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXWaitOnAddress( volatile void* Address, PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds); static YY_THUNK_FNTYPE_WaitOnAddress __attribute__((__cdecl__)) try_get_WaitOnAddress() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WakeByAddressSingle
__APPLY_UNIT_TEST_BOOL(WakeByAddressSingle); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WakeByAddressSingle)( PVOID Address); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXWakeByAddressSingle( PVOID Address); static YY_THUNK_FNTYPE_WakeByAddressSingle __attribute__((__cdecl__)) try_get_WakeByAddressSingle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_WakeByAddressAll
__APPLY_UNIT_TEST_BOOL(WakeByAddressAll); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_WakeByAddressAll)( PVOID Address); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXWakeByAddressAll( PVOID Address); static YY_THUNK_FNTYPE_WakeByAddressAll __attribute__((__cdecl__)) try_get_WakeByAddressAll() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetWaitableTimerEx
__APPLY_UNIT_TEST_BOOL(SetWaitableTimerEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetWaitableTimerEx)( HANDLE _hTimer, const LARGE_INTEGER* _lpDueTime, LONG _iPeriod, PTIMERAPCROUTINE _pfnCompletionRoutine, LPVOID _lpArgToCompletionRoutine, PREASON_CONTEXT _pWakeContext, ULONG _uTolerableDelay); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetWaitableTimerEx( HANDLE _hTimer, const LARGE_INTEGER* _lpDueTime, LONG _iPeriod, PTIMERAPCROUTINE _pfnCompletionRoutine, LPVOID _lpArgToCompletionRoutine, PREASON_CONTEXT _pWakeContext, ULONG _uTolerableDelay); static YY_THUNK_FNTYPE_SetWaitableTimerEx __attribute__((__cdecl__)) try_get_SetWaitableTimerEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetSystemTimePreciseAsFileTime
__APPLY_UNIT_TEST_BOOL(GetSystemTimePreciseAsFileTime); typedef void(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetSystemTimePreciseAsFileTime)( LPFILETIME lpSystemTimeAsFileTime); extern "C" void __attribute__((__stdcall__)) __FALLBACK_PREFIXGetSystemTimePreciseAsFileTime( LPFILETIME lpSystemTimeAsFileTime); static YY_THUNK_FNTYPE_GetSystemTimePreciseAsFileTime __attribute__((__cdecl__)) try_get_GetSystemTimePreciseAsFileTime() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetLogicalProcessorInformationEx
__APPLY_UNIT_TEST_BOOL(GetLogicalProcessorInformationEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetLogicalProcessorInformationEx)( LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer, PDWORD ReturnedLength); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetLogicalProcessorInformationEx( LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer, PDWORD ReturnedLength); static YY_THUNK_FNTYPE_GetLogicalProcessorInformationEx __attribute__((__cdecl__)) try_get_GetLogicalProcessorInformationEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetPhysicallyInstalledSystemMemory
__APPLY_UNIT_TEST_BOOL(GetPhysicallyInstalledSystemMemory); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetPhysicallyInstalledSystemMemory)( PULONGLONG _puTotalMemoryInKilobytes); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetPhysicallyInstalledSystemMemory( PULONGLONG _puTotalMemoryInKilobytes); static YY_THUNK_FNTYPE_GetPhysicallyInstalledSystemMemory __attribute__((__cdecl__)) try_get_GetPhysicallyInstalledSystemMemory() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetNumaNodeProcessorMaskEx
__APPLY_UNIT_TEST_BOOL(GetNumaNodeProcessorMaskEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetNumaNodeProcessorMaskEx)( USHORT Node, PGROUP_AFFINITY ProcessorMask); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetNumaNodeProcessorMaskEx( USHORT Node, PGROUP_AFFINITY ProcessorMask); static YY_THUNK_FNTYPE_GetNumaNodeProcessorMaskEx __attribute__((__cdecl__)) try_get_GetNumaNodeProcessorMaskEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetThreadGroupAffinity
__APPLY_UNIT_TEST_BOOL(GetThreadGroupAffinity); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetThreadGroupAffinity)( HANDLE _hThread, PGROUP_AFFINITY _pAffinity); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetThreadGroupAffinity( HANDLE _hThread, PGROUP_AFFINITY _pAffinity); static YY_THUNK_FNTYPE_GetThreadGroupAffinity __attribute__((__cdecl__)) try_get_GetThreadGroupAffinity() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetThreadGroupAffinity
__APPLY_UNIT_TEST_BOOL(SetThreadGroupAffinity); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetThreadGroupAffinity)( HANDLE hThread, const GROUP_AFFINITY* GroupAffinity, PGROUP_AFFINITY PreviousGroupAffinity); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetThreadGroupAffinity( HANDLE hThread, const GROUP_AFFINITY* GroupAffinity, PGROUP_AFFINITY PreviousGroupAffinity); static YY_THUNK_FNTYPE_SetThreadGroupAffinity __attribute__((__cdecl__)) try_get_SetThreadGroupAffinity() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetNumaProximityNodeEx
__APPLY_UNIT_TEST_BOOL(GetNumaProximityNodeEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetNumaProximityNodeEx)( ULONG ProximityId, PUSHORT NodeNumber); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetNumaProximityNodeEx( ULONG ProximityId, PUSHORT NodeNumber); static YY_THUNK_FNTYPE_GetNumaProximityNodeEx __attribute__((__cdecl__)) try_get_GetNumaProximityNodeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_TzSpecificLocalTimeToSystemTimeEx
__APPLY_UNIT_TEST_BOOL(TzSpecificLocalTimeToSystemTimeEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_TzSpecificLocalTimeToSystemTimeEx)( const DYNAMIC_TIME_ZONE_INFORMATION* _pTimeZoneInformation, const SYSTEMTIME* _pLocalTime, LPSYSTEMTIME _pUniversalTime); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXTzSpecificLocalTimeToSystemTimeEx( const DYNAMIC_TIME_ZONE_INFORMATION* _pTimeZoneInformation, const SYSTEMTIME* _pLocalTime, LPSYSTEMTIME _pUniversalTime); static YY_THUNK_FNTYPE_TzSpecificLocalTimeToSystemTimeEx __attribute__((__cdecl__)) try_get_TzSpecificLocalTimeToSystemTimeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SystemTimeToTzSpecificLocalTimeEx
__APPLY_UNIT_TEST_BOOL(SystemTimeToTzSpecificLocalTimeEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SystemTimeToTzSpecificLocalTimeEx)( const DYNAMIC_TIME_ZONE_INFORMATION* _pTimeZoneInformation, const SYSTEMTIME* _pUniversalTime, LPSYSTEMTIME _pLocalTime); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSystemTimeToTzSpecificLocalTimeEx( const DYNAMIC_TIME_ZONE_INFORMATION* _pTimeZoneInformation, const SYSTEMTIME* _pUniversalTime, LPSYSTEMTIME _pLocalTime); static YY_THUNK_FNTYPE_SystemTimeToTzSpecificLocalTimeEx __attribute__((__cdecl__)) try_get_SystemTimeToTzSpecificLocalTimeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetTimeZoneInformationForYear
__APPLY_UNIT_TEST_BOOL(GetTimeZoneInformationForYear); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetTimeZoneInformationForYear)( USHORT _uYear, PDYNAMIC_TIME_ZONE_INFORMATION _pDynamicTimeZoneInfo, LPTIME_ZONE_INFORMATION _pTimeZoneInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetTimeZoneInformationForYear( USHORT _uYear, PDYNAMIC_TIME_ZONE_INFORMATION _pDynamicTimeZoneInfo, LPTIME_ZONE_INFORMATION _pTimeZoneInfo); static YY_THUNK_FNTYPE_GetTimeZoneInformationForYear __attribute__((__cdecl__)) try_get_GetTimeZoneInformationForYear() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetDynamicTimeZoneInformationEffectiveYears
__APPLY_UNIT_TEST_BOOL(GetDynamicTimeZoneInformationEffectiveYears); typedef DWORD(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetDynamicTimeZoneInformationEffectiveYears)( const PDYNAMIC_TIME_ZONE_INFORMATION _pTimeZoneInformation, LPDWORD _puFirstYear, LPDWORD _puLastYear); extern "C" DWORD __attribute__((__stdcall__)) __FALLBACK_PREFIXGetDynamicTimeZoneInformationEffectiveYears( const PDYNAMIC_TIME_ZONE_INFORMATION _pTimeZoneInformation, LPDWORD _puFirstYear, LPDWORD _puLastYear); static YY_THUNK_FNTYPE_GetDynamicTimeZoneInformationEffectiveYears __attribute__((__cdecl__)) try_get_GetDynamicTimeZoneInformationEffectiveYears() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_IsWow64Process2
__APPLY_UNIT_TEST_BOOL(IsWow64Process2); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_IsWow64Process2)( HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXIsWow64Process2( HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine); static YY_THUNK_FNTYPE_IsWow64Process2 __attribute__((__cdecl__)) try_get_IsWow64Process2() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_IsWow64GuestMachineSupported
__APPLY_UNIT_TEST_BOOL(IsWow64GuestMachineSupported); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_IsWow64GuestMachineSupported)( USHORT WowGuestMachine, BOOL* MachineIsSupported); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXIsWow64GuestMachineSupported( USHORT WowGuestMachine, BOOL* MachineIsSupported); static YY_THUNK_FNTYPE_IsWow64GuestMachineSupported __attribute__((__cdecl__)) try_get_IsWow64GuestMachineSupported() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetNumaNodeNumberFromHandle
__APPLY_UNIT_TEST_BOOL(GetNumaNodeNumberFromHandle); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetNumaNodeNumberFromHandle)( HANDLE hFile, PUSHORT NodeNumber); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetNumaNodeNumberFromHandle( HANDLE hFile, PUSHORT NodeNumber); static YY_THUNK_FNTYPE_GetNumaNodeNumberFromHandle __attribute__((__cdecl__)) try_get_GetNumaNodeNumberFromHandle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetNumaProcessorNodeEx
__APPLY_UNIT_TEST_BOOL(GetNumaProcessorNodeEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetNumaProcessorNodeEx)( PPROCESSOR_NUMBER Processor, PUSHORT NodeNumber); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetNumaProcessorNodeEx( PPROCESSOR_NUMBER Processor, PUSHORT NodeNumber); static YY_THUNK_FNTYPE_GetNumaProcessorNodeEx __attribute__((__cdecl__)) try_get_GetNumaProcessorNodeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetNumaAvailableMemoryNodeEx
__APPLY_UNIT_TEST_BOOL(GetNumaAvailableMemoryNodeEx); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetNumaAvailableMemoryNodeEx)( USHORT Node, PULONGLONG AvailableBytes); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetNumaAvailableMemoryNodeEx( USHORT Node, PULONGLONG AvailableBytes); static YY_THUNK_FNTYPE_GetNumaAvailableMemoryNodeEx __attribute__((__cdecl__)) try_get_GetNumaAvailableMemoryNodeEx() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetFirmwareType
__APPLY_UNIT_TEST_BOOL(GetFirmwareType); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetFirmwareType)( PFIRMWARE_TYPE _peFirmwareType); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetFirmwareType( PFIRMWARE_TYPE _peFirmwareType); static YY_THUNK_FNTYPE_GetFirmwareType __attribute__((__cdecl__)) try_get_GetFirmwareType() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_IsNativeVhdBoot
__APPLY_UNIT_TEST_BOOL(IsNativeVhdBoot); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_IsNativeVhdBoot)( PBOOL _pbNativeVhdBoot); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXIsNativeVhdBoot( PBOOL _pbNativeVhdBoot); static YY_THUNK_FNTYPE_IsNativeVhdBoot __attribute__((__cdecl__)) try_get_IsNativeVhdBoot() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetProcessDEPPolicy
__APPLY_UNIT_TEST_BOOL(SetProcessDEPPolicy); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetProcessDEPPolicy)( DWORD dwFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetProcessDEPPolicy( DWORD dwFlags); static YY_THUNK_FNTYPE_SetProcessDEPPolicy __attribute__((__cdecl__)) try_get_SetProcessDEPPolicy() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetSystemDEPPolicy
__APPLY_UNIT_TEST_BOOL(GetSystemDEPPolicy); typedef DEP_SYSTEM_POLICY_TYPE(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetSystemDEPPolicy)(); extern "C" DEP_SYSTEM_POLICY_TYPE __attribute__((__stdcall__)) __FALLBACK_PREFIXGetSystemDEPPolicy(); static YY_THUNK_FNTYPE_GetSystemDEPPolicy __attribute__((__cdecl__)) try_get_GetSystemDEPPolicy() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_Shell_NotifyIconGetRect
__APPLY_UNIT_TEST_BOOL(Shell_NotifyIconGetRect); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_Shell_NotifyIconGetRect)( const NOTIFYICONIDENTIFIER* _pIdentifier, RECT* _pIconLocation); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXShell_NotifyIconGetRect( const NOTIFYICONIDENTIFIER* _pIdentifier, RECT* _pIconLocation); static YY_THUNK_FNTYPE_Shell_NotifyIconGetRect __attribute__((__cdecl__)) try_get_Shell_NotifyIconGetRect() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SHGetPropertyStoreForWindow
__APPLY_UNIT_TEST_BOOL(SHGetPropertyStoreForWindow); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SHGetPropertyStoreForWindow)( HWND hwnd, const IID & riid, void** ppv); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXSHGetPropertyStoreForWindow( HWND hwnd, const IID & riid, void** ppv); static YY_THUNK_FNTYPE_SHGetPropertyStoreForWindow __attribute__((__cdecl__)) try_get_SHGetPropertyStoreForWindow() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetProcessDpiAwarenessContext
__APPLY_UNIT_TEST_BOOL(SetProcessDpiAwarenessContext); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetProcessDpiAwarenessContext)( DPI_AWARENESS_CONTEXT value); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT value); static YY_THUNK_FNTYPE_SetProcessDpiAwarenessContext __attribute__((__cdecl__)) try_get_SetProcessDpiAwarenessContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetDpiForSystem
__APPLY_UNIT_TEST_BOOL(GetDpiForSystem); typedef UINT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetDpiForSystem)(void); extern "C" UINT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetDpiForSystem(void); static YY_THUNK_FNTYPE_GetDpiForSystem __attribute__((__cdecl__)) try_get_GetDpiForSystem() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetDpiForWindow
__APPLY_UNIT_TEST_BOOL(GetDpiForWindow); typedef UINT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetDpiForWindow)( HWND _hWnd); extern "C" UINT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetDpiForWindow( HWND _hWnd); static YY_THUNK_FNTYPE_GetDpiForWindow __attribute__((__cdecl__)) try_get_GetDpiForWindow() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetSystemMetricsForDpi
__APPLY_UNIT_TEST_BOOL(GetSystemMetricsForDpi); typedef int(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetSystemMetricsForDpi)( int _nIndex, UINT _uDpi); extern "C" int __attribute__((__stdcall__)) __FALLBACK_PREFIXGetSystemMetricsForDpi( int _nIndex, UINT _uDpi); static YY_THUNK_FNTYPE_GetSystemMetricsForDpi __attribute__((__cdecl__)) try_get_GetSystemMetricsForDpi() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_AdjustWindowRectExForDpi
__APPLY_UNIT_TEST_BOOL(AdjustWindowRectExForDpi); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_AdjustWindowRectExForDpi)( LPRECT _pRect, DWORD _fStyle, BOOL _bMenu, DWORD _fExStyle, UINT _uDpi); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXAdjustWindowRectExForDpi( LPRECT _pRect, DWORD _fStyle, BOOL _bMenu, DWORD _fExStyle, UINT _uDpi); static YY_THUNK_FNTYPE_AdjustWindowRectExForDpi __attribute__((__cdecl__)) try_get_AdjustWindowRectExForDpi() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SystemParametersInfoForDpi
__APPLY_UNIT_TEST_BOOL(SystemParametersInfoForDpi); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SystemParametersInfoForDpi)( UINT _uAction, UINT _uParam, PVOID _pParam, UINT _fWinIni, UINT _uDpi); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSystemParametersInfoForDpi( UINT _uAction, UINT _uParam, PVOID _pParam, UINT _fWinIni, UINT _uDpi); static YY_THUNK_FNTYPE_SystemParametersInfoForDpi __attribute__((__cdecl__)) try_get_SystemParametersInfoForDpi() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetGestureConfig
__APPLY_UNIT_TEST_BOOL(SetGestureConfig); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetGestureConfig)( HWND _hWnd, DWORD _dwReserved, UINT _uDefinitions, PGESTURECONFIG _pGestureConfig, UINT _cbSize); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetGestureConfig( HWND _hWnd, DWORD _dwReserved, UINT _uDefinitions, PGESTURECONFIG _pGestureConfig, UINT _cbSize); static YY_THUNK_FNTYPE_SetGestureConfig __attribute__((__cdecl__)) try_get_SetGestureConfig() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CloseGestureInfoHandle
__APPLY_UNIT_TEST_BOOL(CloseGestureInfoHandle); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CloseGestureInfoHandle)( HGESTUREINFO _hGestureInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXCloseGestureInfoHandle( HGESTUREINFO _hGestureInfo); static YY_THUNK_FNTYPE_CloseGestureInfoHandle __attribute__((__cdecl__)) try_get_CloseGestureInfoHandle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetGestureInfo
__APPLY_UNIT_TEST_BOOL(GetGestureInfo); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetGestureInfo)( HGESTUREINFO _hGestureInfo, PGESTUREINFO _pGestureInfo); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetGestureInfo( HGESTUREINFO _hGestureInfo, PGESTUREINFO _pGestureInfo); static YY_THUNK_FNTYPE_GetGestureInfo __attribute__((__cdecl__)) try_get_GetGestureInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetWindowDisplayAffinity
__APPLY_UNIT_TEST_BOOL(GetWindowDisplayAffinity); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetWindowDisplayAffinity)( HWND _hWnd, DWORD* _pdwAffinity); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetWindowDisplayAffinity( HWND _hWnd, DWORD* _pdwAffinity); static YY_THUNK_FNTYPE_GetWindowDisplayAffinity __attribute__((__cdecl__)) try_get_GetWindowDisplayAffinity() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetWindowDisplayAffinity
__APPLY_UNIT_TEST_BOOL(SetWindowDisplayAffinity); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetWindowDisplayAffinity)( HWND _hWnd, DWORD _dwAffinity); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetWindowDisplayAffinity( HWND _hWnd, DWORD _dwAffinity); static YY_THUNK_FNTYPE_SetWindowDisplayAffinity __attribute__((__cdecl__)) try_get_SetWindowDisplayAffinity() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_RegisterTouchWindow
__APPLY_UNIT_TEST_BOOL(RegisterTouchWindow); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_RegisterTouchWindow)( HWND _hWnd, ULONG _ulFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXRegisterTouchWindow( HWND _hWnd, ULONG _ulFlags); static YY_THUNK_FNTYPE_RegisterTouchWindow __attribute__((__cdecl__)) try_get_RegisterTouchWindow() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_UnregisterTouchWindow
__APPLY_UNIT_TEST_BOOL(UnregisterTouchWindow); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_UnregisterTouchWindow)( HWND _hWnd); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXUnregisterTouchWindow( HWND _hWnd); static YY_THUNK_FNTYPE_UnregisterTouchWindow __attribute__((__cdecl__)) try_get_UnregisterTouchWindow() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_IsTouchWindow
__APPLY_UNIT_TEST_BOOL(IsTouchWindow); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_IsTouchWindow)( HWND _hWnd, PULONG _puFlags); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXIsTouchWindow( HWND _hWnd, PULONG _puFlags); static YY_THUNK_FNTYPE_IsTouchWindow __attribute__((__cdecl__)) try_get_IsTouchWindow() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetTouchInputInfo
__APPLY_UNIT_TEST_BOOL(GetTouchInputInfo); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetTouchInputInfo)( HTOUCHINPUT _hTouchInput, UINT _uInputs, PTOUCHINPUT _pInputs, int _cbSize); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetTouchInputInfo( HTOUCHINPUT _hTouchInput, UINT _uInputs, PTOUCHINPUT _pInputs, int _cbSize); static YY_THUNK_FNTYPE_GetTouchInputInfo __attribute__((__cdecl__)) try_get_GetTouchInputInfo() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CloseTouchInputHandle
__APPLY_UNIT_TEST_BOOL(CloseTouchInputHandle); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CloseTouchInputHandle)( HTOUCHINPUT _hTouchInput); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXCloseTouchInputHandle( HTOUCHINPUT _hTouchInput); static YY_THUNK_FNTYPE_CloseTouchInputHandle __attribute__((__cdecl__)) try_get_CloseTouchInputHandle() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetAwarenessFromDpiAwarenessContext
__APPLY_UNIT_TEST_BOOL(GetAwarenessFromDpiAwarenessContext); typedef DPI_AWARENESS(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetAwarenessFromDpiAwarenessContext)( DPI_AWARENESS_CONTEXT _hValue); extern "C" DPI_AWARENESS __attribute__((__stdcall__)) __FALLBACK_PREFIXGetAwarenessFromDpiAwarenessContext( DPI_AWARENESS_CONTEXT _hValue); static YY_THUNK_FNTYPE_GetAwarenessFromDpiAwarenessContext __attribute__((__cdecl__)) try_get_GetAwarenessFromDpiAwarenessContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_AreDpiAwarenessContextsEqual
__APPLY_UNIT_TEST_BOOL(AreDpiAwarenessContextsEqual); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_AreDpiAwarenessContextsEqual)( DPI_AWARENESS_CONTEXT _hDpiContextA, DPI_AWARENESS_CONTEXT _hDpiContextB); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXAreDpiAwarenessContextsEqual( DPI_AWARENESS_CONTEXT _hDpiContextA, DPI_AWARENESS_CONTEXT _hDpiContextB); static YY_THUNK_FNTYPE_AreDpiAwarenessContextsEqual __attribute__((__cdecl__)) try_get_AreDpiAwarenessContextsEqual() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_EnableNonClientDpiScaling
__APPLY_UNIT_TEST_BOOL(EnableNonClientDpiScaling); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_EnableNonClientDpiScaling)( HWND _hWnd); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXEnableNonClientDpiScaling( HWND _hWnd); static YY_THUNK_FNTYPE_EnableNonClientDpiScaling __attribute__((__cdecl__)) try_get_EnableNonClientDpiScaling() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetThreadDpiAwarenessContext
__APPLY_UNIT_TEST_BOOL(GetThreadDpiAwarenessContext); typedef DPI_AWARENESS_CONTEXT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetThreadDpiAwarenessContext)(); extern "C" DPI_AWARENESS_CONTEXT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetThreadDpiAwarenessContext(); static YY_THUNK_FNTYPE_GetThreadDpiAwarenessContext __attribute__((__cdecl__)) try_get_GetThreadDpiAwarenessContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetWindowDpiAwarenessContext
__APPLY_UNIT_TEST_BOOL(GetWindowDpiAwarenessContext); typedef DPI_AWARENESS_CONTEXT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetWindowDpiAwarenessContext)( HWND _hWnd); extern "C" DPI_AWARENESS_CONTEXT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetWindowDpiAwarenessContext( HWND _hWnd); static YY_THUNK_FNTYPE_GetWindowDpiAwarenessContext __attribute__((__cdecl__)) try_get_GetWindowDpiAwarenessContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetDisplayAutoRotationPreferences
__APPLY_UNIT_TEST_BOOL(SetDisplayAutoRotationPreferences); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetDisplayAutoRotationPreferences)( ORIENTATION_PREFERENCE _eOrientation); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXSetDisplayAutoRotationPreferences( ORIENTATION_PREFERENCE _eOrientation); static YY_THUNK_FNTYPE_SetDisplayAutoRotationPreferences __attribute__((__cdecl__)) try_get_SetDisplayAutoRotationPreferences() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetDisplayAutoRotationPreferences
__APPLY_UNIT_TEST_BOOL(GetDisplayAutoRotationPreferences); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetDisplayAutoRotationPreferences)( ORIENTATION_PREFERENCE* _peOrientation); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXGetDisplayAutoRotationPreferences( ORIENTATION_PREFERENCE* _peOrientation); static YY_THUNK_FNTYPE_GetDisplayAutoRotationPreferences __attribute__((__cdecl__)) try_get_GetDisplayAutoRotationPreferences() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetSystemDpiForProcess
__APPLY_UNIT_TEST_BOOL(GetSystemDpiForProcess); typedef UINT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetSystemDpiForProcess)( HANDLE _hProcess); extern "C" UINT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetSystemDpiForProcess( HANDLE _hProcess); static YY_THUNK_FNTYPE_GetSystemDpiForProcess __attribute__((__cdecl__)) try_get_GetSystemDpiForProcess() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_IsValidDpiAwarenessContext
__APPLY_UNIT_TEST_BOOL(IsValidDpiAwarenessContext); typedef BOOL(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_IsValidDpiAwarenessContext)( DPI_AWARENESS_CONTEXT _eValue); extern "C" BOOL __attribute__((__stdcall__)) __FALLBACK_PREFIXIsValidDpiAwarenessContext( DPI_AWARENESS_CONTEXT _eValue); static YY_THUNK_FNTYPE_IsValidDpiAwarenessContext __attribute__((__cdecl__)) try_get_IsValidDpiAwarenessContext() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetDpiForMonitor
__APPLY_UNIT_TEST_BOOL(GetDpiForMonitor); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetDpiForMonitor)( HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetDpiForMonitor( HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY); static YY_THUNK_FNTYPE_GetDpiForMonitor __attribute__((__cdecl__)) try_get_GetDpiForMonitor() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetProcessDpiAwareness
__APPLY_UNIT_TEST_BOOL(SetProcessDpiAwareness); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetProcessDpiAwareness)( PROCESS_DPI_AWARENESS _eValue); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXSetProcessDpiAwareness( PROCESS_DPI_AWARENESS _eValue); static YY_THUNK_FNTYPE_SetProcessDpiAwareness __attribute__((__cdecl__)) try_get_SetProcessDpiAwareness() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_GetProcessDpiAwareness
__APPLY_UNIT_TEST_BOOL(GetProcessDpiAwareness); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_GetProcessDpiAwareness)( HANDLE _hProcess, PROCESS_DPI_AWARENESS* _peValue); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXGetProcessDpiAwareness( HANDLE _hProcess, PROCESS_DPI_AWARENESS* _peValue); static YY_THUNK_FNTYPE_GetProcessDpiAwareness __attribute__((__cdecl__)) try_get_GetProcessDpiAwareness() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateRandomAccessStreamOverStream
__APPLY_UNIT_TEST_BOOL(CreateRandomAccessStreamOverStream); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateRandomAccessStreamOverStream)( IStream* _pStream, BSOS_OPTIONS _eOptions, const IID & _riid, void** _ppv); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateRandomAccessStreamOverStream( IStream* _pStream, BSOS_OPTIONS _eOptions, const IID & _riid, void** _ppv); static YY_THUNK_FNTYPE_CreateRandomAccessStreamOverStream __attribute__((__cdecl__)) try_get_CreateRandomAccessStreamOverStream() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateRandomAccessStreamOnFile
__APPLY_UNIT_TEST_BOOL(CreateRandomAccessStreamOnFile); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateRandomAccessStreamOnFile)( PCWSTR _szFilePath, DWORD _fAccessMode, const IID & _Id, void** _ppv); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateRandomAccessStreamOnFile( PCWSTR _szFilePath, DWORD _fAccessMode, const IID & _Id, void** _ppv); static YY_THUNK_FNTYPE_CreateRandomAccessStreamOnFile __attribute__((__cdecl__)) try_get_CreateRandomAccessStreamOnFile() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_CreateStreamOverRandomAccessStream
__APPLY_UNIT_TEST_BOOL(CreateStreamOverRandomAccessStream); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_CreateStreamOverRandomAccessStream)( IUnknown* _pRandomAccessStream, const IID & _Id, void**_ppv); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXCreateStreamOverRandomAccessStream( IUnknown* _pRandomAccessStream, const IID & _Id, void**_ppv); static YY_THUNK_FNTYPE_CreateStreamOverRandomAccessStream __attribute__((__cdecl__)) try_get_CreateStreamOverRandomAccessStream() noexcept;
#endif

#if YY_THUNK_GUARD_try_get_SetCurrentProcessExplicitAppUserModelID
__APPLY_UNIT_TEST_BOOL(SetCurrentProcessExplicitAppUserModelID); typedef HRESULT(__attribute__((__stdcall__))* YY_THUNK_FNTYPE_SetCurrentProcessExplicitAppUserModelID)( PCWSTR _szAppID); extern "C" HRESULT __attribute__((__stdcall__)) __FALLBACK_PREFIXSetCurrentProcessExplicitAppUserModelID( PCWSTR _szAppID); static YY_THUNK_FNTYPE_SetCurrentProcessExplicitAppUserModelID __attribute__((__cdecl__)) try_get_SetCurrentProcessExplicitAppUserModelID() noexcept;
#endif
} // namespace YY::Thunks
