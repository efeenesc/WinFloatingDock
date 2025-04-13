#pragma once

#include <windows.h>;
#include <string>;
#include <psapi.h>;
#include <iostream>;
#include <sstream>;
#include <iomanip>;
#include "../manager/manager.h";
#include "../graphics/redrawhelper.h";

std::wstring GetProcessNameFromWindow(HWND hwnd);
void PrintWindowInfo(HWND hwnd, const char* eventType);
void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
);
bool InitHooks();
void KillHooks();