//! thunks-gnu —— 把 YY-Thunk-gnu（YY-Thunks 的 MinGW-w64 移植）编译进 Rust 工程。
//!
//! 本 crate 是**构建期**辅助库：它不导出任何 Rust 符号，唯一作用是被消费方的
//! `build.rs` 调用，用 [cc] 编译出 `libyythunks.a`，并把链接所需的 `-l` 回传给 Cargo。
//!
//! ```toml
//! # Cargo.toml
//! [build-dependencies]
//! thunks-gnu = "*"
//! ```
//!
//! ```rust,ignore
//! // build.rs
//! fn main() {
//!     thunks_gnu::build();
//! }
//! ```
//!
//! 它是仓库根 `CMakeLists.txt` 中 `add_library(yythunks STATIC ...)` 的等价物：
//! 同一个翻译单元、同一份 include 顺序、同一组宏与编译开关（见下方 [`build`]）。
//! 目标版本由 Cargo feature 选择（`vista`/`win7`/`win8`/`win10_10240`/`win10_19041`），
//! 对应上游的 `YY_Thunks_Target`，含义是**希望支持的最低系统**。

use std::env;
use std::path::PathBuf;

/// vendored 的移植层源码树（`vendor/YY-Thunks-gnu/src`，自包含：
/// `compat/` 是手写垫片，`port/` 是 portgen 产物 + 上游头文件），相对 crate 根目录。
const SOURCE_ROOT: &[&str] = &["vendor", "YY-Thunks-gnu", "src"];
/// 唯一的翻译单元，等价 CMake 的 `YY_THUNKS_PORT_SOURCE`。
const ENTRY_SOURCE: &[&str] = &["port", "Thunks", "YY_Thunks.cpp"];
/// 产出的静态库名（最终文件为 libyythunks.a）。
const LIB_NAME: &str = "yythunks";

/// 编译 vendored 的 YY-Thunks 并把链接指令回传给 Cargo。
///
/// 由消费方的 `build.rs` 调用；非 windows-gnu 目标只打印一条提示后返回。
pub fn build() {
    let target_os = env::var("CARGO_CFG_TARGET_OS").unwrap();
    let target_env = env::var("CARGO_CFG_TARGET_ENV").unwrap();

    if target_os != "windows" || target_env != "gnu" {
        println!("cargo::warning=Skipped! Only Windows(GNU) is supported!");
        return;
    }

    // 32 位不支持的原因：上游用 MSVC 的 __identifier 定义 stdcall 修饰的 IAT 符号
    // （_imp__Api@N），GCC 无法用 ## 拼出含 '@' 的记号（实测 1000+ 编译错误）。
    // 与其让它一路走到编译阶段才炸 1000+ 错误，不如在这里说清楚。
    let target_arch = env::var("CARGO_CFG_TARGET_ARCH").unwrap_or_default();
    if target_arch != "x86_64" {
        panic!(
            "thunks-gnu: 仅支持 x86_64-windows-gnu，当前目标是 {target_arch}-windows-gnu。\n\
             i686 需要先用汇编 .set 或 -Wl,--defsym 生成等价的 IAT 符号。"
        );
    }

    // CARGO_MANIFEST_DIR 在**编译本 crate** 时展开，因此这里拿到的一定是 thunks-gnu
    // 自己的目录，与调用方 build.rs 的工作目录无关。
    let crate_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let source_root = join_path(crate_dir, SOURCE_ROOT);
    let entry_source = join_path(source_root.clone(), ENTRY_SOURCE);

    if !entry_source.is_file() {
        panic!(
            "thunks-gnu: 找不到移植层入口翻译单元：{}\n\
             请确认 vendor/YY-Thunks-gnu/src 完整（compat/ + port/ 两棵子树都要有）。",
            entry_source.display()
        );
    }

    let thunk_target = thunk_target();

    // 是否把编译器的消息转发给 Cargo（默认：不转发，见下方编译选项处的说明）。
    // 由 feature = "show_warnings" 或环境变量 THUNKS_GNU_SHOW_WARNINGS=1 开启。
    let show_warnings =
        cfg!(feature = "show_warnings") || env::var_os("THUNKS_GNU_SHOW_WARNINGS").is_some();

    let mut build = cc::Build::new();

    build
        // 纯 C++（上游无 STL 容器，但要 c++17 的 constexpr/decltype 语义）
        .cpp(true)
        .flag_if_supported("-std=c++17")
        // 告警开关（feature = "show_warnings" 或 THUNKS_GNU_SHOW_WARNINGS=1 打开）：
        //   - 默认关闭：cc 默认会加 -Wall -Wextra，而 CMake 侧一个都不加 —— 这才是
        //     Rust 侧告警远多于 CMake 的原因（-Wswitch、-Wimplicit-fallthrough、
        //     -Wsubobject-linkage…）；两者都关掉时 cc 会改传 -w，连
        //     `__YY_Thunks_libs redefined` 这种"GCC 默认开启且没有 -Wno- 开关"
        //     的告警也一并静默（Clang 才有 -Wno-macro-redefined）。
        //   - 打开后恢复 -Wall -Wextra，用于排查。
        .warnings(show_warnings)
        .extra_warnings(show_warnings)
        // 不生成调试信息：CMake 侧没设 CMAKE_BUILD_TYPE，既无 -O 也无 -g；而 cc 会
        // 按 Cargo profile 加 -g -gdwarf-2（dev 下 DEBUG=1），这一个开关就让目标文件
        // 从 500KB 涨到 1.54MB —— 之前"Rust 版更大"的主因就在这里。
        .debug(false)
        // 不做优化：CMake 同样从不给 port 加 -O。实测开启 -O3（release profile）会破坏
        // YY-Thunks 的静态注册（全局构造在 main 之前就 ACCESS VIOLATION），所以强制 -O0，
        // 与上游构建行为完全一致。port 是极薄的转发层，不优化也无性能损失。
        .opt_level(0)
        // 搜索路径沿用 CMake 的 YY_THUNKS_INCLUDE_DIRS：compat（手写垫片）最前，
        // 随后是 port 根（"Shared/…"、"Thunks\…" 这类相对写法在此命中）、
        // port/Shared、port/Thunks。
        .include(source_root.join("compat"))
        .include(source_root.join("port"))
        .include(source_root.join("port").join("Shared"))
        .include(source_root.join("port").join("Thunks"))
        // 目标：希望支持的最低系统
        .define("YY_Thunks_Target", Some(thunk_target))
        // 与 MSVC + 现代 Windows SDK 的默认行为对齐：宽宣告（不影响运行期目标）。
        // 不抬高这两个宏的话，MinGW 头会隐藏 PROCESSOR_NUMBER 等较新类型，
        // 而上游 Shared/km.h 假设它们存在。
        .define("_WIN32_WINNT", Some("0x0A00"))
        .define("WINVER", Some("0x0A00"));

    // 说明：CMake 侧还有 YY_THUNKS_PREFIXED_SYMBOLS（__FALLBACK_PREFIX=YY_Thunks_，
    // 即"导出符号带前缀"的 lib 模式）与 __USING_NTDLL_LIB 两个开关。当前 Cargo.toml
    // 没有对应的 feature，故此处保持与 CMake 默认值一致（都关）；需要的话加一个
    // feature 再 .define(...) 即可。

    // 与 CMake target_compile_options 逐项对齐：
    build
        // 强制前置垫片：把 MSVC 专有扩展映射到 GCC 语义（src/compat/yy_thunks_prelude.h）
        .flag("-include")
        .flag("yy_thunks_prelude.h")
        // 上游通过"节的首元素是边界哨兵"实现自动注册数组，GCC 默认会重排顶层定义，必须关闭
        .flag("-fno-toplevel-reorder")
        // 上游会在 SDK 已声明 dllimport 的情况下定义同名函数（这是它的核心手法）
        .flag("-Wno-attributes")
        // GCC 忽略 #pragma comment(linker, "/merge:.YYThu=.data")，不必报错
        .flag("-Wno-unknown-pragmas")
        // 上游在 <Windows.h> 之后才 include <WinSock2.h>（MSVC 可容忍）
        .flag("-Wno-cpp")
        // 上游使用 MSVC 的多字符字面量（如 'RSMB'）作为 FourCC
        .flag("-Wno-multichar")
        .flag("-Wno-unused-parameter")
        .flag("-Wno-unused-variable")
        .flag("-Wno-unused-function")
        .flag("-Wno-unused-const-variable")
        .flag("-Wno-missing-field-initializers")
        // 上游代码面向 VC6~VS2022，存在大量旧写法
        .flag("-Wno-cast-function-type")
        .flag("-Wno-invalid-offsetof")
        .flag("-fno-strict-aliasing")
        // 上游是纯 C 风格代码（无 try/catch/throw）。关掉异常后 GCC 不再生成
        // 异常清理表，产物就不会引用 __gxx_personality_seh0 / _Unwind_Resume ——
        // 这对本 crate 很关键：链接方往往是 Rust/Cargo，它不会带 GCC 的 C++ 运行时。
        .flag("-fno-exceptions")
        .flag("-fno-unwind-tables")
        .flag("-fno-asynchronous-unwind-tables")
        // cc 会无条件加 -ffunction-sections -fdata-sections（再涨约 240KB）。对
        // "--gc-sections" 才有意义，而本库是单个目标文件整包参与链接，拿不到那点收益。
        // 用户旗标排在 cc 默认旗标之后，可以正常覆盖掉。
        .flag("-fno-function-sections")
        .flag("-fno-data-sections");

    let compiler = build.get_compiler();
    if !compiler.is_like_gnu() {
        panic!(
            "thunks-gnu: 检测到非 GNU 工具链（{}），不支持该工具链。\n\
             移植依赖 GCC 专有的 -fno-toplevel-reorder：上游靠“节内首元素是边界哨兵”",
            compiler.path().display()
        );
    }

    build.file(&entry_source);

    // 编译器告警已被 -w 抑制，但上游还有一批**故意留下**的提示：__WarningMessage__
    // 展开成 `#pragma message`，它不受 -w 影响，cc 仍会逐条转发成 cargo:warning。
    // 这里一并关掉；编译失败时 cc 依旧会打印完整命令与错误输出，不影响排错。
    // 需要排查时开启 feature = "show_warnings" 或设 THUNKS_GNU_SHOW_WARNINGS=1（同时恢复 -Wall -Wextra）。
    build.cargo_warnings(show_warnings);

    // 显式声明重跑条件：一旦打印了 rerun-if-changed，Cargo 就不再使用
    // "包内任何文件变动都重跑"的默认行为，所以要把整棵 vendor 源码树列上
    // （Cargo 对目录是递归监听的）。
    println!("cargo:rerun-if-changed={}", source_root.display());
    println!("cargo:rerun-if-env-changed=CXX");
    println!("cargo:rerun-if-env-changed=THUNKS_GNU_SHOW_WARNINGS");

    if show_warnings {
        println!(
            "cargo::warning=thunks-gnu: 编译 lib{LIB_NAME}.a（YY_Thunks_Target={thunk_target}）…"
        );
    }

    // 产出 $OUT_DIR/libyythunks.a，并由 cc 自动打印
    // cargo:rustc-link-search=native=<OUT_DIR> 与 cargo:rustc-link-lib=static=yythunks。
    build.compile(LIB_NAME);

    // 兜底再给一次搜索路径（cc 的输出目录策略若变化也不会失效）：
    // `-L` 指向不存在的目录是无害的。
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
    println!("cargo:rustc-link-search=native={}", out_dir.display());
    // 链接顺序是关键：yythunks 必须排在系统导入库之前，链接器才会优先取 YY-Thunks
    // 的符号；下面的清单按这个要求依次输出（顺序颠倒会报 multiple definition，
    // 不会静默失去覆盖）。
    println!("cargo:rustc-link-lib=static=yythunks");
    
    println!("cargo:rustc-link-lib=static=yythunks");
    println!("cargo:rustc-link-lib=kernel32"); //
    println!("cargo:rustc-link-lib=user32"); //
    println!("cargo:rustc-link-lib=ole32"); //
    println!("cargo:rustc-link-lib=shcore"); // win10
    println!("cargo:rustc-link-lib=advapi32"); //
    println!("cargo:rustc-link-lib=gdi32"); //
    println!("cargo:rustc-link-lib=uuid"); //
    println!("cargo:rustc-link-lib=ntdll"); //
    println!("cargo:rustc-link-lib=iphlpapi"); //
    println!("cargo:rustc-link-lib=bcrypt"); //
    println!("cargo:rustc-link-lib=powrprof"); //
    println!("cargo:rustc-link-lib=winhttp"); //
    println!("cargo:rustc-link-lib=dxgi"); //
    println!("cargo:rustc-link-lib=cfgmgr32"); //
    
    // 上面这份清单是 CMake 汇总"上游全部 #pragma comment(lib, ...)"的结果：
    // GCC 会忽略该指令，所以系统导入库必须由链接方提供。Rust 目标自带
}

/// 由 Cargo feature 决定上游的 `YY_Thunks_Target`，即**希望支持的最低系统**。
///
/// 对应关系与 CMake 的 `YY_THUNKS_TARGET_MACRO` 一致；同时打开多个 feature 时取最高目标。
fn thunk_target() -> &'static str {
    if cfg!(feature = "win10_19041") {
        "__WindowsNT10_19041"
    } else if cfg!(feature = "win10_10240") {
        "__WindowsNT10_10240"
    } else if cfg!(feature = "win8") {
        "__WindowsNT6_2"
    } else if cfg!(feature = "win7") {
        "__WindowsNT6_1"
    } else {
        "__WindowsNT6" // vista，默认
    }
}

fn join_path(base: PathBuf, parts: &[&str]) -> PathBuf {
    parts.iter().fold(base, |acc, part| acc.join(part))
}
