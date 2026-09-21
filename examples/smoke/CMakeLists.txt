# 冒烟测试：验证 YY-Thunk-gnu 能被正确链接，且 thunk 运行期可用。
#
# 链接顺序很关键：yythunks 必须排在系统导入库之前，
# 这样链接器才会优先从 YY-Thunks 取出符号，而不是从 kernel32/user32 的
# 导入库里取同名桩函数。（GCC 驱动默认把系统库放在最后，天然满足。）

# 本例断言的是"本库为**低版本**目标兜底的 API"。当目标本身已不低于这些 API 的
# 引入版本时，本库按设计**不再提供**它们（系统已原生具备），示例必然链接失败。
# 这种配置下直接跳过，免得留下一个注定构建不过的 preset。
list(FIND YY_THUNKS_VALID_TARGETS "__WindowsNT6_2" _yy_win8_index)
list(FIND YY_THUNKS_VALID_TARGETS "${YY_THUNKS_TARGET_MACRO}" _yy_target_index)
if(NOT _yy_target_index LESS _yy_win8_index)
    message(STATUS
        "目标 ${YY_THUNKS_TARGET_MACRO} 不低于 Windows 8：本库不再为示例断言的 API "
        "提供兜底（符合设计），跳过 examples/smoke")
    return()
endif()

add_executable(yythunks_smoke main.cpp)

target_compile_features(yythunks_smoke PRIVATE cxx_std_17)

# 让 SDK 头文件声明我们要测试的较新 API（Win8 / Win10 1607），
# 否则编译器看不到原型，就无法验证"SDK 声明 + YY 定义"的共存关系。
target_compile_definitions(yythunks_smoke PRIVATE
    _WIN32_WINNT=0x0A00
    WINVER=0x0A00)

target_link_libraries(yythunks_smoke PRIVATE yythunks)

add_test(NAME smoke COMMAND yythunks_smoke)

# 回归守护：产物不得静态导入"可能不存在"的 DLL。
# 详见 tools/check_static_imports.py 与 docs/PORTING.md 的"静态导入"一节。
# 依赖 Python 与 tools/；只分发 src/ 的场景下不注册（构建本身不需要它们）。
if(EXISTS "${YY_TOOLS_DIR}/check_static_imports.py" AND Python3_Interpreter_FOUND)
    add_test(NAME static_imports
             COMMAND "${Python3_EXECUTABLE}"
                     "${YY_TOOLS_DIR}/check_static_imports.py"
                     "$<TARGET_FILE:yythunks_smoke>")
else()
    message(STATUS "跳过 static_imports 用例（需要 Python3 与 tools/check_static_imports.py）")
endif()
