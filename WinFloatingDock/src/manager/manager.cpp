#include "manager.h"

namespace WinFloatingDock {
    namespace manager {

        std::vector<std::wstring> g_ignoreList = {
            /*L"Program Manager",
            L"Windows Input Experience"*/
        };
        std::vector<WindowInfo> g_windows = {};

        BOOL IsAltTabWindow(HWND hWnd) {
            //TITLEBARINFO ti;
            //HWND hWndTry, hWndWalk = NULL;

            if (!IsWindowVisible(hWnd))
                return FALSE;

            if (GetWindow(hWnd, GW_OWNER) != NULL)
                return FALSE;

            // There's a hidden Settings window that randomly opens in the background - it has WS_VISIBLE, and all other properties of an active window.
            // it's normally DWM cloaked, but when the settings app is actually activated for real, it tells DWM to decloak itself.
            // the problem is that it's literally uncheckable due to a race condition; when you check it below, it returns 2, but it still triggers
            // WM_OBJECT_SHOW.
            //
            // The fix was to also use WM_OBJECT_UNCLOAK, but that probably increased the number of calls to this function.

            DWORD cloakVal = 0;
            HRESULT hr = DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloakVal, sizeof(cloakVal));
            if (SUCCEEDED(hr) && cloakVal == 2) {
                debug::PrintDetailedWindowAttributes(hWnd);
                return FALSE;
            }

            //hWndTry = GetAncestor(hWnd, GA_ROOTOWNER);
            //while (hWndTry != hWndWalk)
            //{
            //    hWndWalk = hWndTry;
            //    hWndTry = GetLastActivePopup(hWndWalk);
            //    if (IsWindowVisible(hWndTry))
            //        break;
            //}
            //if (hWndWalk != hWnd)
            //    return FALSE;

            LONG style = GetWindowLong(hWnd, GWL_STYLE);
            BOOL hasParent = GetParent(hWnd) != NULL; // check if window has a parent HWND
            bool isChild = (style & WS_CHILD) != 0; // check if window has the WS_CHILD window style
            bool isVisible = (style & WS_VISIBLE) != 0; // check if window has the WS_VISIBLE window style
            if (!isVisible || isChild || hasParent)
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

        int FindWindowInWindowsByHWND(HWND target) {
            auto found = std::find_if(g_windows.begin(), g_windows.end(),
                [&target](const WindowInfo& obj) { return obj.hwnd == target; });

            if (found != g_windows.end()) {
                return std::distance(g_windows.begin(), found);
            }
            return -1;
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

            HICON hIcon = NULL;
            hIcon = HandleIconLoad(hwnd);

            WindowInfo info;
            info.hwnd = hwnd;
            info.hIcon = hIcon;
            info.title = title;

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

            if (FindWindowInWindowsByHWND(hwnd) != -1)
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
            auto found = std::find_if(g_windows.begin(), g_windows.end(),
                [&target](const WindowInfo& obj) { return obj.hwnd == target; });

            if (found != g_windows.end()) {
                g_windows.erase(found);
                return TRUE;
            }
            return FALSE;
        }

        BOOL BringWindowIconForward(HWND target) {
            auto found = std::find_if(g_windows.begin(), g_windows.end(),
                [&target](const WindowInfo& obj) { return obj.hwnd == target; });

            if (found != g_windows.end()) {
                std::rotate(found, found + 1, g_windows.end()); // Move window to the end of the list
                return TRUE;
            }
            return FALSE;
        }

        std::vector<WindowInfo> GetRunningWindowsInfo() {
            std::vector<WindowInfo> windows;
            EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

            // reverse the window list, because EnumWindows starts with the current window, and ends at the last used window
            // we draw icons from most recently used to least recent
            std::reverse(windows.begin(), windows.end());
            g_windows = std::move(windows);
            return g_windows;
        }

        void ActivateWindowFromTaskbar(int index) {
            if (index >= 0 && index < g_windows.size()) {
                HWND hwnd = g_windows[index].hwnd;

                // If minimized, restore it
                if (IsIconic(hwnd))
                    ShowWindow(hwnd, SW_RESTORE);

                // Bring to front
                if (!SetForegroundWindow(hwnd))
                    TryDeleteWindow(hwnd);
            }
        }
    }
}