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
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WindowContainer* Get() { return pWindowContainer_; };   // returns a pointer to the current WindowContainer instance

	//HWND GetHWND(); // returns the window handler
	//const KeyboardClass* GetKeyboard() { return &keyboard_; }
	//const MouseClass* GetMouse() { return &mouse_; }

	bool IsExit() const { return isExit_; }

protected:
	static WindowContainer* pWindowContainer_;
	RenderWindow renderWindow_;
	InputManager inputManager_;
	KeyboardClass keyboard_;
	MouseClass mouse_;

	bool  isExit_ = false;           // are we going to exit?
private:
};