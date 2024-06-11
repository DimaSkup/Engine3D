////////////////////////////////////////////////////////////////////
// Filename: inputmanager.cpp
// Revising: 06.10.22
////////////////////////////////////////////////////////////////////
#include "inputmanager.h"

#include <memory>


LRESULT InputManager::HandleKeyboardMessage(KeyboardClass & keyboard, 
	const UINT & message, 
	WPARAM wParam, 
	LPARAM lParam)
{
	switch (message)
	{
		// ---- KEYBOARD MESSAGES ---- //
		case WM_KEYDOWN:
		{
			if (keyboard.IsKeysAutoRepeat())
			{
				keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			else
			{
				// if the key hasn't been pressed before
				if (!(lParam & 0x40000000))  
				{
					keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
				}
			}
			
			return 0;
		} // WM_KEYDOWN
		case WM_KEYUP:
		{
			keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));

			return 0;
		} // WM_KEYUP
		case WM_CHAR:
		{
			keyboard.OnChar(static_cast<unsigned char>(wParam));

			if (keyboard.IsCharsAutoRepeat())
			{
				keyboard.OnChar(static_cast<unsigned char>(wParam));
			}
			else
			{
				// if the key hasn't been pressed before
				if (!(lParam & 0x40000000))
				{
					keyboard.OnChar(static_cast<unsigned char>(wParam));
				}
			}
		
			return 0;
		} // WM_CHAR
	} // switch

	return 0;
}

LRESULT InputManager::HandleMouseMessage(MouseClass & mouse, 
	const UINT& uMsg, 
	WPARAM wParam,
	LPARAM lParam)
{
	static bool isMouseMoving = false;

	switch (uMsg)
	{
		// ---- MOUSE MESSAGES ---- //
		case WM_MOUSEMOVE:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMouseMove(x, y);
			isMouseMoving = true;
			return 0;
		}
		case WM_LBUTTONDOWN: 
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnLeftPressed(x, y);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnRightPressed(x, y);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMiddlePressed(x, y);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnLeftReleased(x, y);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnRightReleased(x, y);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMiddleReleased(x, y);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(x, y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(x, y);
			}
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
					std::vector<BYTE> rawdata(dataSize);
					if (GetRawInputData(*ptrHRawInput, RID_INPUT, rawdata.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
					{
						void* ptrRawDataGetToVoid = rawdata.data();
						RAWINPUT* raw = static_cast<RAWINPUT*>(ptrRawDataGetToVoid);
						if (raw->header.dwType == RIM_TYPEMOUSE)
						{
							// set how much the mouse position changed from the previous one
							mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
							isMouseMoving = false;
						}
					}
				}
			}
			else
			{
				mouse.OnMouseMoveRaw(0, 0);
			}

			return 0;

		} // case WM_INPUT
	} // switch

	return 0;
}

/*
////////////////////////////////////////////////////////////////////
// Filename: inputmanager.cpp
// Revising: 01.06.22
////////////////////////////////////////////////////////////////////
#include "../Engine/Log.h"
#include "inputmanager.h"
#include "inputcodes.h"
#include "inputlistener.h"

// ------------------------------------------------------------------ //
//                                                                    //
//                          PUBLIC METHODS                            //
//                                                                    //
// ------------------------------------------------------------------ //


void InputManager::Initialize(void)
{
	m_curX = m_curY = m_curMouseWheel = 0;
	Log::Get()->Debug(LOG_MACRO);
	return;
}

void InputManager::Shutdown(void)
{
	if (!m_Listeners.empty())
	{
		m_Listeners.clear();
	}

	Log::Get()->Debug(LOG_MACRO);
}

void InputManager::AddInputListener(InputListener* listener)
{
	m_Listeners.push_back(listener);
	Log::Get()->Debug(LOG_MACRO, "a new input listener is added");
	return;
}

void InputManager::SetWinRect(const RECT &winRect)
{
	m_wndRect.left = winRect.left;
	m_wndRect.top = winRect.top;
	m_wndRect.right = winRect.right;
	m_wndRect.bottom = winRect.bottom;
}

void InputManager::Run(const UINT &message, WPARAM wParam, LPARAM lParam)
{
	if (m_Listeners.empty())
	{
		Log::Get()->Error(LOG_MACRO, "the input handlers list is empty!");
		return;
	}

	eKeyCodes keyIndex;
	BYTE lpKeyState[256];
	wchar_t symbol[1];

	m_eventMouseMove();  // mouse moving event
	
	switch (message)
	{
		// keyboard events
		case WM_KEYDOWN:
			keyIndex = static_cast<eKeyCodes>(wParam);
			GetKeyboardState(lpKeyState);
			ToUnicode(static_cast<UINT>(wParam),
				      HIWORD(lParam) & 0xFF,
				      lpKeyState, 
				      symbol, 1, 0);
			
			m_eventKeyBtn(keyIndex, symbol[0], true);
			break;

		case WM_KEYUP:
			keyIndex = static_cast<eKeyCodes>(wParam);
			GetKeyboardState(lpKeyState);
			ToUnicode(static_cast<UINT>(wParam),
				      HIWORD(lParam) & 0xFF,
				      lpKeyState,
				      symbol, 1, 0);
			m_eventKeyBtn(keyIndex, symbol[0], false);
			break;


		// mouse events
		case WM_MOUSEMOVE:
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			mouse.OnMouseMove(x, y);
			return;
		case WM_LBUTTONDOWN:  // LMB
			m_eventMouseClick(MOUSE_LEFT, m_curX, m_curY, true);
			break;
		case WM_LBUTTONUP:   // LMB
			m_eventMouseClick(MOUSE_LEFT, m_curX, m_curY, false);
			break;

		case WM_RBUTTONDOWN: // RMB
			m_eventMouseClick(MOUSE_RIGHT, m_curX, m_curY, true);
			break;
		case WM_RBUTTONUP:   // RMB
			m_eventMouseClick(MOUSE_RIGHT, m_curX, m_curY, false);
			break;
		case WM_MBUTTONDOWN: // MMB
			m_eventMouseClick(MOUSE_MIDDLE, m_curX, m_curY, true);
			break;
		case WM_MBUTTONUP:   // MMB
			m_eventMouseClick(MOUSE_MIDDLE, m_curX, m_curY, false);
			break;
		case WM_MOUSEWHEEL:  
			m_eventMouseWheel(static_cast<short>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA);
			break;
	}

	return;
}



// ------------------------------------------------------------------ //
//                                                                    //
//                          EVENT HANDLERS                            //
//                                                                    //
// ------------------------------------------------------------------ //
void InputManager::m_eventMouseMove(void)
{
	POINT pos;  // current position of the mouse cursor on the screen
	GetCursorPos(&pos);

	pos.x -= m_wndRect.left;
	pos.y -= m_wndRect.top;

	if ((pos.x == m_curX) && (pos.y == m_curY))
		return;

	// current position of the cursor in the window
	m_curX = pos.x;
	m_curY = pos.y;

	for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
	{
		if (!(*it)) 
			continue;
		else if ((*it)->MouseMove(MouseMoveEvent(m_curX, m_curY)) == true)
			return;
	}
}

void InputManager::m_eventMouseClick(const eMouseKeyCodes& code, int x, int y, bool press)
{
	for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
	{
		if (!(*it)) continue;

		
		if (press)	// the button is pressed
		{
			if ((*it)->MousePressed(MouseClickEvent(code, x, y)) == true)
				return;
		}
		else   // the button is released
		{
			if ((*it)->MouseReleased(MouseClickEvent(code, x, y)) == true)
				return;
		}
	}
}

void InputManager::m_eventMouseWheel(short nWheel)
{
	if (m_curMouseWheel == nWheel)
		return;

	for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
	{
		if (!(*it)) 
			continue;
		else if ((*it)->MouseWheel(MouseWheelEvent(nWheel, m_curX, m_curY)) == true)
			return;
	}
}

void InputManager::m_eventKeyBtn(const eKeyCodes& keyCode, const wchar_t wchar, bool pressed)
{
	for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
	{
		if (!(*it))
			continue;
	
		
		if (pressed)
		{
			if ((*it)->KeyPressed(KeyButtonEvent(keyCode, wchar)) == true)
				return;
		}
		else
		{
			if ((*it)->KeyReleased(KeyButtonEvent(keyCode, wchar)) == true)
				return;
		}
	}

	return;
}


*/