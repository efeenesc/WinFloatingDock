#pragma once

#include <windows.h>
#include <atlbase.h>
#include <string>
#include <vector>
#include <sstream>
#include "../graphics/redrawhelper.h"
#include "../graphics/iconutil.h"
#include "../manager/windowinfo.h"

/**
 * @brief Global ignore list for window titles that should not be shown (e.g., "Settings", "Program Manager").
 */
extern std::vector<std::wstring> g_ignoreList;

/**
 * @brief Global list tracking currently active and ALT-TABbable windows.
 * 
 * This is in reverse order of opening; windows opened last are at the end of the list. After a window is opened,
 * insert the window's WindowInfo at the end of the vector. Icons are drawn going from index ::size() to '0'.
 */
extern std::vector<WindowInfo> g_windows;

/**
 * @brief Checks whether a window is ALT-TABbable (i.e., should be visible to the user).
 *
 * @param hWnd Handle to the window to check.
 * @return TRUE if the window is ALT-TABbable; FALSE otherwise.
 */
BOOL IsAltTabWindow(HWND hWnd);

/**
 * @brief Retrieves the full executable path for a process associated with a window handle.
 *
 * @param hWnd Handle to the window whose process path is to be retrieved.
 * @return Full path to the executable, or an empty string if not found.
 */
std::wstring GetProcessPathFromHWND(HWND hWnd);

/**
 * @brief Checks whether the given window title is part of the ignore list.
 *
 * @param title Title of the window to check.
 * @return TRUE if the title is in the ignore list; FALSE otherwise.
 */
static BOOL CheckWindowInIgnoreList(std::wstring title);

/**
 * @brief Safely adds a window to the tracked list if it passes through IsAltTabWindow.
 *
 * @param hwnd Handle to the window to be added.
 * @return TRUE if the window was successfully added; FALSE otherwise.
 */
BOOL AddWindowSafe(HWND hwnd);

/**
 * @brief Removes a window from the tracked list, if it exists.
 * 
 * Searches for the window handle in g_windows, erases from g_windows vector if found.
 *
 * @param target Handle to the window to remove.
 * @return TRUE if the window was found and removed; FALSE otherwise.
 */
BOOL TryDeleteWindow(HWND target);

/**
 * @brief Brings window icon to the front of list if window is tracked internally.
 *
 * This is to automatically organize used apps on when they were opened. This operation is
 * done via std::rotate on g_windows, pushing the window to the end of the std::vector.
 * 
 * @param target Handle to the window to be brought forward.
 * @return TRUE if the window was successfully brought forward in the list; FALSE otherwise.
 */
BOOL BringWindowIconForward(HWND target);

/**
 * @brief Callback function used with EnumWindows to enumerate all top-level windows.
 *
 * @param hwnd Handle to the enumerated window.
 * @param lParam User-defined parameter passed from EnumWindows.
 * @return TRUE to continue enumeration; FALSE to stop.
 */
static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

/**
 * @brief Gets a list of all currently running ALT-TABbable windows and their associated information.
 *
 * @return Vector containing information on all valid windows.
 */
std::vector<WindowInfo> GetRunningWindowsInfo();

/**
 * @brief Activates a window by index in the global tracked list by bringing it to the foreground.
 *
 * @param index Index of the window in the g_windows vector.
 */
void ActivateWindowFromTaskbar(int index);
