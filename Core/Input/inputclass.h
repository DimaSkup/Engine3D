/////////////////////////////////////////////////////////////////////
// Filename:      inputclass.h
// Description:   this class handles input from the devices
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "inputlistener.h"
#include "../Engine/Log.h"

#include <iostream>
#include <DirectXMath.h>

//////////////////////////////////
// Class name: InputClass
//////////////////////////////////
class InputClass : public InputListener
{
public:
	InputClass(void);
	InputClass(const InputClass& copy);
	~InputClass(void);

	//void GetMouseLocation(int&, int&);

	bool IsEscapePressed();      // defines if the "Esc" button is pressed or not

	// ----------------------- GETTERS ------------------------------ //

	DirectX::XMFLOAT2 GetMousePos(void);     // returns a current mouse position
	int GetActiveKeyCode(void);  // returns a scan-code of the currently pressed keyboard button



	//  ------------------ EVENTS HANDLERS -------------------------- //	

	bool MouseMove(const MouseMoveEvent& arg);     // every time when mouse is moved this function is called
	bool MouseWheel(const MouseWheelEvent& arg);

	bool MousePressed(const MouseClickEvent& arg); //  every time when mouse button is pressed this function is called
	bool MouseReleased(const MouseClickEvent& arg);

	bool KeyPressed(const KeyButtonEvent& arg);	 // every time when keyboard button is pressed this function is called
	bool KeyReleased(const KeyButtonEvent& arg); // every time when keyboard button is released this function is called

private:
	//void ProcessInput(void);

	
private:
	int m_mouseX, m_mouseY;
	int m_activeKeyCode;
	bool isEscPressed;
};
