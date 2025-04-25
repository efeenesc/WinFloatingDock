#pragma once

#include <windows.h>
#include <shellapi.h>

namespace WinFloatingDock {
	namespace shell {

		// Floating dock's taskbar tray/shell icon's message ID. 
		// This is used to handle trayicon messages sent to the program's window
#define FLOATINGDOCK_TRAYICON_MSG (WM_USER + 1)

		BOOL InitShellIcon(HWND hWnd, HICON hIcon);
	}
}