#pragma once

#include <windows.h>
#include <atlbase.h>
#include <string>

namespace WinFloatingDock {
	namespace graphics {
		/**
		 * @brief Saves HICON to the given path, into an ICO file.
		 * @param hIcon HICON to save
		 * @param fileName Reference to std::wstring containing the filename. Make sure the filename has .ico at the end
		 * @return TRUE if saved successfully, FALSE if not
		 */
		BOOL SaveIconToIcoFile(HICON hIcon, const std::wstring& fileName);
	}
}