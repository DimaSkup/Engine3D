#pragma once

#include "RenderWindow.h"
#include "../Input/inputmanager.h"

class WindowContainer
{
public:
	static LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WindowContainer* Get() { return pWindowContainer_; };   // returns a pointer to the current WindowContainer instance

	//MouseClass
	//Keyboard

protected:
	static WindowContainer* pWindowContainer_;
	RenderWindow renderWindow_;
	InputManager input_;
	//KeyboardClass keyboard;
	MouseClass mouse_;

private:
};