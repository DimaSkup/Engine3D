#include "WindowContainer.h"

WindowContainer* WindowContainer::pWindowContainer_ = nullptr;

WindowContainer::WindowContainer()
{
	Log::Debug(THIS_FUNC_EMPTY);

	// we can have only one instance of the WindowContainer
	if (WindowContainer::pWindowContainer_ == nullptr)
	{
		WindowContainer::pWindowContainer_ = this;
		inputManager_.Initialize(&keyboard_, &mouse_);
	}
	else
	{
		Log::Error(THIS_FUNC, "you can have only one instance of the WindowContainer");
		exit(-1);
	}
}

// a handler for the window messages
LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		{
			this->inputManager_.HandleMessage(uMsg, wParam, lParam);
		}
		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	
}
