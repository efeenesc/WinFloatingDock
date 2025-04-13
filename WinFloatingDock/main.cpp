// main.cpp : Defines the entry point for the application.
//

#include "main.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = L"";                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
static bool isMouseInWindow = false;
UINT_PTR g_timerID = 100;

// Forward declarations of functions included in this code module:


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    if (!InitHooks())
    {
        return FALSE;
    }

    if (!InitKeyboardHook())
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(51, 51, 51));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    GetRunningWindowsInfo();
    hInst = hInstance; // Store instance handle in our global variable

    g_hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED, szWindowClass, szTitle, WS_VISIBLE | WS_POPUP | WS_BORDER,
        50, 50, 400, 55, nullptr, nullptr, hInstance, nullptr);

    if (!g_hWnd)
    {
        return FALSE;
    }

    SetLayeredWindowAttributes(g_hWnd, RGB(0, 0, 0), 100, LWA_ALPHA);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        if (hWnd) {
            DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
            DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

            bool fuck = true;
            DwmSetWindowAttribute(hWnd, DWMWA_USE_HOSTBACKDROPBRUSH, &fuck, sizeof BOOL);
        }
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Draw(hdc, g_windows, ps.rcPaint);
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_LBUTTONDOWN: {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        POINT pnt = { xPos, yPos };

        // Check if click was inside the drag handle area
        if (PtInRect(&g_dragHandleRect, pnt)) {
            // Use the system's built-in window dragging
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;
        }
        else {
            for (size_t i = 0; i < g_iconRects.size(); i++) {
                if (PtInRect(&g_iconRects[i], pnt)) {
                    if (!g_windows[i].hwnd) {
                        break;
                    }

                    SetForegroundWindow(g_windows[i].hwnd);
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

        int prevHover = g_hoveredIconIndex;
        g_hoveredIconIndex = -1;

        for (size_t i = 0; i < g_iconRects.size(); ++i) {
            if (PtInRect(&g_iconRects[i], pt)) {
                g_hoveredIconIndex = (int)i;
                InvalidateIcon(g_iconRects[i]);
            }
        }

        if (prevHover != g_hoveredIconIndex && prevHover != -1) {
            InvalidateIcon(g_iconRects[prevHover]);
        }

        if (!wasInWindow) {
            SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
            SetTimer(hWnd, g_timerID, 50, nullptr);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }
    case WM_TIMER:
        if (wParam == g_timerID) {
            // Check if mouse is still in window
            POINT pt;
            GetCursorPos(&pt);
            HWND hwndUnderMouse = WindowFromPoint(pt);

            if (hwndUnderMouse != hWnd) {
                // Mouse left
                isMouseInWindow = false;
                g_hoveredIconIndex = -1;
                KillTimer(hWnd, g_timerID);

                SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 100, LWA_ALPHA);
                InvalidateRect(hWnd, NULL, true);
            }
        }
        return 0;
    case WM_DESTROY:
        KillHooks();
        KillTimer(hWnd, g_timerID);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}