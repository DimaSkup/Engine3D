#pragma once

#include <memory>

#include "RenderWindow.h"
#include "../Input/inputmanager.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Mouse/MouseClass.h"
#include "../Input/inputcodes.h"
#include "../Engine/Settings.h"


class WindowContainer
{
public:
	WindowContainer();
	~WindowContainer();

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WindowContainer* Get() { return pWindowContainer_; };   // returns a pointer to the current WindowContainer instance

	//HWND GetHWND(); // returns the window handler
	//const KeyboardClass* GetKeyboard() { return &pKeyboard_; }
	//const MouseClass* GetMouse() { return &pMouse_; }

	bool WindowResize(HWND hwnd, WPARAM wParam, LPARAM lParam);
	bool WindowResizing(HWND hwnd, WPARAM wParam, LPARAM lParam);

	inline bool IsExit() const { return isExit_; }

	inline bool IsResizing()
	{ 
		bool temp = isResizing_;
		isResizing_ = false;
		return temp;
	}


public: 
	static WindowContainer* pWindowContainer_;
	RenderWindow  renderWindow_;
	InputManager  inputManager_;
	KeyboardClass keyboard_;         // represents a keyboard device
	MouseClass    mouse_;            // represents a mouse device

	bool  isExit_ = false;           // are we going to exit?
	bool  isResizing_ = false;       // are we resizing the window?
};