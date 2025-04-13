#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <atlbase.h>
#include <shlwapi.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")

HBITMAP IconToBitmap(HICON hIcon);
bool SaveIconToFile(HICON hIcon, const std::wstring& filePath);
bool SaveIconToIcoFile(HICON hIcon, const std::wstring& filePath);