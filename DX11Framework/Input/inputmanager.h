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
	LRESULT HandleKeyboardMessage(KeyboardClass & keyboard, const UINT &message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMouseMessage(MouseClass & mouse, const UINT &message, WPARAM wParam, LPARAM lParam);
};


