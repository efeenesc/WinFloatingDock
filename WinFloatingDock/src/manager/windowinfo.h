#pragma once
#include <wtypes.h>
#include <string>

typedef struct WindowInfo {
    HWND hwnd;
    HICON hIcon;
    std::wstring title;
} WindowInfo;