#include "keyboardhook.h";

static HHOOK g_KeyboardHook;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            switch (pKeyboard->vkCode) {
            case VK_ESCAPE:
                std::vector<WindowInfo> windows = GetRunningWindowsInfo();
                break;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL InitKeyboardHook() {
    g_KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (g_KeyboardHook == NULL) {
        OutputDebugStringW(L"Failed to install hook");
        return false;
    }
    return true;
}