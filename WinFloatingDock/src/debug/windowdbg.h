#pragma once

#include <wtypes.h>
#include <string>
#include <Psapi.h>
#include <sstream>
#include <dwmapi.h>

namespace debug {

	/**
	 * @brief Get process name from window handle
	 * @param hwnd HWND to get process name of
	 * @return Process name wstring if process found; "<unknown>" if not
	 */
	std::wstring GetProcessNameFromWindow(HWND hwnd);

	/**
	 * @brief Prints window info of given HWND. Prints out the given eventType string before
	 * printing to output console own debug string of window's basic attributes
	 * 
	 * @param hwnd HWND of target window
	 */
	void PrintWindowInfo(HWND hwnd);

	void PrintDetailedWindowAttributes(HWND hwnd);
}
