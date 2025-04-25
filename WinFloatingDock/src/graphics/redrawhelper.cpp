#include "redrawhelper.h"

namespace WinFloatingDock {

	namespace graphics {

		void InvalidateDragHandle() {
			InvalidateRect(window::g_hWnd, &window::g_dragHandleRect, true);
		};
		void InvalidateClock() {
			InvalidateRect(window::g_hWnd, &window::g_clockRect, true);
		};
		void InvalidateIcons() {
			InvalidateRect(window::g_hWnd, &window::g_iconAreaRect, true);
		};
		void InvalidateIcon(RECT& iconRect) {
			InvalidateRect(window::g_hWnd, &iconRect, true);
		}
	}
}