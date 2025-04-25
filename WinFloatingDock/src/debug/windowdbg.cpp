#include "windowdbg.h"

namespace debug {

    std::wstring GetProcessNameFromWindow(HWND hwnd) {
        if (!hwnd) return L"<unknown>";

        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess) {
            wchar_t processName[MAX_PATH] = L"<unknown>";

            if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
                // Extract just the filename from the path
                wchar_t* lastSlash = wcsrchr(processName, L'\\');
                if (lastSlash) {
                    // Move pointer to character after the last slash
                    memmove(processName, lastSlash + 1, (wcslen(lastSlash + 1) + 1) * sizeof(wchar_t));
                }
            }

            CloseHandle(hProcess);
            return processName;
        }

        return L"<unknown>";
    }

    void PrintWindowInfo(HWND hwnd) {
        // Get window title
        wchar_t windowTitle[256] = L"";
        GetWindowText(hwnd, windowTitle, 256);

        // Get window class name
        wchar_t className[256] = L"";
        GetClassName(hwnd, className, 256);

        // Get window styles to determine if it's visible
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        bool isVisible = (style & WS_VISIBLE) != 0;

        // Convert from wide string to regular string for console output
        char title[256];
        char classNameA[256];

        WideCharToMultiByte(CP_ACP, 0, windowTitle, -1, title, 256, NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, className, -1, classNameA, 256, NULL, NULL);

        // Print the information
        std::wstringstream ss;
        ss << L"  Title: " << title << L"\n";
        ss << L"  Class: " << classNameA << L"\n";
        ss << L"  Visible: " << (isVisible ? L"Yes" : L"No") << L"\n";
        ss << L"------------------------------\n";

        OutputDebugStringW(ss.str().c_str());
    }

    void PrintDetailedWindowAttributes(HWND hwnd) {
        wchar_t debugBuffer[1024];

        // Get basic window information
        wchar_t className[256] = L"";
        GetClassNameW(hwnd, className, 256);

        wchar_t windowText[256] = L"";
        GetWindowTextW(hwnd, windowText, 256);

        // Get various style flags
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

        // Get process and thread ID
        DWORD processId = 0;
        DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);

        // Get parent and owner windows
        HWND parentHwnd = GetParent(hwnd);
        HWND ownerHwnd = GetWindow(hwnd, GW_OWNER);
        HWND rootOwner = GetAncestor(hwnd, GA_ROOTOWNER);

        // Print all this information
        swprintf(debugBuffer, 1024, L"Window Handle: 0x%p\n", hwnd);
        OutputDebugString(debugBuffer);

        swprintf(debugBuffer, 1024, L"Class Name: %s\n", className);
        OutputDebugString(debugBuffer);

        swprintf(debugBuffer, 1024, L"Window Text: %s\n", windowText);
        OutputDebugString(debugBuffer);

        swprintf(debugBuffer, 1024, L"Process ID: %lu\n", processId);
        OutputDebugString(debugBuffer);

        swprintf(debugBuffer, 1024, L"Thread ID: %lu\n", threadId);
        OutputDebugString(debugBuffer);

        // Print style flags in hex and decode important ones
        swprintf(debugBuffer, 1024, L"Style: 0x%08X\n", style);
        OutputDebugString(debugBuffer);

        OutputDebugString(L"Style Flags:\n");
        if (style & WS_OVERLAPPED) OutputDebugString(L"  WS_OVERLAPPED\n");
        if (style & WS_POPUP) OutputDebugString(L"  WS_POPUP\n");
        if (style & WS_CHILD) OutputDebugString(L"  WS_CHILD\n");
        if (style & WS_MINIMIZE) OutputDebugString(L"  WS_MINIMIZE\n");
        if (style & WS_VISIBLE) OutputDebugString(L"  WS_VISIBLE\n");
        if (style & WS_DISABLED) OutputDebugString(L"  WS_DISABLED\n");
        if (style & WS_CLIPSIBLINGS) OutputDebugString(L"  WS_CLIPSIBLINGS\n");
        if (style & WS_CLIPCHILDREN) OutputDebugString(L"  WS_CLIPCHILDREN\n");
        if (style & WS_MAXIMIZE) OutputDebugString(L"  WS_MAXIMIZE\n");
        if (style & WS_CAPTION) OutputDebugString(L"  WS_CAPTION\n");
        if (style & WS_BORDER) OutputDebugString(L"  WS_BORDER\n");
        if (style & WS_DLGFRAME) OutputDebugString(L"  WS_DLGFRAME\n");
        if (style & WS_VSCROLL) OutputDebugString(L"  WS_VSCROLL\n");
        if (style & WS_HSCROLL) OutputDebugString(L"  WS_HSCROLL\n");
        if (style & WS_SYSMENU) OutputDebugString(L"  WS_SYSMENU\n");
        if (style & WS_THICKFRAME) OutputDebugString(L"  WS_THICKFRAME\n");
        if (style & WS_GROUP) OutputDebugString(L"  WS_GROUP\n");
        if (style & WS_TABSTOP) OutputDebugString(L"  WS_TABSTOP\n");

        // Print extended style flags
        swprintf(debugBuffer, 1024, L"Extended Style: 0x%08X\n", exStyle);
        OutputDebugString(debugBuffer);

        OutputDebugString(L"Extended Style Flags:\n");
        if (exStyle & WS_EX_DLGMODALFRAME) OutputDebugString(L"  WS_EX_DLGMODALFRAME\n");
        if (exStyle & WS_EX_NOPARENTNOTIFY) OutputDebugString(L"  WS_EX_NOPARENTNOTIFY\n");
        if (exStyle & WS_EX_TOPMOST) OutputDebugString(L"  WS_EX_TOPMOST\n");
        if (exStyle & WS_EX_ACCEPTFILES) OutputDebugString(L"  WS_EX_ACCEPTFILES\n");
        if (exStyle & WS_EX_TRANSPARENT) OutputDebugString(L"  WS_EX_TRANSPARENT\n");
        if (exStyle & WS_EX_MDICHILD) OutputDebugString(L"  WS_EX_MDICHILD\n");
        if (exStyle & WS_EX_TOOLWINDOW) OutputDebugString(L"  WS_EX_TOOLWINDOW\n");
        if (exStyle & WS_EX_WINDOWEDGE) OutputDebugString(L"  WS_EX_WINDOWEDGE\n");
        if (exStyle & WS_EX_CLIENTEDGE) OutputDebugString(L"  WS_EX_CLIENTEDGE\n");
        if (exStyle & WS_EX_CONTEXTHELP) OutputDebugString(L"  WS_EX_CONTEXTHELP\n");
        if (exStyle & WS_EX_RIGHT) OutputDebugString(L"  WS_EX_RIGHT\n");
        if (exStyle & WS_EX_RTLREADING) OutputDebugString(L"  WS_EX_RTLREADING\n");
        if (exStyle & WS_EX_LEFTSCROLLBAR) OutputDebugString(L"  WS_EX_LEFTSCROLLBAR\n");
        if (exStyle & WS_EX_CONTROLPARENT) OutputDebugString(L"  WS_EX_CONTROLPARENT\n");
        if (exStyle & WS_EX_STATICEDGE) OutputDebugString(L"  WS_EX_STATICEDGE\n");
        if (exStyle & WS_EX_APPWINDOW) OutputDebugString(L"  WS_EX_APPWINDOW\n");
        if (exStyle & WS_EX_LAYERED) OutputDebugString(L"  WS_EX_LAYERED\n");
        if (exStyle & WS_EX_NOINHERITLAYOUT) OutputDebugString(L"  WS_EX_NOINHERITLAYOUT\n");
        if (exStyle & WS_EX_NOACTIVATE) OutputDebugString(L"  WS_EX_NOACTIVATE\n");
        if (exStyle & WS_EX_COMPOSITED) OutputDebugString(L"  WS_EX_COMPOSITED\n");

        // Show window state
        const wchar_t* showCmdStr = L"Unknown";
        switch (placement.showCmd) {
        case SW_HIDE: showCmdStr = L"SW_HIDE"; break;
        case SW_SHOWNORMAL: showCmdStr = L"SW_SHOWNORMAL"; break;
        case SW_SHOWMINIMIZED: showCmdStr = L"SW_SHOWMINIMIZED"; break;
        case SW_SHOWMAXIMIZED: showCmdStr = L"SW_SHOWMAXIMIZED"; break;
        case SW_SHOWNOACTIVATE: showCmdStr = L"SW_SHOWNOACTIVATE"; break;
        case SW_SHOW: showCmdStr = L"SW_SHOW"; break;
        case SW_MINIMIZE: showCmdStr = L"SW_MINIMIZE"; break;
        case SW_SHOWMINNOACTIVE: showCmdStr = L"SW_SHOWMINNOACTIVE"; break;
        case SW_SHOWNA: showCmdStr = L"SW_SHOWNA"; break;
        case SW_RESTORE: showCmdStr = L"SW_RESTORE"; break;
        case SW_SHOWDEFAULT: showCmdStr = L"SW_SHOWDEFAULT"; break;
        }
        swprintf(debugBuffer, 1024, L"Show Command: %s\n", showCmdStr);
        OutputDebugString(debugBuffer);

        // Window relationships
        swprintf(debugBuffer, 1024, L"Parent Window: 0x%p\n", parentHwnd);
        OutputDebugString(debugBuffer);

        swprintf(debugBuffer, 1024, L"Owner Window: 0x%p\n", ownerHwnd);
        OutputDebugString(debugBuffer);

        swprintf(debugBuffer, 1024, L"Root Owner: 0x%p\n", rootOwner);
        OutputDebugString(debugBuffer);

        BOOL isWindowEnabled = IsWindowEnabled(hwnd);

        swprintf(debugBuffer, 1024, L"Is Window Enabled: %s\n", isWindowEnabled ? L"Yes" : L"No");
        OutputDebugString(debugBuffer);

        // Check cloaking status (Windows 8+ feature that can hide windows)
        DWORD cloakVal = 0;
        const DWORD DWMWA_CLOAKED = 14; // DWM window attribute for cloaking
        HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloakVal, sizeof(cloakVal));
        if (SUCCEEDED(hr)) {
            swprintf(debugBuffer, 1024, L"DWM Cloaked: %lu (0=No, 1=Temporary, 2=From target app, 4=From owner)\n", cloakVal);
            OutputDebugString(debugBuffer);
        }

        OutputDebugString(L"----------------------------------------\n");
    }
}