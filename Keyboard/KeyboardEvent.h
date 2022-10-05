////////////////////////////////////////////////////////////////////
// Filename:      KeyboardEvent.h
// Description:   this class is responsible for containing state 
//                of some particular keyboard event
// Revising:      05.10.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// Class name: KeyboardEvent
//////////////////////////////////
class KeyboardEvent
{
public:
	enum EventType
	{
		Press,    // pressing of key
		Release,  // releasing of key
		Invalid   // event is invalid
	};

	KeyboardEvent();  // generates an invalid event
	KeyboardEvent(const EventType type, const unsigned char key);  // initializes some particular event by passed type and key

	bool IsPress() const;   
	bool IsRelease() const; 
	bool IsValid() const;
	unsigned char GetKeyCode() const;  

private:
	EventType type;
	unsigned char key;
};