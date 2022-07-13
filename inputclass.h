#pragma once

#include "inputlistener.h"
#include "log.h"
#include "macros.h"

#include <iostream>

class InputClass : public InputListener
{
public:
	InputClass(void)
	{
		isEscPressed = false;
	}


	// GETTERS
	int GetMouseX(void)
	{
		return m_mouseX;
	}

	int GetMouseY(void)
	{
		return m_mouseY;
	}

	int GetActiveKeyCode(void)
	{
		return m_activeKeyCode;
	}



	// EVENT HANDLERS

	bool MouseMove(const MouseMoveEvent& arg)
	{
		//Log::Get()->Debug("mouse at %d:%d", arg.x, arg.y);
		return false;
	}

	bool KeyPressed(const KeyButtonEvent& arg)
	{
		//printf("%ñ button is pressed\n", arg.wchar);
		m_activeKeyCode = arg.code;

		/*
		if (arg.code == KEY_UP)
		{
			printf("up is pressed\n");
		}
		else if (arg.code == KEY_DOWN)
		{
			printf("down is pressed\n");
		}
		else if (arg.code == KEY_RIGHT)
		{
			printf("right is pressed\n");
		}
		else if (arg.code == KEY_LEFT)
		{
			printf("left is pressed\n");
		}
		*/
		if (arg.code == KEY_ESCAPE)
		{
			isEscPressed = true;
			Log::Get()->Debug(THIS_FUNC, "ESC is pressed!");
		}
		else
		{
			std::cout << (char)arg.wchar << " is pressed" << std::endl;
		}
		

		return false;
	}

	bool KeyReleased(const KeyButtonEvent& arg)
	{
		m_activeKeyCode = NULL;
		return false;
	}

	bool MousePressed(const MouseClickEvent& arg)
	{
		Log::Get()->Debug("mouse button %d clicked at: %d:%d", arg.code, arg.x, arg.y);

		return false;
	}

	bool IsEscapePressed()
	{
		return isEscPressed;
	}


private:
	int m_mouseX;
	int m_mouseY;
	int m_activeKeyCode;


	bool isEscPressed;
};
