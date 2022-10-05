#include "WindowContainer.h"

WindowContainer* WindowContainer::pWindowContainer_ = nullptr;

// a handler for the window messages
LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
