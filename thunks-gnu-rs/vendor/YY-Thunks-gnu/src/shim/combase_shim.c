/*
 * combase -> ole32 转发垫片
 * ============================================================================
 *
 * 问题
 * ----
 * 新版 Windows SDK / win32metadata 把 CoCreateFreeThreadedMarshaler、
 * CoTaskMemFree 归到 combase.dll（Win8 起才存在的实现 DLL）。Rust 侧
 * `#[link(name = "combase", kind = "raw-dylib")]` 会因此生成 combase.dll 的
 * **静态导入描述符**，而在 Windows 7 上：
 *
 *     无法启动此程序，因为计算机中丢失 combase.dll。
 *
 * 这属于 DLL 级失败（加载器按 DLL 名解析），函数级 thunk 够不到 ——
 * YY-Thunks 也没有覆盖这两个 API。
 *
 * 而这两个导出在 Win7 上都在 ole32.dll 里（见上游版本导出库
 * YY.Depends.Analyzer/Config/x64/6.1.7600.txt：[ole32.dll] 28=...），
 * Win8+ 的 ole32.dll 仍保留同名转发导出。所以只要运行时从 ole32.dll 取即可。
 *
 * 为什么必须同时定义函数体与 IAT 槽
 * ---------------------------------
 * rustc 在 *-windows-gnu 下把 raw-dylib 的导入库**整个塞进使用方的 rlib**，
 * 成员形如 combase.dlls00000.o（一个 DLL 一个成员），其中同时定义 X 与
 * __imp_X。该成员按需提取：
 *
 *   - 只定义 __imp_X  -> 调用点引用的 X 仍未定义 -> 成员被拉出 ->
 *                        multiple definition of `__imp_X`，combase 描述符照样生成
 *   - 同时定义 X 与 __imp_X -> 成员无符号可满足 -> 不被提取 ->
 *                        combase.dll 从导入表彻底消失（实测确认）
 *
 * 两个坑
 * ------
 * 1. 必须先把 DECLSPEC_IMPORT 定义成空，否则 MinGW 的 WINOLEAPI 展开成
 *    __declspec(dllimport)，与这里的定义冲突（"definition of dllimport
 *    function not allowed" / conflicting types）。
 * 2. 签名照头文件抄：CoCreateFreeThreadedMarshaler 只有两个参数
 *    (LPUNKNOWN, LPUNKNOWN*)，不是 REFIID/void** 那种三参数形态。
 *
 * 为什么用 extern "C" 包起来
 * --------------------------
 * 本文件与 YY_Thunks.cpp 共用同一套 cc 编译参数：g++ 驱动 + 强制
 * `-include yy_thunks_prelude.h`（该垫片含 <cstdint>/<new>，是 C++ 专用），
 * 因此 .c 实际按 C++ 编译。不加大括号里的 extern "C"，四个符号会被 C++ 修饰，
 * 既对不上消费方的引用，也无法让 combase 导入对象“无符号可满足”。
 *
 * 前置条件
 * --------
 * 本文件必须**无条件**进入链接（由 build_yythunks 编进 libyythunks.a，
 * 该库以 +whole-archive 参与链接）。一旦被裁剪，符号又变回未定义，
 * combase 的导入描述符就会回来。
 */

/* 见“两个坑”第 1 条 */
#define DECLSPEC_IMPORT

#include <windows.h>

/* 见“为什么用 extern "C" 包起来” */
#ifdef __cplusplus
extern "C"
{
#endif

static HMODULE g_ole32;
static HRESULT(WINAPI* g_cftm)(LPUNKNOWN, LPUNKNOWN*);
static void(WINAPI* g_free)(LPVOID);

/* ole32.dll 是 KnownDLL，正常已在进程内；极端情况下补一次 LoadLibrary。 */
static HMODULE load_ole32(void)
{
    if (!g_ole32)
    {
        g_ole32 = GetModuleHandleW(L"ole32.dll");
        if (!g_ole32)
            g_ole32 = LoadLibraryW(L"ole32.dll");
    }
    return g_ole32;
}

static void* resolve(const char* name)
{
    HMODULE module = load_ole32();
    return module ? (void*)GetProcAddress(module, name) : NULL;
}

HRESULT WINAPI CoCreateFreeThreadedMarshaler(LPUNKNOWN punkOuter, LPUNKNOWN* ppunkMarshal)
{
    if (!g_cftm)
        g_cftm = (HRESULT(WINAPI*)(LPUNKNOWN, LPUNKNOWN*))resolve("CoCreateFreeThreadedMarshaler");
    return g_cftm ? g_cftm(punkOuter, ppunkMarshal) : E_NOINTERFACE;
}

void WINAPI CoTaskMemFree(LPVOID p)
{
    if (!g_free)
        g_free = (void(WINAPI*)(LPVOID))resolve("CoTaskMemFree");
    if (g_free)
        g_free(p);
}

/* 关键：函数体与 IAT 槽都要定义，见“为什么必须同时定义”一节。
 *
 * 必须写 `extern`：本文件按 C++ 编译，而 C++ 里命名空间作用域的 `const` 变量
 * 默认是**内部链接**（C 里是外部链接）。少了它，这两个符号会变成局部符号、
 * 随后被优化掉，消费方的引用依旧未定义 —— 结果是“编过了但 combase 还在”。
 * `extern` + 初始化器在 C/C++ 里都仍是定义，两种模式都正确。 */
extern void const* const __imp_CoCreateFreeThreadedMarshaler = (void const*)&CoCreateFreeThreadedMarshaler;
extern void const* const __imp_CoTaskMemFree = (void const*)&CoTaskMemFree;

#ifdef __cplusplus
} /* extern "C" */
#endif
