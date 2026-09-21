/*
 * YY-Thunk-gnu 冒烟测试
 *
 * 目的有两个：
 *   1. 验证静态库能被正确链接，并且确实"顶替"了系统导入库中的同名符号；
 *   2. 验证 thunk 运行期解析链路（延迟初始化 -> 模块加载 -> 函数指针缓存）可用。
 *
 * 之所以能验证 (1)：YY-Thunks 把 SDK 中以 __declspec(dllimport) 声明的函数
 * 定义成了自己的 thunk，如果链接器从 kernel32.dll 的导入库里取了同名桩，
 * __YY_Thunks_Installed 这个标记符号就不会出现在最终映像里。
 */

#include <windows.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <sysinfoapi.h>

#include <cstdio>

extern "C"
{
    // YY_Thunks.h 中导出的 thunk 级别标记（见上游 YY_Thunks.h）
    extern const UINT64 __YY_Thunks_Installed;
}

namespace
{
    int g_failures = 0;

    void Check(bool _bCondition, const char* _szWhat)
    {
        std::printf("[%s] %s\n", _bCondition ? "PASS" : "FAIL", _szWhat);
        if (!_bCondition)
            ++g_failures;
    }

    volatile LONG g_Flag = 0;

    DWORD WINAPI WaitThreadProc(LPVOID)
    {
        LONG expected = 0;
        // 阻塞等待 g_Flag 从 0 变为非 0
        if (!WaitOnAddress((PVOID)&g_Flag, &expected, sizeof(LONG), 5000))
        {
            return 1;
        }
        return g_Flag == 1 ? 0 : 2;
    }
} // namespace

int main()
{
    std::printf("YY-Thunk-gnu 冒烟测试\n");

    // ---- 1. 链接验证：YY-Thunks 必须真的被链接进来 ----
    const UINT64 uThunkLevel = __YY_Thunks_Installed;
    std::printf("      thunk 级别 = %u.%u.%u.%u\n",
                UINT(UINT16(uThunkLevel >> 48)),
                UINT(UINT16(uThunkLevel >> 32)),
                UINT(UINT16(uThunkLevel >> 16)),
                UINT(UINT16(uThunkLevel)));
    Check(uThunkLevel != 0, "YY-Thunks 已链接（__YY_Thunks_Installed 存在）");
    Check((uThunkLevel >> 48) == 6, "thunk 目标版本为 Windows 6.x");

    // ---- 2. Windows 8 起才有的高精度时间 API ----
    FILETIME precise = {};
    FILETIME coarse = {};
    GetSystemTimePreciseAsFileTime(&precise);
    GetSystemTimeAsFileTime(&coarse);
    const ULARGE_INTEGER uPrecise = { precise.dwLowDateTime, precise.dwHighDateTime };
    const ULARGE_INTEGER uCoarse = { coarse.dwLowDateTime, coarse.dwHighDateTime };
    Check(uPrecise.QuadPart != 0, "GetSystemTimePreciseAsFileTime 返回了有效时间");

    // 两次采样之间应当只相差很短的时间（< 1 秒，单位 100ns）
    const ULONGLONG uDelta = uPrecise.QuadPart > uCoarse.QuadPart
                                 ? uPrecise.QuadPart - uCoarse.QuadPart
                                 : uCoarse.QuadPart - uPrecise.QuadPart;
    Check(uDelta < 10000000ull, "高精度时间与系统时间一致");

    // ---- 3. Windows 10 1607 起才有的线程描述 ----
    const wchar_t* szDescription = L"YY-Thunk-gnu Smoke";
    const HRESULT hrSet = SetThreadDescription(GetCurrentThread(), szDescription);
    Check(SUCCEEDED(hrSet), "SetThreadDescription 调用成功");

    PWSTR szReadBack = nullptr;
    const HRESULT hrGet = GetThreadDescription(GetCurrentThread(), &szReadBack);
    Check(SUCCEEDED(hrGet) && szReadBack != nullptr, "GetThreadDescription 调用成功");
    Check(szReadBack != nullptr && wcscmp(szReadBack, szDescription) == 0,
          "线程描述可以原样读回");
    if (szReadBack != nullptr)
    {
        LocalFree(szReadBack);
    }

    // ---- 4. Windows 8 起才有的 WaitOnAddress / WakeByAddress* ----
    // 4.1 比较值不相等时应当立即返回，而不是真的等满超时时间
    volatile LONG value = 1;
    LONG expected = 0;
    const ULONGLONG uBefore = GetTickCount64();
    WaitOnAddress((PVOID)&value, &expected, sizeof(LONG), 1000);
    const ULONGLONG uElapsed = GetTickCount64() - uBefore;
    Check(uElapsed < 200, "WaitOnAddress 在比较值不等时立即返回（未阻塞等待）");

    // 4.2 唤醒语义：另一线程等待，主线程修改值后唤醒
    g_Flag = 0;
    HANDLE hWaiter = CreateThread(nullptr, 0, WaitThreadProc, nullptr, 0, nullptr);
    if (hWaiter != nullptr)
    {
        Sleep(50);
        InterlockedExchange(&g_Flag, 1);
        WakeByAddressAll((PVOID)&g_Flag);
        const DWORD dwWait = WaitForSingleObject(hWaiter, 5000);
        Check(dwWait == WAIT_OBJECT_0, "WakeByAddressAll 成功唤醒等待线程");
        CloseHandle(hWaiter);
    }
    else
    {
        Check(false, "创建等待线程");
    }

    // ---- 5. Windows 7 起才有的处理器组 API（顺带覆盖另一类 thunk）----
    PROCESSOR_NUMBER processorNumber = {};
    GetCurrentProcessorNumberEx(&processorNumber);
    Check(processorNumber.Number < 64, "GetCurrentProcessorNumberEx 返回合理的处理器编号");

    std::printf("\n%s（失败 %d 项）\n", g_failures == 0 ? "全部通过" : "存在失败", g_failures);
    return g_failures == 0 ? 0 : 1;
}
