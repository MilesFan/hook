#include <windows.h>
#include <stdio.h>

void ShowLastError()
{
    LPSTR lpMsgBuf;
    DWORD dw = GetLastError();
    DWORD _Lang_id;
    const int _Ret = GetLocaleInfoEx(L"en-US", LOCALE_ILANGUAGE | LOCALE_RETURN_NUMBER,
        (LPWSTR)&_Lang_id, sizeof(_Lang_id) / sizeof(wchar_t));

    if (FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        _Lang_id,
        (LPSTR)&lpMsgBuf,
        0,
        NULL
    ) == 0)
    {
        printf("Error %d: %s", dw, "Unknown Error");
    }
    else
    {
        printf("Error %d: %s\n", dw, (char*)lpMsgBuf);
    }

    LocalFree(lpMsgBuf);
}

DWORD GetTargetProcessAndThreadId(const char* exeName) {

    HWND hwnd = FindWindow(NULL, L"WebViewDemo");
    //HWND hwnd = FindWindowA("Notepad", NULL);
    if (hwnd == 0) return 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hwnd, NULL);
    return dwThreadId;
}

int main(int argc, char* argv[]) {

    HWND hwnd = FindWindowA(NULL, "WebViewDemo");
    if (hwnd == 0) {
        printf("target not found\n");
        system("pause");
        return -1;
    }
    else
    {
        printf("target hwnd: %08llX\n", (uintptr_t)hwnd);
    }
    DWORD threadId = GetWindowThreadProcessId(hwnd, NULL);
    if (threadId == 0) {
        printf("target not found\n");
        system("pause");
        return -1;
    }
    printf("target threadId: %d\n", threadId);

    const HMODULE hmodule = LoadLibraryA("HookDll.dll");
    if (hmodule == NULL) {
        printf("LoadLibraryA failed\n");
        ShowLastError();
        system("pause");
        return -1;
    }

    const HOOKPROC hookCallWndProcAdd = (HOOKPROC)GetProcAddress(hmodule, "CallWndProc");

    if (hookCallWndProcAdd == NULL) {
        printf("Could not find hookCallWndProcAdd function\n");
        ShowLastError();
        system("pause");
        return -1;
    }

    //const REGCUSTMSG_FUNC RegisterCustomMessage = (REGCUSTMSG_FUNC)GetProcAddress(hmodule, "RegisterCustomeMessage");

    //if (RegisterCustomMessage == NULL) {
    //    printf("Could not find RegisterCustomMessage function\n");
    //    ShowLastError();
    //    system("pause");
    //    return -1;
    //}
    //const UINT msgId = RegisterCustomMessage();
    //printf("msgId: %04X\n", msgId);

    const HHOOK hookWndProc = SetWindowsHookEx(WH_CALLWNDPROC, hookCallWndProcAdd, hmodule, threadId);
    if (hookWndProc == NULL) {
        printf("SetWindowsHookEx WndProc failed\n");
        ShowLastError();
        system("pause");
        return -1;
    }

    printf("Hook installed. Press enter to remove hook and exit.\n");

    printf("Looping to send message to target window, press ESC to terminate...\n");
    while (1) {
        LRESULT ret = SendMessage(hwnd, WM_USER + 100, 0, 0);
        printf("msg sent, return %08X\n", (long)ret);
        Sleep(1000);
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("ESC pressed. Exiting...\n");
            break;
        }
    }

    if (hmodule != NULL) FreeLibrary(hmodule);
    UnhookWindowsHookEx(hookWndProc);

    return 0;
}