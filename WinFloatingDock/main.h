#pragma once

#include "./src/resources/Resource.h"
#include "./src/resources/framework.h"
#include "main.h"
#include "src/hook/windowhook.h";
#include "src/hook/keyboardhook.h"
#include "src/manager/manager.h"
#include <windowsx.h>;
#include <dwmapi.h>
#include "src/graphics/window.h"
#include <cwchar>
#include "src/graphics/redrawhelper.h";

#pragma comment (lib, "Dwmapi")

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);