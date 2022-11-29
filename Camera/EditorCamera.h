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
#include "../Keyboard/KeyboardEvent.h"
#include "../Mouse/MouseEvent.h"
#include "../Input/inputcodes.h"

#include "cameraclass.h"
#include <vector>
#include "../Engine/Settings.h"

//////////////////////////////////
// Class name: EditorCamera
//////////////////////////////////
class EditorCamera : public CameraClass
{
public:
	EditorCamera(void);
	EditorCamera(const EditorCamera& copy);
	~EditorCamera(void);

	void SetFrameTime(float time);
	void HandleMovement(KeyboardEvent& kbe, MouseEvent& me); // handles the camera changes accodring to the input from a mouse or keyboard

private:
	void HandlePosition(const BYTE* keyboardState);  // handles the changing of the camera position
	void HandleRotation(MouseEvent& me, const BYTE* keyboardState);  // handles the changing of the camera rotation

	void calcNewPosition(void);


	bool IsMovingNow();
	bool IsRotationNow();
private:


	DirectX::XMFLOAT3 moveCommand_;

	float m_movementSpeed; // a camera movement speed
	float m_turnSpeed;     // a camera turning speed
	float m_frameTime;

	// moving state bits which are set by the keyboard input events
	//bool isNowMovement_;   // are we moving now?
	bool isForward_;
	bool isBack_;
	bool isRight_;
	bool isLeft_;
	bool isUp_;
	bool isDown_;

	bool isRotateUp_ = false;
	bool isRotateDown_ = false;
	bool isRotateLeft_ = false;
	bool isRotateRight_ = false;


};
