#include "windowhook.h";

HWINEVENTHOOK g_hWinEventHook = NULL;

HWINEVENTHOOK g_hForegroundHook = NULL;
HWINEVENTHOOK g_hCreateWindowHook = NULL;

// Function to get process name from window handle
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

// Helper function to print window information
void PrintWindowInfo(HWND hwnd, const char* eventType) {
    // Get window title
    wchar_t windowTitle[256] = L"";
    GetWindowText(hwnd, windowTitle, 256);

    // Get process name
    std::wstring processName = GetProcessNameFromWindow(hwnd);

    // Get window class name
    wchar_t className[256] = L"";
    GetClassName(hwnd, className, 256);

    // Get window styles to determine if it's visible
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    bool isVisible = (style & WS_VISIBLE) != 0;

    // Convert from wide string to regular string for console output
    char title[256];
    char process[256];
    char classNameA[256];

    WideCharToMultiByte(CP_ACP, 0, windowTitle, -1, title, 256, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, processName.c_str(), -1, process, 256, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, className, -1, classNameA, 256, NULL, NULL);

    // Print the information
    std::wstringstream ss;
    ss << eventType << L"\n";
    ss << L"  HWND: 0x" << std::hex << (uintptr_t)hwnd << std::dec << L"\n";
    ss << L"  Title: " << title << L"\n";
    ss << L"  Process: " << process << L"\n";
    ss << L"  Class: " << classNameA << L"\n";
    ss << L"  Visible: " << (isVisible ? L"Yes" : L"No") << L"\n";
    ss << L"------------------------------\n";

    
    if (isVisible) {
        OutputDebugStringW(ss.str().c_str());
    }
}

// WinEvent callback procedure
void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
) {
    // We only care about events where the object is a window (OBJID_WINDOW)
    if (idObject != OBJID_WINDOW) return;

    // Skip invalid window handles
    if (!IsWindow(hwnd)) return;

    switch (event) {
    case EVENT_SYSTEM_FOREGROUND: {
        TrySetForeground(hwnd);
        InvalidateRect(g_hWnd, NULL, true);
        //PrintWindowInfo(hwnd, "Foreground window changed to:");
        break;
    }
    case EVENT_OBJECT_SHOW: {
        if (AddWindowSafe(hwnd)) {
            InvalidateRect(g_hWnd, NULL, true);
            PrintWindowInfo(hwnd, "New window possibly created:");
        }
        break;
    }
        
    case EVENT_OBJECT_DESTROY: {
        if (TryDeleteWindow(hwnd)) {
            InvalidateIcons();
        }
        
        break;
    }

    default: {
        break;
    }
    }
}

bool InitHooks() {
    g_hForegroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        NULL,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );

    // Set up the event hook for window creation
    g_hCreateWindowHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY,
        EVENT_OBJECT_SHOW,
        NULL,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!g_hForegroundHook || !g_hCreateWindowHook) {
        std::cout << "Failed to set WinEvent hook. Error: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

void KillHooks() {
    if (g_hForegroundHook) UnhookWinEvent(g_hForegroundHook);
    if (g_hCreateWindowHook) UnhookWinEvent(g_hCreateWindowHook);
}