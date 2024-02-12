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
		
		keyboard_.EnableAutoRepeatKeys();
		keyboard_.EnableAutoRepeatChars();
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
	// this function is a handler for the window messages

	static bool isMouseMoving = false;

	switch (uMsg)
	{

		case WM_CLOSE:					// if we hit the "X" (close) button of the window
		{
			Log::Print(LOG_MACRO, "the window is closed");
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY:				// an event of the window destroyment
		{
			// close the engine entirely
			Log::Print(LOG_MACRO, "the window is destroyed");
			isExit_ = true;
			PostQuitMessage(0);
			return 0;
		}

		case WM_MOVE:
		{
			Log::Debug(LOG_MACRO, "THE WINDOW IS MOVED");
			int wndLeftPos = static_cast<int>(LOWORD(lParam));
			int wndTopPos = static_cast<int>(HIWORD(lParam));

			Settings::Get()->UpdateSettingByKey("WINDOW_LEFT_POS", wndLeftPos);
			Settings::Get()->UpdateSettingByKey("WINDOW_TOP_POS", wndTopPos);
				
			return 0;
		}

		case WM_SIZE:
		{
			Log::Debug(LOG_MACRO, "THE WINDOW IS RESIZED");
			//isResizing_ = true;

			int newWindowWidth = static_cast<int>(LOWORD(lParam));
			int newWindowHeight = static_cast<int>(HIWORD(lParam));

			// update the window settings
			Settings::Get()->UpdateSettingByKey("WINDOW_WIDTH", newWindowWidth);
			Settings::Get()->UpdateSettingByKey("WINDOW_HEIGHT", newWindowHeight);

			// update the currernt window dimensions for the renderWindow object
			renderWindow_.UpdateWindowDimensions(newWindowWidth, newWindowHeight);
		
			// update the window rectangle
			RECT winRect; 
			winRect.left = Settings::Get()->GetSettingIntByKey("WINDOW_LEFT_POS");
			winRect.top = Settings::Get()->GetSettingIntByKey("WINDOW_TOP_POS");
			winRect.right = winRect.left + newWindowWidth;
			winRect.bottom = winRect.top + newWindowHeight;
			AdjustWindowRect(&winRect, GetWindowLong(hwnd, GWL_STYLE), FALSE);

		
			// update the window position
			SetWindowPos(hwnd, 0,
				winRect.left, winRect.top,
				winRect.right - winRect.left,
				winRect.bottom - winRect.top,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

			UpdateWindow(hwnd);

			return 0;
		}

		// --- keyboard messages --- //
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		{
			this->inputManager_.HandleKeyboardMessage(keyboard_, uMsg, wParam, lParam);
			return 0;
		}
		// --- mouse messages --- //
		case WM_MOUSEMOVE:
		{
			this->inputManager_.HandleMouseMessage(mouse_, uMsg, wParam, lParam);
			isMouseMoving = true;
			return 0;
		}

		case WM_LBUTTONDOWN: case WM_LBUTTONUP:
		case WM_MBUTTONDOWN: case WM_MBUTTONUP:
		case WM_RBUTTONDOWN: case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
		{
			this->inputManager_.HandleMouseMessage(mouse_, uMsg, wParam, lParam);
			return 0;
		}

		// --- raw input --- //
		case WM_INPUT:
		{
			if (isMouseMoving == true)
			{
				UINT dataSize = 0;
				void* ptrToLParam = &lParam;
				HRAWINPUT* ptrHRawInput = static_cast<HRAWINPUT*>(ptrToLParam); // convert the lParam structure to HRAWINPUT

				GetRawInputData(*ptrHRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

				if (dataSize > 0) // if we got some data about a raw input
				{
					std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
					if (GetRawInputData(*ptrHRawInput, RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
					{
						void* ptrRawDataGetToVoid = rawdata.get();
						RAWINPUT* raw = static_cast<RAWINPUT*>(ptrRawDataGetToVoid);
						if (raw->header.dwType == RIM_TYPEMOUSE)
						{
							// set how much the mouse position changed from the previous one
							mouse_.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
							isMouseMoving = false;
						}
					}
				}
			}
			else
			{
				mouse_.OnMouseMoveRaw(0, 0);
			}
		

			return 0;
		} // WM_INPUT
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	
}  // WindowProc()
