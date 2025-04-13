#include "redrawhelper.h";

void InvalidateDragHandle() {
	InvalidateRect(g_hWnd, &g_dragHandleRect, true);
};
void InvalidateClock() {
	InvalidateRect(g_hWnd, &g_clockRect, true);
};
void InvalidateIcons() {
	InvalidateRect(g_hWnd, &g_iconAreaRect, true);
};
void InvalidateIcon(RECT& iconRect) {
	InvalidateRect(g_hWnd, &iconRect, true);
}