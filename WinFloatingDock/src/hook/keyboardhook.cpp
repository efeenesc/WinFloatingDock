#include "keyboardhook.h"

namespace WinFloatingDock {
    namespace hook {

        HHOOK g_KeyboardHook;

        LRESULT CALLBACK MainKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
            if (nCode == HC_ACTION) {
                KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
                if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                    switch (pKeyboard->vkCode) {
                    case VK_ESCAPE:
                        std::vector<manager::WindowInfo> windows = manager::GetRunningWindowsInfo();
                        break;
                    }
                }
            }
            return CallNextHookEx(NULL, nCode, wParam, lParam); // Call next hook so I don't break the keyboard or anything
        }

        BOOL InitKeyboardHook() {
            g_KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, MainKeyboardProc, NULL, 0);
            if (g_KeyboardHook == NULL) {
                OutputDebugStringW(L"Failed to install hook");
                return false;
            }
            return true;
        }
    }
}