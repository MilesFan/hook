#include <windows.h>
#include <stdio.h>
DWORD procId = 0;
HWND  hwnd_found = 0;


__declspec(dllexport) LRESULT CALLBACK CallWndProc(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    PCWPSTRUCT pcs = (PCWPSTRUCT)lParam;
    if (pcs->message == WM_USER + 100)
    {
        printf("CallWndProc -> hwnd=%08X , msg=%08X, WPARAM=%08X, LPARAM=%08X\n", (long)pcs->hwnd, (long)pcs->message, (long)pcs->wParam, (long)(long)pcs->lParam);
        return 0;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Callback function to find a window by its instance handle
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    if (!IsWindowVisible(hwnd)) return TRUE;
    DWORD _procId;
    GetWindowThreadProcessId(hwnd, &_procId);
    if (_procId == procId)
    {
        hwnd_found = hwnd; // Found the window, store the handle
        return FALSE; // Stop enumeration
    }
    return TRUE; // Continue enumeration
}
HWND FindWindowByInstance(HINSTANCE hInstance)
{
    procId = GetCurrentProcessId();
    EnumWindows(EnumWindowsProc, 0);
    return hwnd_found;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
    LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        //char text[100];
        //sprintf_s(&text[0], 100, "fdwReason = %d", fdwReason);
        //OutputDebugStringA(text);

        HWND hwnd = FindWindowByInstance(hinstDLL);
        printf("DllMain->hwnd = %08X\n", (long)hwnd);
        //sprintf_s(&text[0], 100, "hwnd = %08llX", (uintptr_t)hwnd_found);
        //OutputDebugStringA(text);
    }
    return TRUE;
}
