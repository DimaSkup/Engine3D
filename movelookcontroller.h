////////////////////////////////////////////////////////////////////
// Filename:    movelookcontroller.h
// Description: this class is responsible for handling mouse and 
//              keyboard input events in order to update camera position
//              according to input data.
// Revising:    30.08.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>

#include "log.h"
#include "window.h"
//#include "inputclass.h"
#include "inputlistener.h"

//////////////////////////////////
// DEFINITIONS
//////////////////////////////////
#define MOUSE_SENSITIVITY 0.00004f    // sensitivity adjustment for the look controller
#define MOVEMENT_SPEED 0.1f      // sensitivity adjustment for the move controller

//////////////////////////////////
// Class name: MoveLookController
//////////////////////////////////
class MoveLookController : public InputListener
{
public:
	bool Initialize(void);
	void Update(void);

	DirectX::XMFLOAT3 GetPosition();     // returns the position of the controller object
	DirectX::XMFLOAT3 GetLookAtPoint();  // returns the point which the controller is facing


	//  ------------------ EVENTS HANDLERS -------------------------- //	

	bool MouseMove(const MouseMoveEvent& arg);     // every time when mouse is moved this function is called
	//bool MouseWheel(const MouseWheelEvent& arg);

	bool MousePressed(const MouseClickEvent& arg); //  every time when mouse button is pressed this function is called
	bool MouseReleased(const MouseClickEvent& arg);

	bool KeyPressed(const KeyButtonEvent& arg);	 // every time when keyboard button is pressed this function is called
	bool KeyReleased(const KeyButtonEvent& arg); // every time when keyboard button is released this function is called


	// memory allocation because we have here XM-data
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:
	void setPosition(_In_ DirectX::XMFLOAT3 pos); // accessor to set position of the controller
	void setOrientation(_In_ float pitch, _In_ float yaw); // accessor to set orientation of the controller

	
private:
	//InputClass* m_pInput;

	// properties of the controller object
	DirectX::XMFLOAT3 m_position;   // the position of the controller
	float m_pitch, m_yaw;           // orientation Euler (ֵיכונ) angles in radians
	int m_activeMouseBtn;

	// properties of the Move controler
	bool m_moveInUse;                   // specifies whether the move control is in use
	DirectX::XMFLOAT2 m_moveFirstDown;  // point where initiali contact occured
	DirectX::XMFLOAT2 m_movePointerPosition;  // point where the move pointer is currently located
	DirectX::XMFLOAT3 m_moveCommand;

	// properties of the Look control
	bool m_lookInUse;                   // specifies whether the look control is in use
	DirectX::XMFLOAT2 m_lookLastPoint;  // last point (from last frame)
	DirectX::XMFLOAT2 m_lookLastDelta;  // for smoothing

	bool m_forward, m_back;             // states for movement
	bool m_left, m_right;
	bool m_up, m_down;

	//int m_prevMouseX, m_prevMouseY;
}; // class MoveLookController
