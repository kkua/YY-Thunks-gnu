//! YY-Thunk-gnu 冒烟测试（Rust 版）
//!
//! 与 `examples/smoke/main.cpp` 一一对应，验证两件事：
//!   1. 静态库确实被链接进来，并顶替了系统导入库中的同名符号；
//!   2. thunk 的运行期解析链路可用（延迟初始化 -> 模块加载 -> 函数指针缓存）。
//!
//! 这里手写 `extern "system"` 绑定而不是引入 `windows-sys`，既让示例保持零额外
//! 依赖，也能清楚看到哪些 API 是由 YY-Thunks 顶替的。

// FFI 绑定沿用 Windows SDK 的命名习惯。
#![allow(non_snake_case, non_upper_case_globals)]

use std::ffi::c_void;
use std::process::ExitCode;
use std::sync::atomic::{AtomicI32, Ordering};

type Bool = i32;
type Hresult = i32;
type Handle = *mut c_void;

#[repr(C)]
struct FileTime {
    dw_low_date_time: u32,
    dw_high_date_time: u32,
}

#[repr(C)]
struct ProcessorNumber {
    group: u16,
    number: u8,
    reserved: u8,
}

unsafe extern "C" {
    /// 上游 `YY_Thunks.h` 导出的 thunk 级别标记（C 弱符号，非 0 即表示本库已参与链接）。
    static __YY_Thunks_Installed: u64;
}

unsafe extern "system" {
    // 高精度时间：`GetSystemTimePreciseAsFileTime` 是 Windows 8 才有的 API。
    fn GetSystemTimePreciseAsFileTime(lp_file_time: *mut FileTime);
    fn GetSystemTimeAsFileTime(lp_file_time: *mut FileTime);
    fn GetTickCount64() -> u64;

    // 线程描述：Windows 10 1607 才有的 API。
    fn GetCurrentThread() -> Handle;
    fn SetThreadDescription(h_thread: Handle, description: *const u16) -> Hresult;
    fn GetThreadDescription(h_thread: Handle, description: *mut *mut u16) -> Hresult;
    fn LocalFree(h_mem: *mut c_void) -> *mut c_void;

    // WaitOnAddress / WakeByAddress*：Windows 8 才有的 API。
    fn WaitOnAddress(
        address: *mut c_void,
        compare_address: *const c_void,
        address_size: usize,
        milliseconds: u32,
    ) -> Bool;
    fn WakeByAddressAll(address: *mut c_void);

    // 线程与处理器组。
    fn CreateThread(
        thread_attributes: *mut c_void,
        stack_size: usize,
        start_address: Option<unsafe extern "system" fn(*mut c_void) -> u32>,
        parameter: *mut c_void,
        creation_flags: u32,
        thread_id: *mut u32,
    ) -> Handle;
    fn WaitForSingleObject(h_handle: Handle, milliseconds: u32) -> u32;
    fn CloseHandle(h_object: Handle) -> Bool;
    fn Sleep(milliseconds: u32);
    fn GetCurrentProcessorNumberEx(processor_number: *mut ProcessorNumber);
}

/// 失败计数。用原子类型避免示例里出现 `static mut`。
static FAILURES: AtomicI32 = AtomicI32::new(0);

/// 等待线程与主线程共享的标志。
static FLAG: AtomicI32 = AtomicI32::new(0);

fn check(condition: bool, what: &str) {
    println!("[{}] {}", if condition { "PASS" } else { "FAIL" }, what);
    if !condition {
        FAILURES.fetch_add(1, Ordering::Relaxed);
    }
}

unsafe extern "system" fn wait_thread_proc(_: *mut c_void) -> u32 {
    unsafe {
        let expected: i32 = 0;
        // 阻塞等待 FLAG 从 0 变为非 0，超时 5 秒。
        if WaitOnAddress(
            FLAG.as_ptr() as *mut c_void,
            &expected as *const i32 as *const c_void,
            std::mem::size_of::<i32>(),
            5000,
        ) == 0
        {
            return 1;
        }
        if FLAG.load(Ordering::SeqCst) == 1 { 0 } else { 2 }
    }
}

/// 比较一个以 `\0` 结尾的宽字符串与期望内容（不含结尾 `\0`）。
unsafe fn wide_eq(ptr: *const u16, expected: &[u16]) -> bool {
    if ptr.is_null() {
        return false;
    }
    for (i, &want) in expected.iter().enumerate() {
        if unsafe { *ptr.add(i) } != want {
            return false;
        }
    }
    unsafe { *ptr.add(expected.len()) == 0 }
}

fn main() -> ExitCode {
    println!("YY-Thunk-gnu 冒烟测试（Rust）");
    run_tests();

    let failures = FAILURES.load(Ordering::Relaxed);
    println!(
        "\n{}（失败 {} 项）",
        if failures == 0 { "全部通过" } else { "存在失败" },
        failures
    );
    if failures == 0 {
        ExitCode::SUCCESS
    } else {
        ExitCode::FAILURE
    }
}

fn run_tests() {
    unsafe {
        // ---- 1. 链接验证：YY-Thunks 必须真的被链接进来 ----
        let thunk_level = __YY_Thunks_Installed;
        println!(
            "      thunk 级别 = {}.{}.{}.{}",
            (thunk_level >> 48) as u16,
            (thunk_level >> 32) as u16,
            (thunk_level >> 16) as u16,
            thunk_level as u16
        );
        check(
            thunk_level != 0,
            "YY-Thunks 已链接（__YY_Thunks_Installed 存在）",
        );
        check((thunk_level >> 48) == 6, "thunk 目标版本为 Windows 6.x");

        // ---- 2. Windows 8 起才有的高精度时间 API ----
        let mut precise = FileTime {
            dw_low_date_time: 0,
            dw_high_date_time: 0,
        };
        let mut coarse = FileTime {
            dw_low_date_time: 0,
            dw_high_date_time: 0,
        };
        GetSystemTimePreciseAsFileTime(&mut precise);
        GetSystemTimeAsFileTime(&mut coarse);

        let to_u64 = |ft: &FileTime| {
            ((ft.dw_high_date_time as u64) << 32) | ft.dw_low_date_time as u64
        };
        let precise = to_u64(&precise);
        let coarse = to_u64(&coarse);
        check(precise != 0, "GetSystemTimePreciseAsFileTime 返回了有效时间");
        // 两次采样之间应当只相差很短的时间（< 1 秒，单位 100ns）。
        check(
            precise.abs_diff(coarse) < 10_000_000,
            "高精度时间与系统时间一致",
        );

        // ---- 3. Windows 10 1607 起才有的线程描述 ----
        let description: Vec<u16> = "YY-Thunk-gnu Smoke\0".encode_utf16().collect();
        let hr_set = SetThreadDescription(GetCurrentThread(), description.as_ptr());
        check(hr_set >= 0, "SetThreadDescription 调用成功");

        let mut read_back: *mut u16 = std::ptr::null_mut();
        let hr_get = GetThreadDescription(GetCurrentThread(), &mut read_back);
        check(
            hr_get >= 0 && !read_back.is_null(),
            "GetThreadDescription 调用成功",
        );
        check(
            !read_back.is_null() && wide_eq(read_back, &description[..description.len() - 1]),
            "线程描述可以原样读回",
        );
        if !read_back.is_null() {
            LocalFree(read_back as *mut c_void);
        }

        // ---- 4. Windows 8 起才有的 WaitOnAddress / WakeByAddress* ----
        // 4.1 比较值不相等时应当立即返回，而不是真的等满超时时间。
        let mut value: i32 = 1;
        let expected: i32 = 0;
        let before = GetTickCount64();
        WaitOnAddress(
            &mut value as *mut i32 as *mut c_void,
            &expected as *const i32 as *const c_void,
            std::mem::size_of::<i32>(),
            1000,
        );
        let elapsed = GetTickCount64() - before;
        check(
            elapsed < 200,
            "WaitOnAddress 在比较值不等时立即返回（未阻塞等待）",
        );

        // 4.2 唤醒语义：另一线程等待，主线程修改值后唤醒。
        FLAG.store(0, Ordering::SeqCst);
        let waiter = CreateThread(
            std::ptr::null_mut(),
            0,
            Some(wait_thread_proc),
            std::ptr::null_mut(),
            0,
            std::ptr::null_mut(),
        );
        if !waiter.is_null() {
            Sleep(50);
            FLAG.store(1, Ordering::SeqCst);
            WakeByAddressAll(FLAG.as_ptr() as *mut c_void);
            let waited = WaitForSingleObject(waiter, 5000);
            check(waited == 0, "WakeByAddressAll 成功唤醒等待线程");
            CloseHandle(waiter);
        } else {
            check(false, "创建等待线程");
        }

        // ---- 5. Windows 7 起才有的处理器组 API（顺带覆盖另一类 thunk）----
        let mut processor_number = ProcessorNumber {
            group: 0,
            number: 0,
            reserved: 0,
        };
        GetCurrentProcessorNumberEx(&mut processor_number);
        check(
            processor_number.number < 64,
            "GetCurrentProcessorNumberEx 返回合理的处理器编号",
        );
    }
}
