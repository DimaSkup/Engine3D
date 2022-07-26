////////////////////////////////////////////////////////////////////
// Filename: inputmanager.cpp
// Revising: 01.06.22
////////////////////////////////////////////////////////////////////
#include "inputmanager.h"
#include "inputcodes.h"
#include "inputlistener.h"
#include "log.h"

// ------------------------------------------------------------------ //
//                                                                    //
//                          PUBLIC METHODS                            //
//                                                                    //
// ------------------------------------------------------------------ //


void InputManager::Initialize(void)
{
	m_curX = m_curY = m_curMouseWheel = 0;
	Log::Get()->Debug(THIS_FUNC_EMPTY);
	return;
}

void InputManager::Shutdown(void)
{
	if (!m_Listeners.empty())
	{
		m_Listeners.clear();
	}

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

void InputManager::AddInputListener(InputListener* listener)
{
	m_Listeners.push_back(listener);
	Log::Get()->Debug(THIS_FUNC, "a new input listener is added");
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
		Log::Get()->Error(THIS_FUNC, "the input handlers list is empty!");
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


		// mouse click events
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

		// mouse wheel event
		case WM_MOUSEWHEEL:  
			m_eventMouseWheel(static_cast<short>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA);
			break;
	}

	return;
}



// ------------------------------------------------------------------ //
//                                                                    //
//                          PRIVATE METHODS                           //
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

