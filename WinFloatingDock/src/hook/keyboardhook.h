#pragma once

#include <windows.h>;
#include <stdio.h>;
#include <sstream>;
#include "../manager/manager.h";
#include <vector>

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
BOOL InitKeyboardHook();