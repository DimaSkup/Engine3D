#pragma once

#include "RenderWindow.h"
#include "../Input/inputmanager.h"

class WindowContainer
{
public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	RenderWindow render_window;
	InputManager input;
private:
};