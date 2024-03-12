#include "WindowContainer.h"

WindowContainer* WindowContainer::pWindowContainer_ = nullptr;


// initialize main devices handlers and setup its behaviour
WindowContainer::WindowContainer()
{
	Log::Debug(LOG_MACRO);


	// we can have only one instance of the WindowContainer
	if (WindowContainer::pWindowContainer_ == nullptr)
	{
		WindowContainer::pWindowContainer_ = this;
		static bool raw_input_initialized = false;

		// try to register a mouse as a RAW INPUT device
		if (raw_input_initialized == false) 
		{
			RAWINPUTDEVICE rid;

			rid.usUsagePage = 0x01; // mouse
			rid.usUsage = 0x02;
			rid.dwFlags = 0;        // use default flags
			rid.hwndTarget = NULL;

			if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			{
				Log::Error(LOG_MACRO, "can't register raw input devices");
				exit(-1);
			}

			raw_input_initialized = true;
		}
	}
	else
	{
		Log::Error(LOG_MACRO, "you can have only one instance of the WindowContainer");
		exit(-1);
	}
}




WindowContainer::~WindowContainer()
{
	Log::Debug(LOG_MACRO);
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


LRESULT CALLBACK WindowContainer::WindowProc(HWND hwnd, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam)
{
	// Main window procedure;
	// this function is a handler for the window messages

	static bool isMouseMoving = false;
	
	switch (uMsg)
	{
		case WM_ACTIVATE:             // handle activation or deactivation of the window
		{
			pEventHandler_->HandleEvent(hwnd, uMsg, wParam, lParam);
			return 0;
		}
		case WM_CLOSE:                // if we hit the "X" (close) button of the window
		{
			Log::Print(LOG_MACRO, "the window is closed");
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY:              // an event of the window destroyment
		{
			// close the engine entirely
			Log::Print(LOG_MACRO, "the window is destroyed");
			PostQuitMessage(0);
			return 0;
		}
		case WM_MENUCHAR:             // is sent when a menu is active and user presses a key that doesn't correspone to any mnemonic or accelerator key
		{
			// don't beep when we alt-enter
			return MAKELRESULT(0, MNC_CLOSE);
		}
		case WM_GETMINMAXINFO:        // catch this message to prevent the window from becoming too small
		{
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;
		}

	}

	// handle window movement, window resizing, keyboard and mouse events, etc.
	pEventHandler_->HandleEvent(hwnd, uMsg, wParam, lParam);


	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	
}  // WindowProc()

///////////////////////////////////////////////////////////
