////////////////////////////////////////////////////////////////////
// Filename:      KeyboardClass.h
// Description:   this class is responsible containing and 
//                handling keyboard events, generating of 
//                relevant events, containing the queue of 
//                the keyboard events.
// Revising:      05.10.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "KeyboardEvent.h"
#include <queue>


//////////////////////////////////
// Class name: KeyboardClass
//////////////////////////////////
class KeyboardClass
{
public:
	KeyboardClass();

	bool KeyIsPressed(const unsigned char keycode);
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey();
	unsigned char ReadChar();

private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];     // an array of all the keys

	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};