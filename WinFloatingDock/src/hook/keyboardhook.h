#pragma once

#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include "../manager/manager.h"

namespace WinFloatingDock {
	namespace hook {

		/**
		 * @brief Hook assigned by InitKeyboardHook, MainKeyboardProc
		 */
		extern HHOOK g_KeyboardHook;

		/**
		 * @brief Low-level keyboard hook callback procedure hooked by InitKeyboardHook of type HOOKPROC
		 */
		LRESULT CALLBACK MainKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

		/**
		 * @brief Initializes low-level keyboard hook with MainKeyboardProc callback HOOKPROC
		 *
		 * This has to initialize a low-level hook to get global access to keyboard input.
		 *
		 * @return TRUE if hook initialized without errors; FALSE otherwise
		 */
		BOOL InitKeyboardHook();
	}
}