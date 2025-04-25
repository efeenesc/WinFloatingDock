#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../manager/manager.h"
#include "../graphics/redrawhelper.h"

namespace WinFloatingDock {
    namespace hook {

        // Foreground window hook. This is fired when a window becomes the foreground window (i.e. is focused)
        extern HWINEVENTHOOK g_hook_SystemForeground;

        // Window show hook. This is fired when a window is shown or created.
        extern HWINEVENTHOOK g_hook_ObjectShow;

        // Window hide hook. This is fired when a window is hid or destroyed.
        extern HWINEVENTHOOK g_hook_ObjectHide;

        // Window uncloak hook. This is fired when a window that was cloaked using DMW is uncloaked.
        // This absolute moron is hooked because of the horrendous Settings app. It is the only program
        // I have seen use DWM cloak EXCLUSIVELY for hiding the window. It has WS_VISIBLE despite:
        // 1. opening in the background by itself on a set timer,
        // 2. closing when the settings app is closed,
        // 3. covering the entire current desktop's rect,
        // 4. 
        extern HWINEVENTHOOK g_hook_ObjectUncloak;

        /**
         * @brief Callback for the window tracking related functions
         */
        void CALLBACK WinEventProc(
            HWINEVENTHOOK hWinEventHook,
            DWORD event,
            HWND hwnd,
            LONG idObject,
            LONG idChild,
            DWORD idEventThread,
            DWORD dwmsEventTime
        );

        /**
         * @brief Starts window tracking hooks. Can be configured to throw an error if any of the hooks cannot be initialized.
         * @param ThrowIfFails Defaults to true. If hook initialization fails, throws an error.
         * @returns True if successful, false if not
         */
        BOOL InitHooks(BOOL ThrowIfFails = TRUE);

        /**
         * @brief Kills window tracking hooks. Use for cleanup before terminating program.
         */
        void KillHooks();
    }
}