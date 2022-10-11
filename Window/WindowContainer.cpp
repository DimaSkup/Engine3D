#include "WindowContainer.h"

WindowContainer* WindowContainer::pWindowContainer_ = nullptr;

WindowContainer::WindowContainer()
{
	Log::Debug(THIS_FUNC_EMPTY);

	// we can have only one instance of the WindowContainer
	if (WindowContainer::pWindowContainer_ == nullptr)
	{
		WindowContainer::pWindowContainer_ = this;
		static bool raw_input_initialized = false;

		// try to register a mouse as a raw input device
		if (raw_input_initialized == false) 
		{
			RAWINPUTDEVICE rid;

			rid.usUsagePage = 0x01; // mouse
			rid.usUsage = 0x02;
			rid.dwFlags = 0;        // use default flags
			rid.hwndTarget = NULL;

			if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			{
				Log::Error(THIS_FUNC, "can't register raw input devices");
				exit(-1);
			}

			raw_input_initialized = true;
		}
		
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
		// --- keyboard messages --- //
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		{
			this->inputManager_.HandleKeyboardMessage(uMsg, wParam, lParam);
			return 0;
		}
		// --- mouse messages --- //
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN: case WM_LBUTTONUP:
		case WM_MBUTTONDOWN: case WM_MBUTTONUP:
		case WM_RBUTTONDOWN: case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
		{
			this->inputManager_.HandleMouseMessage(uMsg, wParam, lParam);
			return 0;
		}
		case WM_INPUT:
		{
			UINT dataSize = 0;
			void* ptrToLParam = &lParam;
			HRAWINPUT hRawInput = static_cast<HRAWINPUT>(ptrToLParam);

			GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
				if (GetRawInputData(hRawInput, RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
				{
					void* ptrRawDataGetToVoid = rawdata.get();
					RAWINPUT* raw = static_cast<RAWINPUT*>(ptrRawDataGetToVoid);
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						mouse_.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					}
				}
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	
}
