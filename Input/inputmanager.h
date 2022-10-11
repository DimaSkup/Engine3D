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
//#include <list>

#include "../Mouse/MouseClass.h"
#include "../Keyboard/KeyboardClass.h"
//#include "inputlistener.h"
#include "../Engine/Log.h"


//class InputListener;

//////////////////////////////////
// Class name: InputManager
//////////////////////////////////
class InputManager
{
public:
	bool Initialize(KeyboardClass* keyboard, MouseClass* mouse);

	LRESULT HandleKeyboardMessage(const UINT &message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMouseMessage(const UINT &message, WPARAM wParam, LPARAM lParam);

private:
	KeyboardClass* keyboard_;
	MouseClass* mouse_;
};


