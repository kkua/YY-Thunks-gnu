/*
 * yy_thunks_prelude.h —— YY-Thunks MinGW 移植层的"前置垫片"
 *
 * 必须在任何上游 YY-Thunks 头文件之前包含。
 * 本文件只做两件事：
 *   1. 把 MSVC 专有扩展 / CRT 宏映射为 GCC(MinGW-w64) 等价物；
 *   2. 提供无法映射的部分的显式降级实现，并标注其行为差异。
 *
 * 注意：本文件不得定义依赖 <Windows.h> 的内容（那部分见 yy_thunks_postlude.h）。
 */
#pragma once

// Clang 未经验证（移植依赖 GCC 专有的 -fno-toplevel-reorder 语义），主动拒绝。
// Clang 也定义了 __GNUC__，所以必须显式排除。
#if !defined(__GNUC__) || defined(__clang__)
#error "YY-Thunk-gnu 目前仅支持 GCC (MinGW-w64) 工具链"
#endif

/*
 * 上游在 MSVC 下隐式依赖这些标准头（MSVC 的 <Windows.h> 会间接带入）。
 * 放在最前面，确保后续再定义 min/max 宏时不会破坏 STL（libstdc++ 自带 #undef 保护）。
 */
#include <cstdint>
#include <cstddef>
#include <utility>
#include <new>

/* ======================================================================== *
 * 1. MSVC CRT 通用宏
 * ======================================================================== */

#ifndef _CRT_CONCATENATE_
#define _CRT_CONCATENATE_(_Left, _Right) _Left##_Right
#endif

#ifndef _CRT_CONCATENATE
#define _CRT_CONCATENATE(_Left, _Right) _CRT_CONCATENATE_(_Left, _Right)
#endif

#ifndef __CRT_STRINGIZE
#define __CRT_STRINGIZE(_Value) #_Value
#endif

#ifndef _CRT_STRINGIZE_
#define _CRT_STRINGIZE_(_Value) __CRT_STRINGIZE(_Value)
#endif

#ifndef _CRT_STRINGIZE
#define _CRT_STRINGIZE(_Value) _CRT_STRINGIZE_(_Value)
#endif

#ifndef __CRT_WIDE
#define __CRT_WIDE(_String) L##_String
#endif

#ifndef _CRT_WIDE_
#define _CRT_WIDE_(_String) __CRT_WIDE(_String)
#endif

#ifndef _CRT_WIDE
#define _CRT_WIDE(_String) _CRT_WIDE_(_String)
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif

/* __pragma(x) -> _Pragma("x")。GCC 只认后者。 */
#ifndef __pragma
#define __pragma(_Value) _Pragma(#_Value)
#endif

#ifndef __forceinline
#define __forceinline inline __attribute__((__always_inline__))
#endif

#ifndef __analysis_noreturn
/* SAL 的"分析用 noreturn"注解，在 GCC 下没有对应语义，直接丢弃。
 * 注意：不要映射为 __attribute__((noreturn))，因为上游把它用在会正常返回的函数上。 */
#define __analysis_noreturn
#endif

#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof((_Array)[0]))
#endif

/* 注意：不要在这里定义 __debugbreak —— MinGW 的 _mingw.h 已把它声明为函数。 */

/* MSVC CRT 的 ASCII 大小写转换（不依赖 locale，避免死锁，上游有意使用） */
#ifndef __ascii_tolower
#define __ascii_tolower(_Char) ((_Char) >= 'A' && (_Char) <= 'Z' ? (_Char) + ('a' - 'A') : (_Char))
#endif

#ifndef __ascii_towlower
#define __ascii_towlower(_Char) ((_Char) >= L'A' && (_Char) <= L'Z' ? (_Char) + (L'a' - L'A') : (_Char))
#endif

/* MSVC 旧式调用约定别名（MinGW 只提供双下划线版本） */
#ifndef _cdecl
#define _cdecl __cdecl
#endif

#ifndef _stdcall
#define _stdcall __stdcall
#endif

#ifndef _fastcall
#define _fastcall __fastcall
#endif

/* ======================================================================== *
 * 2. MSVC 专有扩展映射
 * ======================================================================== */

/*
 * __declspec(allocate(".Sec")) 在 MinGW 下会退化成未知属性（仅告警、静默失效），
 * 因此上游源码在移植层里统一改写为下面的宏。
 *
 * GNU ld 会把 ".Name$AAA"、".Name$AAB" 按 $ 后缀排序合并成同一个输出节，
 * 与 MSVC 的 /merge 行为一致，所以上游那套"节即数组"的机制可以原样保留。
 *
 * 但必须要加 -fno-toplevel-reorder：否则 GCC 会重排顶层定义，
 * 破坏"节的第一个元素是边界哨兵"这一前提。
 */
#define YY_THUNKS_ALLOCATE(_Section) __attribute__((section(_Section)))

/* 上游用 __foreinclude(x) 防止编译器把 x 优化掉；这里改为一次真实的读取。 */
#ifndef __foreinclude
#define __foreinclude(_p) ((void)(_p))
#endif

/* ======================================================================== *
 * 3. 结构化异常处理（SEH）降级
 * ======================================================================== *
 *
 * GCC/MinGW 不提供 MSVC 的 __try/__except/__finally。
 * 上游使用 SEH 的场景只有两类：
 *   (a) 防御畸形 PE 文件导致的非法访问（解析导出表、运行时数据结构）；
 *   (b) 防御探测性内存读取。
 *
 * 移植层把 __try 体原样执行、把 __except 体变为死代码：
 *   - 正常路径行为与上游 **完全一致**；
 *   - 异常路径不再被捕获，进程会直接崩溃（已记录为已知限制，见 docs/PORTING.md）。
 *
 * 该降级是有意为之：用 VEH 模拟 __except 会引入全局异常回调、不可接受的性能开销
 * 以及和宿主程序异常处理策略冲突的风险，收益不成比例。
 *
 * 实现方式 **不是** 定义 __try/__except 宏 —— 因为 libstdc++ 的
 * bits/exception_defines.h 已经把 __try 用作 `try` 的别名，宏覆盖会破坏 STL。
 * 因此改由 tools/portgen.py 的 "seh" 转换规则做源码级文本改写
 * （见 port/transforms.py），只在确实使用 SEH 的少量文件上生效。
 */

/* ======================================================================== *
 * 4. SAL 注解兜底
 * ======================================================================== *
 * MinGW 的 sal.h 已覆盖大部分注解，这里只补它缺失的少数几个（定义为空）。
 */
#ifndef _In_NLS_string_
#define _In_NLS_string_(_Size)
#endif

#ifndef _VCRT_ALLOCATOR
#define _VCRT_ALLOCATOR
#endif

#ifndef _CRTALLOCATOR
#define _CRTALLOCATOR
#endif

#ifndef _NODISCARD
#if defined(__cplusplus) && __cplusplus >= 201703L
#define _NODISCARD [[nodiscard]]
#else
#define _NODISCARD
#endif
#endif

#ifndef _Ret_maybenull_
#define _Ret_maybenull_
#endif

#ifndef _Ret_notnull_
#define _Ret_notnull_
#endif

#ifndef _Pre_maybenull_
#define _Pre_maybenull_
#endif

#ifndef _Post_invalid_
#define _Post_invalid_
#endif

#ifndef _Post_writable_byte_size_
#define _Post_writable_byte_size_(_Size)
#endif

#ifndef _Outptr_opt_result_z_
#define _Outptr_opt_result_z_
#endif

#ifndef _Out_writes_z_
#define _Out_writes_z_(_Size)
#endif

#ifndef _Out_writes_opt_
#define _Out_writes_opt_(_Size)
#endif

#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_(_Size)
#endif

#ifndef _Reserved_
#define _Reserved_
#endif

#ifndef _Success_
#define _Success_(_Expr)
#endif

#ifndef _Maybe_raises_SEH_exception_
#define _Maybe_raises_SEH_exception_
#endif

/*
 * 旧式 SAL v1 注解（__in / __out / __deref_out ...）与内核态注解。
 * MinGW 的 sal.h 只提供了 v2 形式的一部分，而上游 Shared/km.h 大量使用 v1，
 * 这里统一兜底为"空"，保持声明语义不变。
 */
#ifndef __kernel_entry
#define __kernel_entry
#endif

#ifndef _Return_type_success_
#define _Return_type_success_(_Expr)
#endif

/* 下面这些若 MinGW 已提供（sal.h），其定义同样是空的，重复定义为空不算冲突。 */
#ifndef __in
#define __in
#endif
#ifndef __out
#define __out
#endif
#ifndef __inout
#define __inout
#endif
#ifndef __in_opt
#define __in_opt
#endif
#ifndef __out_opt
#define __out_opt
#endif
#ifndef __inout_opt
#define __inout_opt
#endif
#ifndef __deref_out
#define __deref_out
#endif
#ifndef __deref_opt_out
#define __deref_opt_out
#endif
#ifndef __in_bcount
#define __in_bcount(_Size)
#endif
#ifndef __out_bcount
#define __out_bcount(_Size)
#endif
#ifndef __inout_bcount
#define __inout_bcount(_Size)
#endif
#ifndef __in_ecount
#define __in_ecount(_Size)
#endif
#ifndef __out_ecount
#define __out_ecount(_Size)
#endif
#ifndef __inout_ecount
#define __inout_ecount(_Size)
#endif
#ifndef __success
#define __success(_Expr)
#endif
#ifndef __checkReturn
#define __checkReturn
#endif
#ifndef __reserved
#define __reserved
#endif
#ifndef __nullterminated
#define __nullterminated
#endif
#ifndef __nullnullterminated
#define __nullnullterminated
#endif
#ifndef __possibly_notnullterminated
#define __possibly_notnullterminated
#endif
#ifndef __format_string
#define __format_string
#endif
#ifndef __deref_in
#define __deref_in
#endif
#ifndef __deref_in_opt
#define __deref_in_opt
#endif
#ifndef __deref_inout
#define __deref_inout
#endif
#ifndef __deref_out_opt
#define __deref_out_opt
#endif
#ifndef __post_invalid
#define __post_invalid
#endif
#ifndef __out_data_source
#define __out_data_source(_Source)
#endif
#ifndef __in_data_source
#define __in_data_source(_Source)
#endif
#ifndef __callback
#define __callback
#endif
#ifndef __in_opt_impl
#define __in_opt_impl
#endif
#ifndef __post_maybenull_
#define __post_maybenull_
#endif
#ifndef __pre_notnull_
#define __pre_notnull_
#endif
#ifndef __out_bcount_opt
#define __out_bcount_opt(_Size)
#endif
#ifndef __in_bcount_opt
#define __in_bcount_opt(_Size)
#endif
#ifndef __inout_bcount_opt
#define __inout_bcount_opt(_Size)
#endif
#ifndef __out_ecount_opt
#define __out_ecount_opt(_Size)
#endif
#ifndef __in_ecount_opt
#define __in_ecount_opt(_Size)
#endif
#ifndef __inout_ecount_opt
#define __inout_ecount_opt(_Size)
#endif
#ifndef __out_bcount_part
#define __out_bcount_part(_Size, _Length)
#endif
#ifndef __deref_out_bcount
#define __deref_out_bcount(_Size)
#endif
#ifndef __deref_opt_out_bcount
#define __deref_opt_out_bcount(_Size)
#endif

/* WDK/DDK 专用 SAL 注解：GCC 下没有分析器，统一丢弃。 */
#ifndef __drv_functionClass
#define __drv_functionClass(_Class)
#endif
#ifndef __drv_sameIRQL
#define __drv_sameIRQL
#endif
#ifndef __drv_maxIRQL
#define __drv_maxIRQL(_Irql)
#endif
#ifndef __drv_requiresIRQL
#define __drv_requiresIRQL(_Irql)
#endif
#ifndef __drv_strictTypeMatch
#define __drv_strictTypeMatch(_Mode)
#endif
#ifndef __drv_strictType
#define __drv_strictType(_Type, _Mode)
#endif
#ifndef __drv_aliasesMem
#define __drv_aliasesMem
#endif
#ifndef __drv_arg
#define __drv_arg(_Arg, _Class)
#endif
#ifndef __drv_in
#define __drv_in
#endif
#ifndef __drv_out
#define __drv_out
#endif
#ifndef __drv_allocatesMem
#define __drv_allocatesMem(_Kind)
#endif
#ifndef __drv_freesMem
#define __drv_freesMem(_Kind)
#endif
#ifndef __drv_mustHold
#define __drv_mustHold(_Kind)
#endif
#ifndef __drv_release
#define __drv_release(_Kind)
#endif
#ifndef __drv_when
#define __drv_when(_Cond, _Class)
#endif
#ifndef __drv_valueIs
#define __drv_valueIs(_Values)
#endif
#ifndef __drv_preferredFunction
#define __drv_preferredFunction(_Name)
#endif
#ifndef __drv_ignoreThis
#define __drv_ignoreThis
#endif
#ifndef __drv_clearDoInit
#define __drv_clearDoInit(_Mode)
#endif
#ifndef __drv_deref
#define __drv_deref(_Mode)
#endif
#ifndef __analysis_assume
#define __analysis_assume(_Expr) ((void)0)
#endif
#ifndef __analysis_satisfies
#define __analysis_satisfies(_Expr, _Cond)
#endif

/* ======================================================================== *
 * 5. 移植层自身的可配置开关
 * ======================================================================== */

/*
 * 上游依赖 LibMaker.exe 为 __security_cookie、__acrt_atexit_table 等符号
 * 生成弱引用；移植层改为在 yy_thunks_postlude.h 里提供弱定义。
 */
#define YY_THUNKS_PORT_GW 1

/* ======================================================================== *
 * 6. YY-Thunks 编译模式推导
 * ======================================================================== *
 *
 * 与上游 Thunks/YY_Thunks.cpp 顶部的推导保持一致，这里提前算好，
 * 让"预处理探测"与真正的编译看到完全相同的宏环境。
 * 上游文件里的同名 #ifndef 判断在宏已定义时会被跳过，不冲突。
 */
#include <winapifamily.h>
#include <sdkddkver.h>
#include <SharedDefs.h>

/* ------------------------------------------------------------------------
 * min / max 宏
 *
 * MSVC 的 minwindef.h 在 C++ 下同样定义这两个宏，上游代码直接依赖它们。
 * MinGW 侧有两处差异导致宏缺席：
 *   1. minwindef.h 用 `#ifndef __cplusplus` 把它们关掉；
 *   2. windows.h 默认 `#define NOMINMAX`。
 * 这里按 MSVC 语义补回。libstdc++ 内部会自行 #undef 保护，不会冲突。
 * ---------------------------------------------------------------------- */
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __FALLBACK_PREFIX
#define __FALLBACK_PREFIX
#define __YY_Thunks_libs 0
#else
#define __YY_Thunks_libs 1
#endif

#if !defined(__USING_NTDLL_LIB) && (__YY_Thunks_libs || YY_Thunks_Target >= __WindowsNT10_10240)
// lib 模式下必然存在 ntdll.lib；最小支持 Windows 10 时强制依赖新 SDK，也存在 ntdll.lib。
#define __USING_NTDLL_LIB 1
#endif
