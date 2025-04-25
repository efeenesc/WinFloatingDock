#pragma once
#include <wtypes.h>
#include <string>

namespace WinFloatingDock {
    namespace manager {

        typedef struct WindowInfo {
            HWND hwnd;
            HICON hIcon;
            std::wstring title;
        } WindowInfo;
    }
}