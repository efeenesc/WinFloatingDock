#include "manager.h";

std::vector<std::wstring> g_ignoreList = {
    L"Program Manager",
    L"Windows Input Experience"
};
std::vector<WindowInfo> g_windows = {};

BOOL IsAltTabWindow(HWND hWnd)
{
    TITLEBARINFO ti;
    HWND hWndTry, hWndWalk = NULL;

    if (!IsWindowVisible(hWnd))
        return FALSE;

    /*if (GetWindow(hWnd, GW_OWNER) != NULL)
        return FALSE;*/

    hWndTry = GetAncestor(hWnd, GA_ROOTOWNER);
    while (hWndTry != hWndWalk)
    {
        hWndWalk = hWndTry;
        hWndTry = GetLastActivePopup(hWndWalk);
        if (IsWindowVisible(hWndTry))
            break;
    }
    if (hWndWalk != hWnd)
        return FALSE;

    // the following removes some task tray programs and "Program Manager"
    ti.cbSize = sizeof(ti);
    GetTitleBarInfo(hWnd, &ti);

    // If STATE_SYSTEM_INVISIBLE is true
    if (ti.rgstate[0] & STATE_SYSTEM_INVISIBLE)
        return FALSE;

    // Tool windows should not be displayed either, these do not appear in the
    // task bar.
    if (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
        return FALSE;

    return TRUE;
}

std::wstring GetProcessPathFromHWND(HWND hWnd) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProc) return L"";

    wchar_t path[MAX_PATH] = { 0 };
    DWORD size = MAX_PATH;
    QueryFullProcessImageName(hProc, 0, path, &size);
    CloseHandle(hProc);

    return path;
}

BOOL CheckWindowInIgnoreList(std::wstring title) {
    for (int i = 0; i < g_ignoreList.size(); i++) {
        if (title.compare(g_ignoreList[i]) == 0)
            return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    std::vector<WindowInfo>* windowList = reinterpret_cast<std::vector<WindowInfo>*>(lParam);

    if (!IsAltTabWindow(hwnd))
        return TRUE;

    // Skip windows without titles
    wchar_t title[256];
    if (GetWindowTextW(hwnd, title, 256) == 0)
        return TRUE;

    if (CheckWindowInIgnoreList(std::wstring(title)))
        return TRUE;

    // Get window icon - try multiple methods
    HICON hIcon = NULL;

    // Method 1: Send WM_GETICON message
    hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    if (!hIcon)
        hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);

    // Method 2: Get class icon if WM_GETICON failed
    if (!hIcon)
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    if (!hIcon)
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);

    if (!hIcon) {
        std::wstring exePath = GetProcessPathFromHWND(hwnd);
        HICON hLarge[1] = { 0 };
        if (!exePath.empty()) {
            ExtractIconExW(exePath.c_str(), 0, hLarge, NULL, 1);
            hIcon = hLarge[0];
        }
    }

    // Method 3: Use default application icon if all else fails
    if (!hIcon)
        hIcon = LoadIcon(NULL, IDI_APPLICATION);

    // Add window info to our list
    WindowInfo info;
    info.hwnd = hwnd;
    info.hIcon = hIcon;
    info.title = title;

    std::wstringstream ss;
    ss << info.title << std::endl;

    OutputDebugString(ss.str().c_str());

    windowList->push_back(info);

    return TRUE;
}

HICON HandleIconLoad(HWND hwnd) {
    HICON hIcon = NULL;

    // Method 1: Send WM_GETICON message
    hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    if (!hIcon)
        hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);

    // Method 2: Get class icon if WM_GETICON failed
    if (!hIcon)
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    if (!hIcon)
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);

    if (!hIcon) {
        std::wstring exePath = GetProcessPathFromHWND(hwnd);
        HICON hLarge[1] = { 0 };
        if (!exePath.empty()) {
            ExtractIconExW(exePath.c_str(), 0, hLarge, NULL, 1);
            hIcon = hLarge[0];
        }
    }

    // Method 3: Use default application icon if all else fails
    if (!hIcon)
        hIcon = LoadIcon(NULL, IDI_APPLICATION);

    return hIcon;
}

BOOL AddWindowSafe(HWND hwnd) {
    if (!IsAltTabWindow(hwnd))
        return FALSE;

    wchar_t title[256];
    if (GetWindowTextW(hwnd, title, 256) == 0)
        return FALSE;

    if (CheckWindowInIgnoreList(std::wstring(title)))
        return FALSE;

    HICON hIcon = HandleIconLoad(hwnd);
    
    // Add window info to our list
    WindowInfo info;
    info.hwnd = hwnd;
    info.hIcon = hIcon;
    info.title = title;

    g_windows.push_back(info);
    return TRUE;
}

BOOL TryDeleteWindow(HWND target) {
    auto found = std::find_if(g_windows.begin(), g_windows.end(), [&target](const WindowInfo& obj) { return obj.hwnd == target; });

    if (found != g_windows.end()) {
        g_windows.erase(found);
        return TRUE;
    }
    return FALSE;
}

BOOL TrySetForeground(HWND target) {
    auto found = std::find_if(g_windows.begin(), g_windows.end(), [&target](const WindowInfo& obj) { return obj.hwnd == target; });

    if (found != g_windows.end()) {
        std::rotate(g_windows.begin(), found, found + 1);
        return TRUE;
    }
    return FALSE;
}

std::vector<WindowInfo> GetRunningWindowsInfo() {
    std::vector<WindowInfo> windows;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));
    g_windows = windows;
    return g_windows;
}