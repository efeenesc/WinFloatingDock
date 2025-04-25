#pragma once

#include "./src/resources/Resource.h"
#include "./src/resources/framework.h"
#include <windowsx.h>
#include <dwmapi.h>
#include <cwchar>
#include "src/graphics/window.h"
#include "src/hook/windowhook.h"
#include "src/hook/keyboardhook.h"
#include "src/manager/manager.h"
#include "src/graphics/redrawhelper.h"
#include "src/shell/shellicon.h"
#include "src/timer/basictimer.h"

#pragma comment (lib, "Dwmapi")

/**
 * @brief Registers class for this program instance's window, sets WndProc to run, sets bg color, etc.
 */
ATOM RegisterProgramClass(HINSTANCE hInstance);

/**
 * @brief Initializes current instance by saving instance handle and creating main window
 */
BOOL InitInstance(HINSTANCE, int);
/**
 * @brief Handles painting, registering mouse clicks and hover for drag handle and icons
 */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void StartClockTimer();