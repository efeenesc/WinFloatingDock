#include "main.h"

#define MAX_LOADSTRING 100

using namespace WinFloatingDock;

HINSTANCE hInst;                                        // Current instance
WCHAR szTitle[MAX_LOADSTRING] = L"Floating Dock";       // Title bar text
WCHAR szWindowClass[MAX_LOADSTRING];                    // Main window class name
static bool isMouseInWindow = false;                    // Whether mouse is in window. This might need to be moved to window.h
timer::BasicTimer* CLOCK_TIMER = nullptr;
UINT_PTR CLOCK_ID = 200;
UINT_PTR MOUSEHOVER_TIMER_ID = 100;
HCURSOR HAND_CURSOR;
HCURSOR ARROW_CURSOR;
LPWSTR ShownCursor = IDC_ARROW;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ARROW_CURSOR = LoadCursor(nullptr, IDC_ARROW);
    HAND_CURSOR = LoadCursor(nullptr, IDC_HAND);
    LoadStringW(hInstance, IDC_WINFLOATINGDOCK, szWindowClass, MAX_LOADSTRING);
    RegisterProgramClass(hInstance);
    

    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    if (!hook::InitHooks())
        return FALSE;

    if (!hook::InitKeyboardHook())
        return FALSE;

    if (!shell::InitShellIcon(window::g_hWnd, window::g_wndIcon))
        return FALSE;

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

ATOM RegisterProgramClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    window::g_wndIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINFLOATINGDOCK));
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINFLOATINGDOCK));
    wcex.hCursor        = ARROW_CURSOR;
    wcex.hbrBackground  = CreateSolidBrush(RGB(51, 51, 51));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINFLOATINGDOCK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    manager:: GetRunningWindowsInfo();
    hInst = hInstance; // Store instance handle in our global variable

    window::g_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED, 
        szWindowClass, 
        szTitle, 
        WS_VISIBLE | WS_POPUP | WS_BORDER,
        50, // X pos
        50, // Y pos
        400, // width
        55, // height
        nullptr, // parent hWnd
        nullptr, // menu hWnd
        hInstance, 
        nullptr
    );

    if (!window::g_hWnd) {
        return FALSE;
    }

    SetLayeredWindowAttributes(window::g_hWnd, RGB(0, 0, 0), 100, LWA_ALPHA);
    StartClockTimer();

    ShowWindow(window::g_hWnd, nCmdShow);
    window:: g_wndShown = TRUE;
    UpdateWindow(window::g_hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        if (hWnd) {
            DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
            DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

            
            //StartClockTimer();
        }
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        window::Draw(hdc, manager::g_windows, ps.rcPaint);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN: {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        POINT pnt = { xPos, yPos };

        // Check if click was inside the drag handle area
        if (PtInRect(&window::g_dragHandleRect, pnt)) {
            // Use the system's built-in window dragging
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;
        }
        else {
            for (size_t i = 0; i < window::g_iconGUIRecords.size(); i++) {
                if (PtInRect(&window::g_iconGUIRecords[i].first, pnt)) {
                    int iconIdx = window::g_iconGUIRecords[i].second; // Icon index in g_windows differs from the GUI/user visible index of icons
                    // This is because g_windows' contents are drawn in reverse

                    if (!manager::g_windows[iconIdx].hwnd) {
                        break;
                    }

                    manager::ActivateWindowFromTaskbar(iconIdx);
                }
            }
        }
        break;
    }
    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        // Handle hover state
        bool wasInWindow = isMouseInWindow;
        isMouseInWindow = true;

        int prevHover = window::g_hoveredIconIndex;
        window::g_hoveredIconIndex = -1;

        if (PtInRect(&window::g_dragHandleRect, pt) && ShownCursor != IDC_HAND) {
            SetCursor(HAND_CURSOR);
        } else if (ShownCursor == IDC_HAND) {
            SetCursor(ARROW_CURSOR);
        }

        for (size_t i = 0; i < window::g_iconGUIRecords.size(); ++i) {
            if (PtInRect(&window::g_iconGUIRecords[i].first, pt)) {
                window::g_hoveredIconIndex = (int)i;
                graphics::InvalidateIcon(window::g_iconGUIRecords[i].first);
            }
        }

        if (prevHover != window::g_hoveredIconIndex && prevHover != -1) {
            graphics::InvalidateIcon(window::g_iconGUIRecords[prevHover].first);
        }

        if (!wasInWindow) {
            SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
            SetTimer(hWnd, MOUSEHOVER_TIMER_ID, 50, nullptr);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }
    case WM_TIMER: {
        if (wParam == MOUSEHOVER_TIMER_ID) {
            // Check if mouse is still in window
            POINT pt;
            GetCursorPos(&pt);
            HWND hwndUnderMouse = WindowFromPoint(pt);

            if (hwndUnderMouse != hWnd) {
                // Mouse left
                isMouseInWindow = false;
                window::g_hoveredIconIndex = -1;
                KillTimer(hWnd, MOUSEHOVER_TIMER_ID);

                SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 100, LWA_ALPHA);
                InvalidateRect(hWnd, NULL, true);
            }
        }
        if (wParam == CLOCK_ID) {
            SetTimer(window::g_hWnd, CLOCK_ID, 60 * 1000, NULL);
            InvalidateRect(hWnd, NULL, true);
        }
        return 0;
    }
    case WM_DESTROY: {
        hook::KillHooks();
        KillTimer(hWnd, MOUSEHOVER_TIMER_ID);
        PostQuitMessage(0);
        break;
    }
    case FLOATINGDOCK_TRAYICON_MSG: {
        switch (lParam) {
        case WM_LBUTTONUP: {
            window::SwitchProgWindowState();
            break;
        }
        case WM_RBUTTONUP: {
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, 1, L"Show");
            AppendMenu(hMenu, MF_STRING, 2, L"Hide");
            AppendMenu(hMenu, MF_STRING, 3, L"Exit");
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, NULL);
            DestroyMenu(hMenu);

            if (cmd == 1) {
                window::ShowProgWindow();
            }
            else if (cmd == 2) {
                window::HideProgWindow();
            }
            else if (cmd == 3) {
                hook::KillHooks();
                KillTimer(hWnd, MOUSEHOVER_TIMER_ID);
                PostQuitMessage(0);
            }
            break;
        }
        default: {
            break;
        }
        }
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void StartClockTimer() {
    auto now = std::chrono::system_clock::now();

    // Get time until next minute directly using chrono
    auto current_tp = now;
    auto current_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        current_tp.time_since_epoch()).count();

    // Calculate milliseconds in current minute
    long ms_in_current_minute = current_ms % 60000;

    // Calculate milliseconds until next minute
    long total_ms_until_next_minute = 60000 - ms_in_current_minute;

    // Ensure we don't set a timer for 0ms (rare edge case)
    if (total_ms_until_next_minute == 0) {
        total_ms_until_next_minute = 60000;
    }

    SetTimer(window::g_hWnd, CLOCK_ID, total_ms_until_next_minute, NULL);
}