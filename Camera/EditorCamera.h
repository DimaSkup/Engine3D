////////////////////////////////////////////////////////////////////
// Filename:      EditorCamera.h
// Description:   is needed to calculate and maintain  the position of 
//                the ENGINE'S EDITOR camera. Handles different 
//                movement changes. The movement can also include
//                acceleration and deceleration to create a smooth
//                camera effect.
//
// Revising:      25.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <cmath>
#include <DirectXMath.h>

#include "../Engine/Log.h"
//#include "../Input/inputclass.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Mouse/MouseClass.h"
#include "../Input/inputcodes.h"

//////////////////////////////////
// Class name: EditorCamera
//////////////////////////////////
class EditorCamera
{
public:
	EditorCamera(void);
	EditorCamera(const EditorCamera& copy);
	~EditorCamera(void);

	void SetFrameTime(float time);
	void HandleMovement(KeyboardClass& keyboard, MouseClass& mouse); // handles the camera changes accodring to the input from a mouse or keyboard

	void SetPosition(float posX, float posY, float posZ);
	void SetRotation(float pitch, float yaw);

	void GetPosition(DirectX::XMFLOAT3& position);  // returns the x,y,z position coordinates
	void GetRotation(DirectX::XMFLOAT2& rotation);  // returns the pitch and yaw values 

private:
	void HandlePosition(KeyboardClass& keyboard);  // handles the changing of the camera position
	void HandleRotation(KeyboardClass& keyboard);  // handles the changing of the camera rotation

	void calcNewPosition(void);
	void calcTurnSpeed(bool increase);  // calculate the camera movement speed for this frame

	void KeyPressed(UINT keyCode);

private:
	DirectX::XMFLOAT3 position_;
	DirectX::XMFLOAT3 m_moveCommand;
	float m_pitch;
	float m_yaw;

	float m_movementSpeed; // a camera movement speed
	float m_turnSpeed;     // a camera turning speed
	float m_frameTime;

	// moving state bits which are set by the keyboard input events
	bool m_forward;
	bool m_back;
	bool m_right;
	bool m_left;
	bool m_up;
	bool m_down;
};
