#include "windowhook.h"

namespace WinFloatingDock {
    namespace hook {

        HWINEVENTHOOK g_hook_SystemForeground = NULL;
        HWINEVENTHOOK g_hook_ObjectShow = NULL;
        HWINEVENTHOOK g_hook_ObjectHide = NULL;
        HWINEVENTHOOK g_hook_ObjectUncloak = NULL;

        void CALLBACK WinEventProc(
            HWINEVENTHOOK hWinEventHook,
            DWORD event,
            HWND hwnd,
            LONG idObject,
            LONG idChild,
            DWORD idEventThread,
            DWORD dwmsEventTime
        ) {
            // We only care about events where the object is a window (OBJID_WINDOW)
            if (idObject != OBJID_WINDOW) return;

            // Skip invalid window handles
            if (!IsWindow(hwnd)) return;

            switch (event) {
            case EVENT_SYSTEM_FOREGROUND: {
                if (manager::BringWindowIconForward(hwnd)) {
                    InvalidateRect(window::g_hWnd, NULL, true);
                };
                break;
            }

            case EVENT_OBJECT_UNCLOAKED:
            case EVENT_OBJECT_SHOW: {
                if (manager::AddWindowSafe(hwnd)) {
                    InvalidateRect(window::g_hWnd, NULL, true);
                }
                break;
            }

                                  // This was originally EVENT_OBJECT_DESTROY, until I realized hiding windows doesn't destroy them. Duh
            case EVENT_OBJECT_HIDE: {
                if (manager::TryDeleteWindow(hwnd)) {
                    graphics::InvalidateIcons();
                }
                break;
            }

            default: {
                break;
            }
            }
        }

        BOOL InitHooks(BOOL ThrowIfFails) {
            g_hook_SystemForeground = SetWinEventHook(
                EVENT_SYSTEM_FOREGROUND,
                EVENT_SYSTEM_FOREGROUND,
                NULL, // requests DLL here - null cause WinEventProc func is used
                WinEventProc,
                0,
                0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );

            g_hook_ObjectUncloak = SetWinEventHook(
                EVENT_OBJECT_UNCLOAKED,
                EVENT_OBJECT_UNCLOAKED,
                NULL,
                WinEventProc,
                0,
                0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );

            g_hook_ObjectShow = SetWinEventHook(
                EVENT_OBJECT_SHOW,
                EVENT_OBJECT_SHOW,
                NULL, // requests DLL here - null cause WinEventProc func is used
                WinEventProc,
                0,
                0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );

            g_hook_ObjectHide = SetWinEventHook(
                EVENT_OBJECT_HIDE,
                EVENT_OBJECT_HIDE,
                NULL, // requests DLL here - null cause WinEventProc func is used
                WinEventProc,
                0,
                0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );

            if (!g_hook_SystemForeground || !g_hook_ObjectShow || !g_hook_ObjectHide) {
                if (ThrowIfFails) {
                    std::stringstream ss;
                    ss << "Failed to create hooks. Error: " << GetLastError() << std::endl;
                    throw std::runtime_error{ ss.str() };
                }
                else {
                    return FALSE;
                }
            }

            return TRUE;
        }

        void KillHooks() {
            if (g_hook_SystemForeground)    UnhookWinEvent(g_hook_SystemForeground);
            if (g_hook_ObjectShow)          UnhookWinEvent(g_hook_ObjectShow);
            if (g_hook_ObjectHide)          UnhookWinEvent(g_hook_ObjectHide);
        }
    }
}