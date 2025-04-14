#include "window.h";

HWND g_hWnd = NULL;
int g_hoveredIconIndex = -1;
int g_focusedIconIndex = -1;
RECT g_iconAreaRect = { 0 };
std::vector<std::pair<RECT, int>> g_iconGUIRecords;
RECT g_dragHandleRect = { 0 };
RECT g_clockRect = { 0 };

void Draw(HDC hdc, const std::vector<WindowInfo>& windows, const RECT& updateRect) {
    if (!IsWindow(g_hWnd)) {
        return;
    }

    RECT clientRect;
    GetClientRect(g_hWnd, &clientRect);

    // Full redraw if update region is the entire window
    if (updateRect.left == 0 && updateRect.top == 0 &&
        updateRect.right == clientRect.right && updateRect.bottom == clientRect.bottom) {
        DrawWindow(hdc, windows, clientRect);
        return;
    }

    // Partial redraw based on update region
    LONG currentXPos = 0;

    // Draw drag handle if it intersects the update region
    if (updateRect.left < g_dragHandleRect.right && updateRect.right > g_dragHandleRect.left) {
        DrawHandle(hdc, clientRect, 0, &currentXPos);
    }
    else {
        // Skip past drag handle area
        currentXPos = g_dragHandleRect.right;
    }

    // Draw all icons if the icon area intersects the update region
    if (updateRect.left < g_iconAreaRect.right && updateRect.right > g_iconAreaRect.left) {
        DrawIcons(hdc, windows, clientRect, currentXPos, &currentXPos);
    }
    else {
        // Skip past icon area
        currentXPos = g_iconAreaRect.right;
    }

    // Draw clock if it intersects the update region
    RECT clockRect = {
        clientRect.right - 80, // Approximate position
        0,
        clientRect.right,
        clientRect.bottom
    };

    if (updateRect.left < clockRect.right && updateRect.right > clockRect.left) {
        DrawClock(hdc, clientRect, currentXPos);
    }
}

void DrawWindow(HDC hdc, const std::vector<WindowInfo>& windows, RECT& clientRect) {
    LONG currentXPos = 0;
    DrawHandle(hdc, clientRect, 0, &currentXPos);
    DrawIcons(hdc, windows, clientRect, currentXPos, &currentXPos);
    DrawClock(hdc, clientRect, currentXPos, &currentXPos);
}

void DrawHandle(HDC hdc, RECT& clientRect, long startingX, long* endingX) {
    int xPad = 5;
    int dragWidth = 10;
    int dragHeight = clientRect.bottom - 10;
    int dragY = 5;
    SetRect(&g_dragHandleRect, startingX + xPad, dragY, startingX + xPad + dragWidth, dragY + dragHeight);
    auto handleColor = RGB(154, 154, 154);
    HBRUSH handleBrush = CreateSolidBrush(handleColor);
    HPEN handlePen = CreatePen(PS_SOLID, 0, handleColor);
    SelectObject(hdc, handlePen);
    SelectObject(hdc, handleBrush);
    RoundRect(hdc, g_dragHandleRect.left, g_dragHandleRect.top,
        g_dragHandleRect.right, g_dragHandleRect.bottom, 5, 5);
    DeleteObject(handleBrush);
    DeleteObject(handlePen);
    if (endingX != nullptr) {
        *endingX = g_dragHandleRect.right;
    }
}

void DrawIcons(HDC hdc, const std::vector<WindowInfo>& windows, RECT& clientRect, long startingX, long* endingX) {
    std::vector<std::pair<RECT, int>> newRects = {};
    int x = 12 + startingX;
    int iconSize = 32;
    int iconPadding = 14;
    HPEN handlePen = CreatePen(PS_SOLID, 0, RGB(58, 58, 58));
    SelectObject(hdc, handlePen);

    // Set the icon area bounds for hit testing and partial redraws
    g_iconAreaRect.left = x - 6;  // Account for the bg rect padding
    g_iconAreaRect.top = 4;       // Top of icon area
    g_iconAreaRect.bottom = 10 + iconSize + 6; // Bottom of icon area with padding

    auto totalsize = windows.size();
    int internal_hovered_idx = g_hoveredIconIndex < 0 ? -1 : totalsize -1 - g_hoveredIconIndex;

    // Decrement just after the check (i.e. if i is 1, it passes; decrement to 0; for loop dies next check)
    for (size_t i = totalsize; i-- > 0; ) {
        RECT iconRect = { x, 10, x + iconSize, 10 + iconSize };
        RECT iconBgRect = { iconRect.left - 6, iconRect.top - 6, iconRect.right + 6, iconRect.bottom + 6 };
        if ((int)i == internal_hovered_idx) {
            HBRUSH hoverBrush = CreateSolidBrush(RGB(58, 58, 58));
            SelectObject(hdc, hoverBrush);
            RoundRect(hdc, iconBgRect.left, iconBgRect.top,
                iconBgRect.right, iconBgRect.bottom, 12, 12);
            DeleteObject(hoverBrush);
        }
        newRects.push_back(std::pair<RECT, int>(iconBgRect, i));
        DrawIconEx(hdc, x, 10, windows[i].hIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);
        x += iconSize + iconPadding;
    }

    // Update the right edge of the icon area
    g_iconAreaRect.right = x;
    g_iconGUIRecords = std::move(newRects);

    DeleteObject(handlePen);
    if (endingX != nullptr) {
        *endingX = x;
    }
}

void DrawIcon(HDC hdc, const WindowInfo& window, const RECT& iconRect, bool isHovered) {
    // Extract the icon area from the surrounding background rect
    int iconSize = 32;
    int iconX = (iconRect.left + iconRect.right - iconSize) / 2;
    int iconY = (iconRect.top + iconRect.bottom - iconSize) / 2;

    // Draw the background if this is the hovered icon
    if (isHovered) {
        HBRUSH hoverBrush = CreateSolidBrush(RGB(58, 58, 58));
        HPEN hoverPen = CreatePen(PS_SOLID, 0, RGB(58, 58, 58));
        SelectObject(hdc, hoverBrush);
        SelectObject(hdc, hoverPen);
        RoundRect(hdc, iconRect.left, iconRect.top,
            iconRect.right, iconRect.bottom, 12, 12);
        DeleteObject(hoverBrush);
        DeleteObject(hoverPen);
    }

    // Draw the icon
    DrawIconEx(hdc, iconX, iconY, window.hIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);
}

void DrawClock(HDC hdc, RECT& clientRect, long startingX, long* endingX) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    WCHAR timeStr[32];
    swprintf_s(timeStr, L"%02d:%02d", st.wHour, st.wMinute);

    // Create clock rectangle
    int clockWidth = 70;
    int clockHeight = 26;
    int clockX = clientRect.right - clockWidth - 10; // 10px padding from right edge
    int clockY = (clientRect.bottom - clockHeight) / 2; // Vertically centered

    RECT clockRect = { clockX, clockY, clockX + clockWidth, clockY + clockHeight };

    // Draw clock background
    HBRUSH clockBrush = CreateSolidBrush(RGB(65, 65, 65));
    HPEN clockPen = CreatePen(PS_SOLID, 0, RGB(65, 65, 65));
    SelectObject(hdc, clockBrush);
    SelectObject(hdc, clockPen);
    RoundRect(hdc, clockRect.left, clockRect.top, clockRect.right, clockRect.bottom, 8, 8);
    DeleteObject(clockBrush);
    DeleteObject(clockPen);

    // Draw time text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(220, 220, 220));

    // Create and select a font
    HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SelectObject(hdc, hFont);

    // Draw the text centered in the clock rectangle
    DrawText(hdc, timeStr, -1, &clockRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DeleteObject(hFont);

    if (endingX != nullptr) {
        *endingX = clockRect.right;
    }
}