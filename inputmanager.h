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
//#include "inputcodes.h"
#include "inputlistener.h"
#include "includes.h"
#include "log.h"


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
	void SetWindowZone(const RECT& windowRect);

	void Run(const UINT &message, WPARAM wParam, LPARAM lParam);

private:
	//void m_eMouseMove(void);
	//void m_eMouseClick(const eMouseKeyCodes& code, int x, int y, bool pressed);
	//void m_eMouseWheel(short wheel);
	void m_eventKeyBtn(const eKeyCodes& kc, const wchar_t wch, bool press);

private:
	std::vector<InputListener*> m_Listeners;

	RECT m_wndRect;
	int m_curX;
	int m_curY;
	int m_curMouseWheel;
};


