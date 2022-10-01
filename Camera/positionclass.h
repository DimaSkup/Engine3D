////////////////////////////////////////////////////////////////////
// Filename:      positionclass.h
// Description:   this class is needed to calculate and maintain
//                the position of the viewer. Handles different 
//                movement changes. The movement can also include
//                acceleration and deceleration to create a smooth
//                camera effect.
//
// Revising:      24.09.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <cmath>
#include <DirectXMath.h>

#include "../LogSystem/Log.h"
#include "../Input/inputclass.h"

//////////////////////////////////
// Class name: PositionClass
//////////////////////////////////
class PositionClass
{
public:
	PositionClass(void);
	PositionClass(const PositionClass& copy);
	~PositionClass(void);

	void SetFrameTime(float time);
	void HandleMovement(InputClass* input);

	void SetPosition(float posX, float posY, float posZ);
	void SetRotation(float pitch, float yaw); 

	DirectX::XMFLOAT3 GetPosition(void);  // returns the x,y,z position coordinates
	DirectX::XMFLOAT2 GetRotation(void);  // returns the pitch and yaw values 

private:
	void calcNewPosition(void);
	void calcTurnSpeed(bool increase);  // calculate the camera movement speed for this frame

	void KeyPressed(UINT keyCode);

private:
	DirectX::XMFLOAT3 m_position;
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
