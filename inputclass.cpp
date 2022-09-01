/////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass(void)
{
	isEscPressed = false;
}

// we don't use a copy constructor and destructor in this class
InputClass::InputClass(const InputClass& copy) {}
InputClass::~InputClass(void) {}



// ------------------------------------------------------------------ //
//                                                                    //
//                          PUBLIC METHODS                            //
//                                                                    //
// ------------------------------------------------------------------ //

// Initialize() sets up
//bool InputClass::Initialize()

// defines if the "Esc" button is pressed or not
bool InputClass::IsEscapePressed()
{
	return isEscPressed;
}

// returns current mouse position
DirectX::XMFLOAT2 InputClass::GetMousePos(void)
{
	return DirectX::XMFLOAT2{ m_mouseX, m_mouseY };
}

// returns a scan-code of the currently pressed keyboard button
int InputClass::GetActiveKeyCode(void)
{
	return m_activeKeyCode;
}


// every time when keyboard button is pressed this function is called
bool InputClass::KeyPressed(const KeyButtonEvent& arg)
{
	//printf("%ñ button is pressed\n", arg.wchar);
	m_activeKeyCode = arg.code;  //the scan-code of the currently pressed keyboard button
	std::string arrowName;
	std::string keybuttonName;
	bool isArrow = false;

	switch (arg.code)
	{
		case KEY_UP:
			arrowName = "UP";
			isArrow = true;
			break;
		case KEY_RIGHT:
			arrowName = "RIGHT";
			isArrow = true;
			break;
		case KEY_DOWN:
			arrowName = "DOWN";
			isArrow = true;
			break;
		case KEY_LEFT:
			arrowName = "LEFT";
			isArrow = true;
			break;
	}
	
	if (isArrow)
	{
		keybuttonName = "arrow " + arrowName;
	}
	else
	{
		keybuttonName = static_cast<char>(arg.wchar);
	}


	if (arg.code == KEY_ESCAPE)  // if the "Esc" button is pressed
	{
		isEscPressed = true;
		Log::Get()->Debug(THIS_FUNC, "ESC is pressed!");
	}
	else  // if any other keyboard button is pressed
	{
	   //std::cout << __FUNCTION__ << "() (" <<  __LINE__ << "): " << keybuttonName << " is pressed" << std::endl;
	}


	return false;
} // KeyPressed();

// every time when keyboard button is released this function is called
bool InputClass::KeyReleased(const KeyButtonEvent& arg)
{
	m_activeKeyCode = NULL;
	return false;
}

// every time when mouse is moved this function is called
bool InputClass::MouseMove(const MouseMoveEvent& arg)
{
	//Log::Get()->Debug("INPUTCLASS::MOUSE MOVE: %d:%d", arg.x, arg.y);
	m_mouseX = arg.x;
	m_mouseY = arg.y;
	//Log::Get()->Debug("mouse at %d:%d", arg.x, arg.y);
	return false;
}

//  every time when mouse wheel is rotated this function is called
bool InputClass::MouseWheel(const MouseWheelEvent& arg)
{
	return false;
}

//  every time when mouse button is pressed this function is called
bool InputClass::MousePressed(const MouseClickEvent& arg)
{
	//Log::Get()->Debug("mouse button %d clicked at: %d:%d", arg.code, arg.x, arg.y);

	return false;
}

//  every time when mouse button is released this function is called
bool InputClass::MouseReleased(const MouseClickEvent& arg)
{
	return false;
}






// ------------------------------------------------------------------ //
//                                                                    //
//                          PRIVATE METHODS                           //
//                                                                    //
// ------------------------------------------------------------------ //

