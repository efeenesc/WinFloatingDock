#pragma once

#include <windows.h>
#include <atlbase.h>
#include <string>
#include <vector>;
#include <sstream>;
#include "../graphics/redrawhelper.h";
#include "../graphics/iconutil.h";
#include "../manager/windowinfo.h";

// Holds the ignore list of window titles. Some are "Settings" and "Program Manager"
extern std::vector<std::wstring> g_ignoreList;

// Holds the program's tracking of active, ALT-TABbable windows
extern std::vector<WindowInfo> g_windows;

/// <summary>
/// Checks if window is alt-tabbable.
/// </summary>
/// <param name="hWnd">HWND of the window to be checked</param>
/// <returns>TRUE if alt - tabbable; show to user if TRUE</returns>
BOOL IsAltTabWindow(HWND hWnd);

/// <summary>
/// Attempts to get process path from HWND, to be later used in attempting 
/// to get the executable's icon.
/// </summary>
/// <param name="hWnd">HWND of the executable's window</param>
/// <returns>The retrieved process path</returns>
std::wstring GetProcessPathFromHWND(HWND hWnd);

/// <summary>
/// 
/// </summary>
/// <param name="title"></param>
/// <returns></returns>
static BOOL CheckWindowInIgnoreList(std::wstring title);
BOOL AddWindowSafe(HWND hwnd);
BOOL TryDeleteWindow(HWND target);
BOOL TrySetForeground(HWND target);
static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
std::vector<WindowInfo> GetRunningWindowsInfo();

/// <summary>
/// Attempts to activate the window a user clicks on by bringing it to foreground via
/// a system call.
/// </summary>
/// <param name="index">int index of the clicked icon in the g_windows vector</param>
/// <param name="windows">Reference to the g_windows vector</param>
void ActivateWindowFromTaskbar(int index, const std::vector<WindowInfo>& windows);
