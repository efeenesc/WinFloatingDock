#pragma once

#include "window.h"

namespace WinFloatingDock {

	namespace graphics {
		void InvalidateDragHandle();
		void InvalidateClock();
		void InvalidateIcons();
		void InvalidateIcon(RECT& iconRect);
	}
}