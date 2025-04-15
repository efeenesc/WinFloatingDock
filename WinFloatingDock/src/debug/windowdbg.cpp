#include "windowdbg.h"

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
    ss << L"  Title: " << title << L"\n";
    ss << L"  Process: " << process << L"\n";
    ss << L"  Class: " << classNameA << L"\n";
    ss << L"  Visible: " << (isVisible ? L"Yes" : L"No") << L"\n";
    ss << L"------------------------------\n";


    if (strcmp(process, "explorer.exe") != 0) {
        OutputDebugStringW(ss.str().c_str());
    }
}