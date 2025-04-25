#include "shellicon.h"

namespace WinFloatingDock {
    namespace shell {

        BOOL InitShellIcon(HWND hWnd, HICON hIcon) {
            NOTIFYICONDATA d = { 0 };

            wchar_t title[14] = L"Floating Dock";
            d.cbSize = sizeof(NOTIFYICONDATA);
            d.hWnd = hWnd;
            d.hIcon = hIcon;
            d.uID = 1;
            d.uVersion = NOTIFYICON_VERSION_4;
            d.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            d.uCallbackMessage = FLOATINGDOCK_TRAYICON_MSG;
            wcsncpy_s(d.szTip, title, 127);

            return Shell_NotifyIcon(NIM_ADD, &d);
        }
    }
}