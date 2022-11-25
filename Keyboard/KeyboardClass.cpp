////////////////////////////////////////////////////////////////////
// Filename:      KeyboardClass.cpp
// Revising:      06.11.22
////////////////////////////////////////////////////////////////////
#include "KeyboardClass.h"


KeyboardClass::KeyboardClass()
{
	for (size_t i = 0; i < 256; i++)
		this->keyStates_[i] = false;      // initialize all key states to off (false)
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates_[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty() const
{
	return this->keyBuffer_.empty();
}

bool KeyboardClass::CharBufferIsEmpty() const
{
	return this->charBuffer_.empty();
}

KeyboardEvent KeyboardClass::ReadKey()
{
	if (this->keyBuffer_.empty()) // if no keys to be read
	{
		return KeyboardEvent();   // return an empty keyboard event
	}
	else
	{
		KeyboardEvent e = this->keyBuffer_.front();  // get the first Keyboard Event from the queue
		this->keyBuffer_.pop();  // remove the first item from the queue
		return e; // return a keyboard event
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (this->charBuffer_.empty()) // if no characters to be read
	{
		return 0u; // return 0 (NULL char == '\0')
	}
	else
	{
		unsigned char e = this->charBuffer_.front();  // get the first character from the queue
		this->charBuffer_.pop(); // remove the first item from the queue
		return e; // return a character
	}
}



// -------------------------------- EVENTS ------------------------------------------- //

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	this->keyStates_[key] = true;
	this->keyBuffer_.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	this->keyStates_[key] = false;
	this->keyBuffer_.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key)
{
	this->charBuffer_.push(key);
}



// ---------------------- KEY READING PARAMS SETTERS --------------------------------- //
void KeyboardClass::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys_ = true;
}

void KeyboardClass::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys_ = false;
}

void KeyboardClass::EnableAutoRepeatChars()
{
	this->autoRepeatChars_ = true;
}

void KeyboardClass::DisableAutoRepeatChars()
{
	this->autoRepeatChars_ = false;
}

bool KeyboardClass::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys_;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
	return this->autoRepeatChars_;
}