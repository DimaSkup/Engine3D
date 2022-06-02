////////////////////////////////////////////////////////////////////
// Filename: inputmanager.cpp
// Revising: 01.06.22
////////////////////////////////////////////////////////////////////
#include "inputmanager.h"
#include "inputcodes.h"
#include "inputlistener.h"
#include "log.h"

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

void InputManager::SetWindowZone(const RECT& clientRect)
{
	m_wndRect.left = clientRect.left;
	m_wndRect.top = clientRect.top;
	m_wndRect.right = clientRect.right;
	m_wndRect.bottom = clientRect.bottom;

	Log::Get()->Debug(THIS_FUNC, "the window zone is updated");
	return;
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

	//m_eventMouseMove();  // mouse moving event
	
	switch (message)
	{
		case WM_KEYDOWN:
			
			keyIndex = static_cast<eKeyCodes>(wParam);
			GetKeyboardState(lpKeyState);
			ToUnicode(wParam, 
				      HIWORD(lParam) & 0xFF,
				      lpKeyState, 
				      symbol, 1, 0);
			
			m_eventKeyBtn(keyIndex, symbol[0], true);
			break;
	}

	return;
}

void InputManager::m_eventKeyBtn(const eKeyCodes& keyCode, const wchar_t wchar, bool pressed)
{
	for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
	{
		if (!(*it))
		{
			continue;
		}
		
		if (pressed)
		{
			if ((*it)->KeyPressed(KeyButtonEvent(keyCode, wchar)) == true)
				return;
		}
		else
		{
			//if ((*it)->KeyReleased(KeyButtonEvent(keyCode, wchar)) == true)
				//return;
		}
	}

	return;
}

