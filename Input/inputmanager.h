////////////////////////////////////////////////////////////////////
// Filename:     inputmanager.h
// Description:  contains methods for calling handlers for input events
//
// Revising:     01.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <list>

#include "../Mouse/MouseClass.h"
//#include "Keyboard/KeyboardClass.h"
#include "inputlistener.h"
#include "../Engine/Log.h"


class InputListener;

//////////////////////////////////
// Class name: InputManager
//////////////////////////////////
class InputManager
{
public:
	void Initialize(void);
	void Shutdown(void);

	void AddInputListener(InputListener* listener);
	void SetWinRect(const RECT& winRect);

	void Run(const UINT &message, WPARAM wParam, LPARAM lParam);

private:
	void m_eventMouseMove(void);
	void m_eventMouseClick(const eMouseKeyCodes& code, int x, int y, bool pressed);
	void m_eventMouseWheel(short wheel);
	void m_eventKeyBtn(const eKeyCodes& kc, const wchar_t wch, bool press);

private:

	std::list<InputListener*> m_Listeners;

	// KeyboardClass keyboard;
	MouseClass mouse;

	RECT m_wndRect;
	int m_curX;
	int m_curY;
	int m_curMouseWheel;
};


