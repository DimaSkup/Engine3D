#pragma once

#include "RenderWindow.h"
#include "../Input/inputmanager.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Mouse/MouseClass.h"
#include <memory>

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WindowContainer* Get() { return pWindowContainer_; };   // returns a pointer to the current WindowContainer instance

	//HWND GetHWND(); // returns the window handler

protected:
	static WindowContainer* pWindowContainer_;
	RenderWindow renderWindow_;
	InputManager inputManager_;
	KeyboardClass keyboard_;
	MouseClass mouse_;
private:
};