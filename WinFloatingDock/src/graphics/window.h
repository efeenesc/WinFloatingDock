#pragma once;

#include <string>;
#include <windows.h>;
#include <vector>;
#include "../manager/manager.h";
#include "../manager/windowinfo.h";
#include <thread>;

extern HWND g_hWnd;
extern int g_hoveredIconIndex;
extern int g_focusedIconIndex;
extern RECT g_iconAreaRect;
extern std::vector<RECT> g_iconRects;
extern RECT g_dragHandleRect;
extern bool g_isDragging;
extern RECT g_clockRect;

void Draw(HDC hdc, const std::vector<WindowInfo>& windows, const RECT& updateRect);
void DrawWindow(HDC hdc, const std::vector<WindowInfo>& windows, RECT& clientRect);
void DrawHandle(HDC hdc, RECT& clientRect, long startingX, long* endingX = nullptr);
void DrawIcons(HDC hdc, const std::vector<WindowInfo>& windows, RECT& clientRect, long startingX, long* endingX = nullptr);
void DrawIcon(HDC hdc, const WindowInfo& window, const RECT& iconRect);
void DrawClock(HDC hdc, RECT& clientRect, long startingX, long* endingX = nullptr);
void ActivateWindowFromTaskbar(int index, const std::vector<WindowInfo>& windows);
