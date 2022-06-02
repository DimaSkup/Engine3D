#pragma once
#include "inputlistener.h"
#include "includes.h"
#include "log.h"
#include <iostream>

class InputClass : public InputListener
{
public:
	InputClass(void)
	{
		isEscPressed = false;
	}

	bool KeyPressed(const KeyButtonEvent& arg)
	{
		//printf("%ñ button is pressed\n", arg.wchar);
		std::cout << (char)arg.wchar << " is pressed" << std::endl;

		if (arg.code == KEY_UP)
		{
			printf("up is pressed\n");
		}
		if (arg.code == KEY_DOWN)
		{
			printf("down is pressed\n");
		}
		if (arg.code == KEY_RIGHT)
		{
			printf("right is pressed\n");
		}
		if (arg.code == KEY_LEFT)
		{
			printf("left is pressed\n");
		}

		if (arg.code == KEY_ESCAPE)
		{
			isEscPressed = true;
			Log::Get()->Debug(THIS_FUNC, "ESC is pressed!");
		}
		

		
		return false;
	}

	bool IsEscapePressed()
	{
		return isEscPressed;
	}


private:
	bool isEscPressed;
};
