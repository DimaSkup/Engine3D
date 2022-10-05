////////////////////////////////////////////////////////////////////
// Filename: KeyboardEvent.cpp
// Revising: 05.10.22
////////////////////////////////////////////////////////////////////
#include "KeyboardEvent.h"

// generates an invalid keyboard event
KeyboardEvent::KeyboardEvent()
{
	type_ = EventType::Invalid;
	key_ = 0u;
}

// generates some particular event by passed type and key
KeyboardEvent::KeyboardEvent(const EventType type, const unsigned char key)
{
	type_ = type;
	key_ = key;
}

bool KeyboardEvent::IsPress() const
{
	return type_ == EventType::Press;
}

bool KeyboardEvent::IsRelease() const
{
	return type_ == EventType::Release;
}

bool KeyboardEvent::IsValid() const
{
	return type_ != EventType::Invalid;
}

unsigned char KeyboardEvent::GetKeyCode() const
{
	return this->key_;
}