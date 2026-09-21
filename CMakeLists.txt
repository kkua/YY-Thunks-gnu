cmake_minimum_required(VERSION 3.20)

project(YYThunkGnu
    VERSION 1.0.0
    DESCRIPTION "YY-Thunks 的 MinGW-w64 (GCC) 移植版本"
    LANGUAGES CXX)

# ----------------------------------------------------------------------------
# 目录布局
# ----------------------------------------------------------------------------
set(YY_REPO_ROOT        "${CMAKE_CURRENT_SOURCE_DIR}")
set(YY_UPSTREAM_DIR     "${YY_REPO_ROOT}/upstream/YY-Thunks"  CACHE PATH "上游 YY-Thunks 源码目录（只读）")
set(YY_COMPAT_DIR       "${YY_REPO_ROOT}/src/compat")
set(YY_TOOLS_DIR        "${YY_REPO_ROOT}/tools")

# 移植后的源码同步到哪里（Rust crate thunks-gnu 里自带一份，便于独立分发）。
#
# 目录名为什么叫 vendor 而不是 assets：这里放的是**要交给编译器编译的 C/C++ 源码**
# （compat 垫片 + port 生成物，112 个文件），不是运行时加载的数据。
# vendor 的语义是"第三方来源的代码副本，纳入本仓库以便独立构建"，与 cargo vendor
# 一致；叫 assets 容易被理解成"改它不必重新编译"。
#
# 留空即关闭同步。
#
# 注意：这是 CACHE 变量，**改这里的默认值不会覆盖已缓存的旧值**。改名后必须
# 清理构建目录（或显式传 -DYY_THUNKS_GNU_DIST_DIR=...），否则仍会同步到旧路径、
# 把已被移走的目录重新创建出来。
set(YY_THUNKS_GNU_DIST_DIR "${YY_REPO_ROOT}/thunks-gnu/vendor/YY-Thunks-gnu" CACHE PATH
    "移植后源码的发布目录（Rust crate：thunks-gnu/vendor/YY-Thunks-gnu）")

# 生成物放在源码树里，与手写的 src/compat 并列。
#
# 之所以能共用一份：模块清单、覆盖层、守卫表都已**与目标无关** —— 目标差异由
# YY_Thunks_EnableTable.hpp 里的守卫表达式在编译期按 -DYY_Thunks_Target 求值决定。
# 仍与目标相关的少数产物由 portgen 的"生成戳记"保护：若某个构建目录用不同配置
# 重新生成过它，另一个目录构建时会**显式报错**（见下方 yythunks_port_stamp），
# 而不是静默编译到不属于自己的生成物。
set(YY_PORT_DIR         "${YY_REPO_ROOT}/src/port")

# ----------------------------------------------------------------------------
# 可配置项
# ----------------------------------------------------------------------------
# 与上游 YY_Thunks_for_Vista.obj / YY_Thunks_for_Win7.obj 等一一对应。
# 取值形如 __WindowsNT6 / __WindowsNT6_1 / __WindowsNT10_10240 ...
set(YY_THUNKS_TARGET_MACRO "__WindowsNT6" CACHE STRING
    "YY-Thunks 最低兼容系统（对应上游 YY_Thunks_Target 宏）")

# 是否生成带 YY_Thunks_ 前缀的符号（上游 lib/NuGet 模式）。
# 默认关闭：直接提供真实 API 符号，配合静态库使用（等价于上游 obj 模式）。
option(YY_THUNKS_PREFIXED_SYMBOLS "为导出的 thunk 符号加 YY_Thunks_ 前缀" OFF)

option(YY_THUNKS_BUILD_EXAMPLES "构建示例/冒烟测试" ON)

# 让生成目录（src/port）自足：把上游 Thunks/ 与 Shared/ 整个复制进去。
#
# 关闭时只物化"转换规则命中"的文件（17 个），其余靠 include 路径回退到上游读
# 原文 —— 那样**编译必须带着上游源码**（实测会从上游读 72 个头文件，其中还包含
# ThreadRunner.h 这类不在模块清单里的文件）。
#
# 开启后 src/port 成为一棵完整源码树（约 1.9 MB），构建与上游解耦：拿到 src/
# 与 tools/ 就能构建，不必 clone 上游。上游只在"升级 / 重新生成"时需要，
# 其基线由 tools/upstream_lock.py 守护。
option(YY_THUNKS_SELF_CONTAINED "让生成目录自足（编译不再依赖上游源码）" ON)

# ----------------------------------------------------------------------------
# 目标宏校验
#
# 为什么必须校验：预处理器把 `#if` 中未定义的标识符一律当成 0，因此宏名写错
# **不会报错**，只会让 `#if (YY_Thunks_Target < __WindowsNT6_2)` 变成
# `0 < __WindowsNT6_2` —— 恒真，于是**所有** thunk 都被编译进来
# （实测 565 个；而 Vista 目标应只有 259 个）。结果是静默产出一个与预期
# 完全不同的配置，且毫无提示。
#
# 合法取值直接从上游 SharedDefs.h 解析，不手工维护 —— 上游新增 Windows 版本时
# 自动跟上，也不会因为本文件写错而误判。
# ----------------------------------------------------------------------------
# 优先读生成目录里的副本：自足模式下它与上游逐字一致，且在"上游缺失"时仍然
# 可用（正是分发自足想要的）。首次生成时它还不存在，回退到上游。
if(EXISTS "${YY_PORT_DIR}/Shared/SharedDefs.h")
    set(YY_SHAREDDEFS_H "${YY_PORT_DIR}/Shared/SharedDefs.h")
else()
    set(YY_SHAREDDEFS_H "${YY_UPSTREAM_DIR}/src/Shared/SharedDefs.h")
endif()
if(NOT EXISTS "${YY_SHAREDDEFS_H}")
    message(FATAL_ERROR
        "找不到 SharedDefs.h，无法校验 YY_THUNKS_TARGET_MACRO。已尝试：\n"
        "  ${YY_PORT_DIR}/Shared/SharedDefs.h\n"
        "  ${YY_UPSTREAM_DIR}/src/Shared/SharedDefs.h")
endif()
file(STRINGS "${YY_SHAREDDEFS_H}" YY_THUNKS_TARGET_LINES
     REGEX "^#define[ \t]+__WindowsNT[A-Za-z0-9_]*[ \t]+__Version")
set(YY_THUNKS_VALID_TARGETS "")
foreach(YY_TARGET_LINE IN LISTS YY_THUNKS_TARGET_LINES)
    string(REGEX MATCH "__WindowsNT[A-Za-z0-9_]*" YY_TARGET_NAME "${YY_TARGET_LINE}")
    if(YY_TARGET_NAME)
        list(APPEND YY_THUNKS_VALID_TARGETS "${YY_TARGET_NAME}")
    endif()
endforeach()

if(NOT YY_THUNKS_VALID_TARGETS)
    message(FATAL_ERROR
        "无法从上游 SharedDefs.h 解析出任何 __WindowsNT* 版本宏：\n"
        "  ${YY_UPSTREAM_DIR}/src/Shared/SharedDefs.h\n"
        "上游可能改写了这些宏的形式，请更新 CMakeLists.txt 中的解析规则，"
        "否则目标宏校验会形同虚设。")
endif()

if(NOT YY_THUNKS_TARGET_MACRO IN_LIST YY_THUNKS_VALID_TARGETS)
    string(JOIN ", " YY_VALID_TARGETS_TEXT ${YY_THUNKS_VALID_TARGETS})
    message(FATAL_ERROR
        "非法的 YY_THUNKS_TARGET_MACRO：\"${YY_THUNKS_TARGET_MACRO}\"\n"
        "合法取值（解析自上游 SharedDefs.h）：\n  ${YY_VALID_TARGETS_TEXT}\n"
        "另外：这个值写错不会自动报错（#if 中未定义标识符按 0 处理，会导致所有 thunk "
        "都被编译），所以这里做了显式校验。")
endif()
message(STATUS "目标最低兼容系统：YY_Thunks_Target = ${YY_THUNKS_TARGET_MACRO}")

# ----------------------------------------------------------------------------
# 支持范围：低于 __WindowsNT6（Vista）的目标一律拒绝
#
# 为什么不支持 XP/2003：上游在目标 < Vista 时会启用一批"只有那时才需要"的 thunk，
# 而那批代码在 GCC/MinGW-w64 下根本编不过 —— 实测最低目标有 37 个硬错误
# （PEVT_HANDLE 未声明、DWM_E_COMPOSITIONDISABLED 缺失、bcrypt 的显式特化出现在
# 非命名空间作用域、threadpool 的 nRef/VFuncs 未声明……），且与宽容开关无关。
# 与其留一个"填了就编不过"的可选项，不如在这里明确拒绝，并把范围写成事实。
# ----------------------------------------------------------------------------
set(YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO "__WindowsNT6" CACHE STRING
    "本移植支持的最低系统（低于它的目标会被拒绝）")

list(FIND YY_THUNKS_VALID_TARGETS "${YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO}" YY_MIN_SUPPORTED_INDEX)
list(FIND YY_THUNKS_VALID_TARGETS "${YY_THUNKS_TARGET_MACRO}" YY_TARGET_INDEX)

if(YY_MIN_SUPPORTED_INDEX EQUAL -1)
    message(FATAL_ERROR
        "YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO=\"${YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO}\" "
        "不是上游定义的版本宏，请核对 SharedDefs.h。")
endif()

if(YY_TARGET_INDEX LESS YY_MIN_SUPPORTED_INDEX)
    message(FATAL_ERROR
        "YY_THUNKS_TARGET_MACRO=\"${YY_THUNKS_TARGET_MACRO}\" 低于本移植支持的最低系统 "
        "\"${YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO}\"。\n"
        "本移植不支持 Windows XP / Server 2003 目标 —— 那批 target<N6 的 thunk 在\n"
        "GCC/MinGW-w64 下编译不过（详见 CMakeLists.txt 中的说明）。")
endif()
message(STATUS "支持的最低系统：${YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO}")

# ----------------------------------------------------------------------------
# 工具链校验：本移植只针对 GCC(MinGW-w64)
#
# 为什么明确排除 Clang：它**从未被编译/链接/运行验证过**。
# 移植使用的 `-fno-toplevel-reorder` 是 GCC 专有选项，而上游靠"节内首元素是
# 边界哨兵"实现自动注册数组，依赖定义顺序不被重排；Clang 无此选项，其行为需要
# 实测才能确认。与其提供一个未经验证的"支持"，不如在这里明确失败。
# ----------------------------------------------------------------------------
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR
        "YY-Thunk-gnu 目前仅支持 GCC(MinGW-w64) 工具链，"
        "当前编译器为 ${CMAKE_CXX_COMPILER_ID}。\n"
        "（Clang 未经验证；移植依赖 GCC 专有的 -fno-toplevel-reorder 语义。）")
endif()

# ----------------------------------------------------------------------------
# 位宽校验：暂不支持 32 位（i686）
#
# 为什么：上游用 MSVC 的 __identifier 定义 stdcall 修饰过的 IAT 符号：
#     __identifier("_imp__GetCurrentPackageFullName@8")
# GCC 没有 __identifier，而记号拼接（##）也无法拼出含 '@' 的记号 ——
# 实测在 i686 + Vista 下有 1000+ 个编译错误，全部源于此。
# 要支持需在移植层改用汇编 .set 或 -Wl,--defsym 生成等价符号。
#
# 与其让 32 位配置一路走到构建阶段才炸（1000+ 错误），不如在这里说清楚。
# ----------------------------------------------------------------------------
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR
        "YY-Thunk-gnu 暂不支持 32 位（i686）目标。\n"
        "原因：上游用 MSVC 的 __identifier 定义 stdcall 修饰的 IAT 符号"
        "（_imp__Api@N），\n"
        "GCC 无法用 ## 拼出含 '@' 的记号，实测该目标有 1000+ 个编译错误。\n"
        "支持它需要在移植层用汇编 .set 或 -Wl,--defsym 生成等价符号。")
endif()

if(NOT WIN32)
    message(FATAL_ERROR "YY-Thunk-gnu 只能在 Windows 目标上构建。")
endif()

# Python 只在"重新生成覆盖层"和"跑 tools/ 下的检查脚本"时才需要。
#
# 构建本身（configure 复用生成物 + 编译链接）不需要它 —— 分发自足场景下使用者
# 拿到 src/ 用 cmake/ninja 就能构建，不该被迫装 Python。
# 因此这里只在 tools/ 存在时才 REQUIRED；否则找不到也只是跳过相关步骤。
if(EXISTS "${YY_TOOLS_DIR}/portgen.py")
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
else()
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(NOT Python3_Interpreter_FOUND)
        message(STATUS "未找到 Python3 且无 tools/：跳过需要 Python 的步骤。\n"
                       "      构建本身不受影响；重新生成覆盖层时才需要 Python 与上游源码。")
    endif()
endif()

# ----------------------------------------------------------------------------
# 编译选项
#    探测（portgen 预处理）与真正的编译必须使用完全一致的宏环境。
# ----------------------------------------------------------------------------
set(YY_THUNKS_COMMON_DEFS
    "YY_Thunks_Target=${YY_THUNKS_TARGET_MACRO}"
    # 与 MSVC + 现代 Windows SDK 的默认行为对齐：宽宣告（但不影响运行期目标）。
    # 不抬高这两个宏的话，MinGW 头会隐藏 PROCESSOR_NUMBER 等较新类型，
    # 而上游 Shared/km.h 假设它们存在。
    "_WIN32_WINNT=0x0A00"
    "WINVER=0x0A00")

if(YY_THUNKS_PREFIXED_SYMBOLS)
    list(APPEND YY_THUNKS_COMMON_DEFS "__FALLBACK_PREFIX=YY_Thunks_")
endif()

# ----------------------------------------------------------------------------
# portgen 用"最低受支持目标"生成，而不是当前的 YY_THUNKS_TARGET_MACRO
#
# 模块清单、覆盖层、守卫表本就与目标无关。声明遍需要"所有受支持目标可能用到的
# 声明"的并集 —— 而按单调性，该并集恰好等于**最低受支持目标**下的启用集合，
# 所以在它上面探测一次即可，产物供所有目标共用。
# （单调性由 tools/target_matrix.py 在全部合法取值上校验：564/564 通过。）
#
# 注意：这里用"最低受支持"而非"最低合法"。上游最老的取值（XP/2003）本身编不过，
# 若拿它做规范目标，声明遍就会为一个永远不会构建的目标准备内容。
# ----------------------------------------------------------------------------
set(YY_THUNKS_PORTGEN_TARGET_MACRO "${YY_THUNKS_MIN_SUPPORTED_TARGET_MACRO}")
message(STATUS "portgen 生成用目标（最低受支持）：${YY_THUNKS_PORTGEN_TARGET_MACRO}")

set(YY_THUNKS_PORTGEN_DEFS ${YY_THUNKS_COMMON_DEFS})
list(TRANSFORM YY_THUNKS_PORTGEN_DEFS
     REPLACE "^YY_Thunks_Target=.*" "YY_Thunks_Target=${YY_THUNKS_PORTGEN_TARGET_MACRO}")

set(YY_THUNKS_PORTGEN_FLAG_LIST "")
foreach(YY_DEF IN LISTS YY_THUNKS_PORTGEN_DEFS)
    list(APPEND YY_THUNKS_PORTGEN_FLAG_LIST "-D${YY_DEF}")
endforeach()
list(APPEND YY_THUNKS_PORTGEN_FLAG_LIST "-DYY_Thunks_Port=1")
string(JOIN " " YY_THUNKS_PORTGEN_FLAGS ${YY_THUNKS_PORTGEN_FLAG_LIST})

# ----------------------------------------------------------------------------
# 把 portgen 的输入登记为 configure 依赖
#
# portgen 只在 configure 阶段运行。若不同步登记依赖，改完 transforms.py 或
# manifest 后直接 `cmake --build`，会拿着旧的生成物编译且毫无提示。
# 登记之后，改动这些文件会自动触发重新 configure（连带重新生成）。
# ----------------------------------------------------------------------------
set(YY_PORTGEN_INPUTS
    "${YY_REPO_ROOT}/portrule/thunks.manifest"
    "${YY_REPO_ROOT}/portrule/exclude.txt"
    "${YY_TOOLS_DIR}/transforms.py"
    "${YY_TOOLS_DIR}/portgen.py")

set(YY_PORTGEN_EXISTING_INPUTS "")
foreach(YY_INPUT IN LISTS YY_PORTGEN_INPUTS)
    if(EXISTS "${YY_INPUT}")
        list(APPEND YY_PORTGEN_EXISTING_INPUTS "${YY_INPUT}")
    else()
        message(WARNING "portgen 输入文件不存在，无法登记为 configure 依赖：${YY_INPUT}")
    endif()
endforeach()

set_property(DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS ${YY_PORTGEN_EXISTING_INPUTS})

# ----------------------------------------------------------------------------
# 生成覆盖层源码树（位于 ${YY_PORT_DIR}）
#   生成内容：
#     - YY_Thunks_List.hpp         （Thunk 模块清单，等价上游 MSBuild 生成物）
#     - YY_Thunks_EnableTable.hpp  （守卫表达式表，与目标无关）
#     - YY_Thunks_Declarations.hpp （声明遍）
#     - YY_Thunks_Features.hpp     （Fallback 解析器映射）
#     - Thunks/*.{hpp,cpp}         （经过增量转换的上游文件）
#     - Shared/*.h                 （自足模式下为完整副本）
#
# 先尝试复用：戳记一致就跳过生成。
#
# 为什么要能跳过：自足模式下 src/port 是一棵完整源码树，可以随源码树分发。
# 使用者不必 clone 上游 —— configure 检测到戳记匹配即跳过生成，整个构建
# （含编译）完全不碰上游。戳记不匹配（换了目标宏 / 改了 transforms.py /
# 生成物不是自足的…）则回退到"需要上游"的正常流程。
# ----------------------------------------------------------------------------
set(YY_PORTGEN_REPORT "${CMAKE_BINARY_DIR}/portgen-report.json")

if(YY_THUNKS_SELF_CONTAINED)
    set(YY_PORTGEN_SC_ARG --self-contained)
else()
    set(YY_PORTGEN_SC_ARG "")
endif()

# 纯 CMake 判定生成物能否复用 —— 刻意**不调用 Python**。
#
# 为什么：构建（configure + build）不该依赖 tools/。分发自足场景下使用者只拿到
# src/，用 cmake/ninja 就应当能构建。判据全部写在戳记里（cxx_flags /
# self_contained / 逐文件 SHA-256），CMake 靠 file(READ) + string(JSON) +
# file(SHA256) 就能独立判定。
#
# 顺带修掉一个隐患：原先是调用 `portgen.py verify`，而 Python 找不到脚本时的
# 退出码恰好是 2，与"上游漂移"同码 —— 于是"脚本根本没跑起来"会被静默当成漂移
# 放过（实测过：移开 tools/ 后 configure 仍报 Configuring done）。
function(yy_port_is_reusable out_var)
    set(${out_var} FALSE PARENT_SCOPE)
    set(_stamp "${YY_PORT_DIR}/portgen-stamp.json")
    if(NOT EXISTS "${_stamp}")
        return()
    endif()
    file(READ "${_stamp}" _json)

    # 注意：CMake 的 string(JSON) 把 JSON 布尔值映射为 ON / OFF。
    string(JSON _sc ERROR_VARIABLE _e GET "${_json}" self_contained)
    if(_e)
        return()
    endif()
    if(NOT _sc)
        return()
    endif()

    string(JSON _flags ERROR_VARIABLE _e GET "${_json}" cxx_flags)
    if(_e)
        return()
    endif()
    # 戳记里的 cxx_flags 是排序过的（见 portgen.write_stamp）
    string(REPLACE " " ";" _want "${YY_THUNKS_PORTGEN_FLAGS}")
    list(SORT _want)
    string(REPLACE ";" " " _want "${_want}")
    if(NOT _flags STREQUAL _want)
        return()
    endif()

    string(JSON _n ERROR_VARIABLE _e LENGTH "${_json}" inputs)
    if(_e)
        return()
    endif()
    if(_n LESS 1)
        return()
    endif()

    math(EXPR _last "${_n} - 1")
    foreach(_i RANGE ${_last})
        string(JSON _item ERROR_VARIABLE _e GET "${_json}" inputs ${_i})
        if(_e)
            return()
        endif()
        string(FIND "${_item}" ":" _pos)
        if(_pos LESS 1)
            return()
        endif()
        string(SUBSTRING "${_item}" 0 ${_pos} _rel)
        math(EXPR _hp "${_pos} + 1")
        string(SUBSTRING "${_item}" ${_hp} -1 _want_hash)
        if(NOT EXISTS "${YY_REPO_ROOT}/${_rel}")
            # tools/ 下的条目是**可选**输入：分发自足场景下 tools/ 不随包发出，
            # 缺失不算过期（否则生成物在那里永远无法复用）。
            # 其余条目缺失则算过期 —— 输入被删掉同样该重新生成。
            if(NOT _rel MATCHES "^tools/")
                return()
            endif()
        else()
            file(SHA256 "${YY_REPO_ROOT}/${_rel}" _cur_hash)
            if(NOT _cur_hash STREQUAL _want_hash)
                return()
            endif()
        endif()
    endforeach()

    set(${out_var} TRUE PARENT_SCOPE)
endfunction()

set(YY_THUNKS_REUSED_PORT FALSE)
if(YY_THUNKS_SELF_CONTAINED)
    yy_port_is_reusable(YY_THUNKS_REUSED_PORT)
    if(YY_THUNKS_REUSED_PORT)
        message(STATUS "复用已有生成物（戳记匹配，未重新生成）：${YY_PORT_DIR}")
    else()
        message(STATUS "生成物不可复用，将重新生成（需要上游源码与 tools/）")
    endif()
endif()

if(NOT YY_THUNKS_REUSED_PORT)
    if(NOT EXISTS "${YY_UPSTREAM_DIR}/src/Thunks")
        message(FATAL_ERROR
            "需要上游源码才能生成覆盖层，但找不到：${YY_UPSTREAM_DIR}/src/Thunks\n"
            "请 clone 上游（见 docs/UPSTREAM.md），或取得一份已生成的、戳记匹配的 "
            "src/port —— 自足模式下这样即可跳过生成。")
    endif()

    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${YY_TOOLS_DIR}/portgen.py" all
                --root       "${YY_REPO_ROOT}"
                --upstream   "${YY_UPSTREAM_DIR}"
                --out        "${YY_PORT_DIR}"
                --compat     "${YY_COMPAT_DIR}"
                --cxx        "${CMAKE_CXX_COMPILER}"
                --cxx-flags  "${YY_THUNKS_PORTGEN_FLAGS}"
                --report     "${YY_PORTGEN_REPORT}"
                --dist-dir   "${YY_THUNKS_GNU_DIST_DIR}"
                ${YY_PORTGEN_SC_ARG}
        RESULT_VARIABLE YY_PORTGEN_RC
        OUTPUT_VARIABLE YY_PORTGEN_OUT
        ERROR_VARIABLE  YY_PORTGEN_ERR)

    message(STATUS "${YY_PORTGEN_OUT}")

    # 3 = 检测到上游漂移：不中断构建，但必须让使用者看到。
    #
    # 为什么不是 2：Python 自身出错（找不到脚本、语法错误…）的退出码就是 2。
    # 用 2 表示漂移会让"脚本根本没跑起来"被当成漂移静默放过 —— 实测过，
    # 移开 tools/ 后 configure 依然报 Configuring done。
    if(YY_PORTGEN_RC EQUAL 3)
        message(WARNING
            "portgen 检测到上游源码漂移（tools/transforms.py 中的规则未匹配）。\n"
            "${YY_PORTGEN_ERR}\n"
            "请核对上游变更并更新转换规则，否则移植行为可能与上游不一致。")
    elseif(NOT YY_PORTGEN_RC EQUAL 0)
        message(FATAL_ERROR "portgen 执行失败（退出码 ${YY_PORTGEN_RC}）：\n${YY_PORTGEN_ERR}")
    endif()
endif()

set(YY_THUNKS_PORT_SOURCE "${YY_PORT_DIR}/Thunks/YY_Thunks.cpp")
if(NOT EXISTS "${YY_THUNKS_PORT_SOURCE}")
    message(FATAL_ERROR "未生成移植入口翻译单元：${YY_THUNKS_PORT_SOURCE}")
endif()

# 注：把 src/ 同步到 YY_THUNKS_GNU_DIST_DIR 的逻辑**不在**这里，而由 portgen.py
# 在"生成成功之后"执行（--dist-dir）。
#
# 原因：重新生成不止一条路径 —— CMake configure、`yythunks_regen` 目标、手工运行
# portgen.py 都会触发。同步若只写在 configure 分支里，后两条路径会漏掉，
# 生成物更新了而副本还是旧的，且毫无提示。

# ----------------------------------------------------------------------------
# 头文件搜索顺序（覆盖层优先于上游）
# ----------------------------------------------------------------------------
set(YY_THUNKS_INCLUDE_DIRS
    "${YY_COMPAT_DIR}"
    "${YY_PORT_DIR}"
    "${YY_PORT_DIR}/Shared"
    "${YY_PORT_DIR}/Thunks")

# 非自足模式下，未被物化的模块要回退到上游读原文，所以必须把上游放进搜索路径。
#
# 自足模式下**刻意不放上游**：生成目录已是完整副本。一旦放进去了，"生成目录其实
# 缺文件"会被上游悄悄补上 —— 自足性就再也测不出来，等分发出去才炸。
if(NOT YY_THUNKS_SELF_CONTAINED)
    list(APPEND YY_THUNKS_INCLUDE_DIRS
        "${YY_UPSTREAM_DIR}/src"
        "${YY_UPSTREAM_DIR}/src/Thunks"
        "${YY_UPSTREAM_DIR}/src/Shared")
endif()

# ----------------------------------------------------------------------------
# 目标：yythunks（静态库，单个对象文件）
# ----------------------------------------------------------------------------
add_library(yythunks STATIC "${YY_THUNKS_PORT_SOURCE}")
add_library(YYThunks::YYThunks ALIAS yythunks)

# 这些路径只服务于本库自身的编译（对外不需要暴露任何头文件）
target_include_directories(yythunks PRIVATE ${YY_THUNKS_INCLUDE_DIRS})

target_compile_definitions(yythunks PRIVATE ${YY_THUNKS_COMMON_DEFS})
target_compile_features(yythunks PRIVATE cxx_std_17)

# 编译选项抽成变量，供真实目标与下方"探测编译"共用，避免两边漂移导致漏库。
set(YY_THUNKS_COMPILE_OPTIONS
    # 强制前置垫片：把 MSVC 专有扩展映射到 GCC 语义（见 src/compat/yy_thunks_prelude.h）
    -include yy_thunks_prelude.h
    # 上游通过"节的首元素是边界哨兵"来实现自动注册数组，
    # GCC 默认会重排顶层定义，必须关闭。
    -fno-toplevel-reorder
    # 上游会在 SDK 已声明 dllimport 的情况下定义同名函数（这是它的核心手法）
    -Wno-attributes
    -Wno-unknown-pragmas
    # 上游在 <Windows.h> 之后才 include <WinSock2.h>（MSVC 可容忍）
    -Wno-cpp
    # 上游使用 MSVC 的多字符字面量（如 'RSMB'）作为 FourCC
    -Wno-multichar
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-const-variable
    -Wno-missing-field-initializers
    # 上游代码面向 VC6~VS2022，存在大量旧的写法
    -Wno-cast-function-type
    -Wno-invalid-offsetof
    -fno-strict-aliasing
    # 上游存在 MSVC 风格的指针/整型窄化转换（如 (ULONG)NtCurrentTeb()->...）。
    # 这类写法**不靠宽容开关全局放宽** —— 那会把真正的类型错误一起静默掉；
    # 改为在 tools/transforms.py 里逐处显式经 ULONG_PTR 中转（共 3 处）。
    #
    # 实测：修完之后 `-fpermissive`、`-Wno-int-to-pointer-cast`、
    # `-Wno-pointer-to-int-cast` 三者全部可以去掉，类型检查保持完整。
    # 上游是纯 C 风格代码（全仓库没有 try/catch/throw），本身也不会抛异常。
    # 关掉异常后 GCC 不再生成异常清理表，产物就不会引用
    # __gxx_personality_seh0 / _Unwind_Resume —— 这很关键：
    # 本库经常被非 g++ 驱动的工程（Rust/Cargo、自研构建系统）直接链接，
    # 那些工程不会自动带上 GCC 的 C++ 运行时。
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables)

target_compile_options(yythunks PRIVATE ${YY_THUNKS_COMPILE_OPTIONS})

# ----------------------------------------------------------------------------
# 链接需求：上游把要链接的系统库写在 `#pragma comment(lib, "...")` 里，
# 但 GCC 会忽略该指令，因此必须在构建系统里显式声明。
#
# 这里是**唯一事实来源**：
#   - CMake 使用者通过 `target_link_libraries(... yythunks)` 自动继承；
#   - 非 CMake 使用者（Rust/Cargo 的 build.rs、自研构建系统…）可以读取
#     配置阶段生成的 ${YY_LINK_REQUIREMENTS_FILE}。
#
# 最终清单（YY_THUNKS_SYSTEM_LIBS）并非手写，而是两步推导得出（见下方）：
#   1) 先取"任一目标都可能需要的系统库"的并集（手写基础清单 ∪ 上游全部 pragma）
#      作为候选 universe，覆盖所有目标；
#   2) 再以当前 YY_Thunks_Target 把入口翻译单元实际编译，对未定义符号逐一反查
#      universe 中"哪个导入库真正定义它"，只保留确有符号被引用的库。
# 因此结果天然与目标相关、不多不少：被 #if 守卫掉的 pragma 对应的库（如 Vista 下
# 不参与编译的 DirectX/Media 库）自然消失，而 win8.1+ 才需要的 shcore 等又自然出现。
# ----------------------------------------------------------------------------
set(YY_THUNKS_LINK_UNIVERSE
    # 基础
    kernel32 user32 advapi32 shlwapi ws2_32 version ole32 shell32 gdi32
    ntdll setupapi iphlpapi bcrypt crypt32 psapi dbghelp winhttp
    uxtheme dwmapi userenv wevtapi credui powrprof pdh
    bcryptprimitives
    # Known Folder GUID（替代上游 INITKNOWNFOLDERS 手法；目标 < Vista 时需要）
    uuid
    # 各 thunk 兜底分支会直接调用 SDK 函数，需要对应导入库
    dxgi mfplat mfuuid mf mfreadwrite mmdevapi
    d3d9 d3d12 dcomp dxva2 dwrite
    winusb bluetoothapis uiautomationcore propsys esent
    cfgmgr32 dnsapi netapi32)

# 再并入"上游全部 `#pragma comment(lib, ...)`"的并集。
#
# 上游的依赖本身就随目标变化，例如：
#     #if (YY_Thunks_Target >= __WindowsNT6_3)
#     #pragma comment(lib, "Shcore.lib")
#     #endif
# 而 GCC 忽略 `#pragma comment`，只能由链接方提供。上面那份手工清单是按
# Vista 目标整理的，换目标后必然漏项 —— 实际就漏了 shcore，在 Win10 目标下表现为：
#     libyythunks.a(YY_Thunks.cpp.obj): undefined reference to `SetProcessDpiAwareness'
# （Vista 下那几个 DPI thunk 参与编译，调用落到本库，所以看不出问题。）
#
# 改成取上游 pragma 的并集后，依赖集合不再与目标相关，也就不会随目标漂移。
# 多出的库不会带来任何导入项（没有符号被引用），是零成本的。
# 自足模式下扫生成目录：它的 Thunks/ 是上游的完整副本，pragma 集合等价，
# 且在"上游缺失"时依然可得。
if(YY_THUNKS_SELF_CONTAINED)
    file(GLOB_RECURSE YY_PRAGMA_SRCS
         "${YY_PORT_DIR}/*.h"
         "${YY_PORT_DIR}/*.hpp"
         "${YY_PORT_DIR}/*.cpp")
else()
    file(GLOB_RECURSE YY_PRAGMA_SRCS
         "${YY_UPSTREAM_DIR}/src/*.h"
         "${YY_UPSTREAM_DIR}/src/*.hpp"
         "${YY_UPSTREAM_DIR}/src/*.cpp")
endif()
# 注意：file(STRINGS) 只接受单个文件，这里逐个读后汇总
set(YY_UPSTREAM_PRAGMA_LIBS "")
foreach(YY_SRC IN LISTS YY_PRAGMA_SRCS)
    file(STRINGS "${YY_SRC}" YY_SRC_PRAGMA_LIBS
         REGEX "pragma[ \t]+comment[ \t]*\\([ \t]*lib[ \t]*,")
    list(APPEND YY_UPSTREAM_PRAGMA_LIBS ${YY_SRC_PRAGMA_LIBS})
endforeach()

foreach(YY_PRAGMA IN LISTS YY_UPSTREAM_PRAGMA_LIBS)
    if(YY_PRAGMA MATCHES "\"([^\"]+)\"")
        string(TOLOWER "${CMAKE_MATCH_1}" YY_PRAGMA_LIB)
        string(REGEX REPLACE "\\.lib$" "" YY_PRAGMA_LIB "${YY_PRAGMA_LIB}")
        list(APPEND YY_THUNKS_LINK_UNIVERSE "${YY_PRAGMA_LIB}")
    endif()
endforeach()
list(REMOVE_DUPLICATES YY_THUNKS_LINK_UNIVERSE)
list(SORT YY_THUNKS_LINK_UNIVERSE)
list(LENGTH YY_THUNKS_LINK_UNIVERSE YY_THUNKS_LINK_UNIVERSE_COUNT)

# 第 2 步：用当前目标的宏，把入口翻译单元编成临时对象，取未定义符号，再为每个符号
# 在 universe（已排序，保证 cfgmgr32 优先于 setupapi 等）中选第一个真正定义它的导入库。
if(NOT CMAKE_NM)
    get_filename_component(YY_CXX_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_program(CMAKE_NM NAMES nm nm.exe HINTS "${YY_CXX_DIR}")
endif()
if(NOT CMAKE_NM)
    message(FATAL_ERROR "反推链接依赖失败：找不到 nm（binutils）。")
endif()
execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-file-name=libkernel32.a
                OUTPUT_VARIABLE YY_IMPLIB_KERNEL32 OUTPUT_STRIP_TRAILING_WHITESPACE)
get_filename_component(YY_IMPLIB_DIR "${YY_IMPLIB_KERNEL32}" DIRECTORY)
if(NOT IS_DIRECTORY "${YY_IMPLIB_DIR}")
    get_filename_component(YY_CXX_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_path(YY_IMPLIB_DIR NAMES libkernel32.a
              HINTS "${YY_CXX_DIR}/../lib"
                    "${YY_CXX_DIR}/../x86_64-w64-mingw32/lib"
                    "${YY_CXX_DIR}/../../lib"
              NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH)
endif()
if(NOT YY_IMPLIB_DIR OR NOT IS_DIRECTORY "${YY_IMPLIB_DIR}")
    message(FATAL_ERROR "反推链接依赖失败：找不到 MinGW 导入库目录。")
endif()

set(YY_LINK_PROBE_OBJ "${CMAKE_BINARY_DIR}/CMakeFiles/yythunks_link_probe.obj")
set(YY_LINK_PROBE_FLAGS -c -std=c++17 -O0 ${YY_THUNKS_COMPILE_OPTIONS})
foreach(YY_INC IN LISTS YY_THUNKS_INCLUDE_DIRS)
    list(APPEND YY_LINK_PROBE_FLAGS -I "${YY_INC}")
endforeach()
foreach(YY_DEF IN LISTS YY_THUNKS_COMMON_DEFS)
    list(APPEND YY_LINK_PROBE_FLAGS -D "${YY_DEF}")
endforeach()
execute_process(COMMAND "${CMAKE_CXX_COMPILER}" ${YY_LINK_PROBE_FLAGS}
                -o "${YY_LINK_PROBE_OBJ}" "${YY_THUNKS_PORT_SOURCE}"
                RESULT_VARIABLE YY_PROBE_RC
                OUTPUT_VARIABLE YY_PROBE_OUT ERROR_VARIABLE YY_PROBE_ERR)
if(NOT YY_PROBE_RC EQUAL 0)
    message(FATAL_ERROR "反推链接依赖：探测编译失败\n${YY_PROBE_ERR}")
endif()

# 未定义符号（U/w/W 类型）
execute_process(COMMAND "${CMAKE_NM}" -u "${YY_LINK_PROBE_OBJ}"
                OUTPUT_VARIABLE YY_PROBE_NM)
set(YY_NEEDED_SYMS "")
string(REGEX MATCHALL "[^\n]+" YY_PROBE_LINES "${YY_PROBE_NM}")
foreach(YY_LINE ${YY_PROBE_LINES})
    if(YY_LINE MATCHES "[ ][UwW][ ]+([A-Za-z_][A-Za-z0-9_@]*)$")
        list(APPEND YY_NEEDED_SYMS "${CMAKE_MATCH_1}")
    endif()
endforeach()
list(LENGTH YY_NEEDED_SYMS YY_NEEDED_COUNT)

# 每个符号对应第一个（按 universe 排序）真正定义它的导入库
set(YY_THUNKS_SYSTEM_LIBS "")
set(YY_RESOLVED_SYMS "")
foreach(YY_LIB ${YY_THUNKS_LINK_UNIVERSE})
    if(NOT EXISTS "${YY_IMPLIB_DIR}/lib${YY_LIB}.a")
        continue()
    endif()
    execute_process(COMMAND "${CMAKE_NM}" "${YY_IMPLIB_DIR}/lib${YY_LIB}.a"
                    OUTPUT_VARIABLE YY_LIB_NM)
    set(YY_DEFS "")
    string(REGEX MATCHALL "[ ][TtIiDdBbRrWwVv][ ]+[A-Za-z_][A-Za-z0-9_@]*"
           YY_TOKS "${YY_LIB_NM}")
    foreach(YY_TOK ${YY_TOKS})
        if(YY_TOK MATCHES "[ ][TtIiDdBbRrWwVv][ ]+([A-Za-z_][A-Za-z0-9_@]*)")
            list(APPEND YY_DEFS "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    set(YY_ADDED FALSE)
    foreach(YY_SYM ${YY_NEEDED_SYMS})
        if(YY_SYM IN_LIST YY_RESOLVED_SYMS)
            continue()
        endif()
        if(YY_SYM IN_LIST YY_DEFS)
            list(APPEND YY_RESOLVED_SYMS "${YY_SYM}")
            if(NOT YY_ADDED)
                list(APPEND YY_THUNKS_SYSTEM_LIBS "${YY_LIB}")
                set(YY_ADDED TRUE)
            endif()
        endif()
    endforeach()
endforeach()
file(REMOVE "${YY_LINK_PROBE_OBJ}")

list(LENGTH YY_THUNKS_SYSTEM_LIBS YY_THUNKS_SYSTEM_LIBS_COUNT)
list(LENGTH YY_RESOLVED_SYMS YY_RESOLVED_COUNT)
message(STATUS
    "链接依赖库：${YY_THUNKS_SYSTEM_LIBS_COUNT} 个（按实际未定义符号反推，候选 ${YY_THUNKS_LINK_UNIVERSE_COUNT}）；"
    "未定义符号 ${YY_NEEDED_COUNT} 个，已反查解析 ${YY_RESOLVED_COUNT} 个"
    "（其余由编译器/CRT 运行时自动提供）")

target_link_libraries(yythunks PUBLIC ${YY_THUNKS_SYSTEM_LIBS})

# 非 CMake 使用者的链接清单（供 Rust build.rs 等脚本直接使用）
#
# 为什么必须由链接方提供：上游把依赖库写成 `#pragma comment(lib, "bcrypt")`，
# GCC 会忽略该指令。曾尝试用 `.drectve` 节 + `-defaultlib:` 把它嵌进产物，
# 但 GNU ld 明确不支持（实测报 `.drectve '-defaultlib:bcrypt' unrecognized`），
# 因此只能落在构建系统里。
set(YY_LINK_REQUIREMENTS_FILE "${CMAKE_BINARY_DIR}/yythunks-link-requirements.txt")
string(REPLACE ";" " " YY_THUNKS_SYSTEM_LIBS_TEXT "${YY_THUNKS_SYSTEM_LIBS}")
file(WRITE "${YY_LINK_REQUIREMENTS_FILE}"
     "# Link requirements of YY-Thunk-gnu (generated by CMake, ASCII on purpose).\n"
     "# Add -l<name> for every entry in system_libs.\n"
     "system_libs=${YY_THUNKS_SYSTEM_LIBS_TEXT}\n"
     "# GCC runtime (__chkstk_ms, __popcountdi2, ...). The g++ driver adds it\n"
     "# automatically; non-g++ drivers (Rust/Cargo, custom build systems) must\n"
     "# add it explicitly.\n"
     "gcc_runtime_libs=gcc\n")

# 可直接粘贴进 Rust build.rs 的片段
set(YY_CARGO_SNIPPET_FILE "${CMAKE_BINARY_DIR}/yythunks-cargo-libs.rs")
set(_YY_CARGO_LINES "")
foreach(_YY_LIB IN LISTS YY_THUNKS_SYSTEM_LIBS)
    string(APPEND _YY_CARGO_LINES
           "println!(\"cargo:rustc-link-lib=${_YY_LIB}\");\n")
endforeach()
string(APPEND _YY_CARGO_LINES
       "# GCC runtime (__chkstk_ms / __popcountdi2), g++ driver adds it implicitly\n"
       "println!(\"cargo:rustc-link-lib=gcc\");\n")
file(WRITE "${YY_CARGO_SNIPPET_FILE}" "${_YY_CARGO_LINES}")

message(STATUS "已生成链接需求清单：${YY_LINK_REQUIREMENTS_FILE}")
message(STATUS "已生成 Cargo 链接片段：${YY_CARGO_SNIPPET_FILE}")

# ----------------------------------------------------------------------------
# 子目录
# ----------------------------------------------------------------------------
enable_testing()

# ----------------------------------------------------------------------------
# CTest：与上游核对（函数名三层对照）
#
# 为什么放进 CTest：本移植靠"文本转换"改上游代码，最大的风险是**静默改坏** ——
# 转换规则本身不会报错，却可能把 thunk 的名字或行为改掉。真实发生过一次：
# MinGW 的 esent.h 用 __MINGW_NAME_AW 把 JetGetTableColumnInfoW 别名成了 ...A，
# 于是 W 版 thunk 实际定义了 A 入口 —— 而编译、链接、冒烟测试全部正常。
#
# 该用例**独立于转换规则本身**（否则就是用规则检查规则），做三层名字对照：
#     上游源码原文  ->  覆盖层（转换后）  ->  编译器实际展开结果
# 任何一层出现差集即判定失败。文本偏差只报告、不判失败 —— 那是给人逐行审阅的。
#
# 注意：它只读源码与生成物，不依赖任何构建产物，所以注册在顶层而**不是**
# examples/smoke 里 —— 后者在目标 ≥ Win8 时会整体跳过，那会让这项守护跟着消失。
# ----------------------------------------------------------------------------
# 只在有上游时注册：本用例的语义是"与上游核对"，没有上游就无从核对。
# 分发自足的场景（只带 src/、不带上游）下自动不注册 —— 让它失败没有意义，
# 那只会把"没有上游"伪装成一个移植缺陷。
if(EXISTS "${YY_UPSTREAM_DIR}/src/Thunks" AND EXISTS "${YY_TOOLS_DIR}/audit_vs_upstream.py")
    add_test(NAME upstream_audit
             COMMAND "${Python3_EXECUTABLE}" "${YY_TOOLS_DIR}/audit_vs_upstream.py"
                     --root     "${YY_REPO_ROOT}"
                     --upstream "${YY_UPSTREAM_DIR}"
                     --port     "${YY_PORT_DIR}")
else()
    message(STATUS "跳过 upstream_audit 用例"
                   "（需要上游源码与 tools/audit_vs_upstream.py；分发自足场景下不适用）")
endif()

if(YY_THUNKS_BUILD_EXAMPLES)
    add_subdirectory(examples/smoke)
endif()

# ----------------------------------------------------------------------------
# 便捷目标：把 portgen 单独跑一遍（修改了 transforms.py / manifest 之后用）
# ----------------------------------------------------------------------------
# 该目标依赖 Python 与 tools/，因此在"只分发了 src/"的场景下**不注册** ——
# 否则每次 build 都会失败。那种场景下 configure 阶段已用纯 CMake 校验过戳记。
if(EXISTS "${YY_TOOLS_DIR}/portgen.py" AND Python3_Interpreter_FOUND)
    add_custom_target(yythunks_port_stamp ALL
        COMMAND "${Python3_EXECUTABLE}" "${YY_TOOLS_DIR}/portgen.py" verify
                --root "${YY_REPO_ROOT}"
                --out  "${YY_PORT_DIR}"
                --cxx-flags "${YY_THUNKS_PORTGEN_FLAGS}"
                ${YY_PORTGEN_SC_ARG}
        COMMENT "校验 ${YY_PORT_DIR} 与当前构建配置一致"
        VERBATIM)
    add_dependencies(yythunks yythunks_port_stamp)
else()
    message(STATUS "跳过 yythunks_port_stamp 目标（需要 Python3 与 tools/portgen.py）")
endif()

# ----------------------------------------------------------------------------
# 便捷目标：把 portgen 单独跑一遍（修改了 transforms.py / manifest 之后用）
# ----------------------------------------------------------------------------
# 手动同步一份移植后的源码到发布目录（thunks-gnu）。
#
# 用 if 包住而不是让命令自己拼路径：变量为空时 "${...}/src/port" 会退化成
# 绝对路径（Windows 上即 C:/src/port），`rm -rf` 打错地方代价太大。
# 实际复制由 portgen.py 的 dist 子命令完成，与"生成后自动同步"共用同一份逻辑。
if(YY_THUNKS_GNU_DIST_DIR)
    add_custom_target(yythunks_dist
        COMMAND "${Python3_EXECUTABLE}" "${YY_TOOLS_DIR}/portgen.py" dist
                --root     "${YY_REPO_ROOT}"
                --dist-dir "${YY_THUNKS_GNU_DIST_DIR}"
        COMMENT "同步移植源码 -> ${YY_THUNKS_GNU_DIST_DIR}/src"
        VERBATIM)
endif()

add_custom_target(yythunks_regen
    COMMAND "${Python3_EXECUTABLE}" "${YY_TOOLS_DIR}/portgen.py" all
            --root "${YY_REPO_ROOT}"
            --upstream "${YY_UPSTREAM_DIR}"
            --out "${YY_PORT_DIR}"
            --cxx "${CMAKE_CXX_COMPILER}"
            --cxx-flags "${YY_THUNKS_PORTGEN_FLAGS}"
            --dist-dir "${YY_THUNKS_GNU_DIST_DIR}"
            ${YY_PORTGEN_SC_ARG}
    COMMENT "重新生成 YY-Thunks 移植覆盖层"
    VERBATIM)
