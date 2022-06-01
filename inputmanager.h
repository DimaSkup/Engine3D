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
#include "inputhandler.h"
#include "includes.h"
#include "log.h"


//////////////////////////////////
// Class name: InputManager
//////////////////////////////////
class InputManager
{
public:
	void Initialize(void);
	void Shutdown(void);

	void AddInputHandler(InputHandler* handler);
	void SetWindowZone(const RECT& windowRect);

	void ManageInput(MSG message, WPARAM wParam, LPARAM lParam);

private:
	void m_eMouseMove(void);
	void m_eMouseClick(const eMouseKeyCodes& code, int x, int y);
	void m_eMouseWheel(short wheel);
	void m_eKeyCode(const eKeyCodes& kc, const wchar_t wch);

private:
	std::vector<InputHandler*> m_inputHandlers;

	RECT m_wndRect;
	int m_curX;
	int m_curY;
	int m_curMouseWheel;
};


