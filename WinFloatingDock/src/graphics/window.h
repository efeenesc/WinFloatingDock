#pragma once;

#include <string>;
#include <windows.h>;
#include <vector>;
#include "../manager/manager.h";
#include "../manager/windowinfo.h";
#include <thread>;

// Program's window's handle
extern HWND g_hWnd;
// Index of the hovered icon. This is expected to be based on the GUI icons shown (i.e. icon is 1st placed on GUI, this is set to 0) 
extern int g_hoveredIconIndex;
// Rectangle reserved for where icons are placed. This rectangle includes the area of all icons
extern RECT g_iconAreaRect;
// RECT (icon area), int (icon's index in g_windows) list of icons displayed on the window
extern std::vector<std::pair<RECT, int>> g_iconGUIRecords;
// Rectangle reserved for where the drag handle is placed
extern RECT g_dragHandleRect;
// Rectangle reserved for where the clock is placed
extern RECT g_clockRect;

/**
 * @brief Draw function used by WM_PAINT. This determines what needs redrawing based on the update rectangle,
 * and calls one or more of the other draw functions.
 * @param hdc HDC
 * @param windows Reference to all tracked windows
 * @param updateRect Reference to update area
 */
void Draw(HDC hdc, const std::vector<WindowInfo>& windows, const RECT& updateRect);

/**
 * @brief Draw function used by 'Draw'. This draws the entire window by calling all other draw functions.
 * @param hdc HDC
 * @param windows Reference to all tracked windows 
 * @param clientRect Reference to update area
 */
void DrawWindow(HDC hdc, const std::vector<WindowInfo>& windows, RECT& clientRect);

/**
 * @brief Draw function used by 'Draw'. This draws the grabbable handle.
 * @param hdc HDC
 * @param clientRect Dimensions of the window
 * @param startingX Where drawing should start on the X axis
 * @param endingX Raw pointer for function to write where drawing on X axis ended. Defaults to null
 */
void DrawHandle(HDC hdc, RECT& clientRect, long startingX, long* endingX = nullptr);

/**
 * @brief Draw function used by 'Draw'. This draws all icons.
 * @param hdc HDC
 * @param windows Reference to all tracked windows
 * @param clientRect Dimensions of the window
 * @param startingX Where drawing should start on the X axis
 * @param endingX Raw pointer for function to write where drawing on X axis ended. Defaults to null
 */
void DrawIcons(HDC hdc, const std::vector<WindowInfo>& windows, RECT& clientRect, long startingX, long* endingX = nullptr);

/**
 * @brief UNUSED draw function. This draws a single icon.
 * @param hdc HDC
 * @param window Reference to WindowInfo of window icon to draw
 * @param iconRect Reference to rectangle where icon will be drawn
 * @param isHovered Whether to draw the rounded background hover rect behind icon
 */
void DrawIcon(HDC hdc, const WindowInfo& window, const RECT& iconRect, bool isHovered = false);

/**
 * @brief Draw function used by 'Draw'. This draws the clock. Clock gets the current time automatically.
 * @param hdc HDC
 * @param clientRect Reference to dimensions of the window
 * @param startingX Where drawing should start on the X axis
 * @param endingX Raw pointer for function to write where drawing on X axis ended. Defaults to null
 */
void DrawClock(HDC hdc, RECT& clientRect, long startingX, long* endingX = nullptr);
