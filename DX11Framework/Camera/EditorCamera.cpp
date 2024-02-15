////////////////////////////////////////////////////////////////////
// Filename:     EditorCamera.cpp
// Description:  an implementation of the EditorCamera class;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#include "EditorCamera.h"


// the class constructor initialize the private member variables to zero to start with
EditorCamera::EditorCamera()
{
}


EditorCamera::~EditorCamera()
{
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//
//                                PUBLIC FUNCTIONS
//
//
/////////////////////////////////////////////////////////////////////////////////////////



void EditorCamera::HandleKeyboardEvents(const KeyboardEvent& kbe, const float deltaTime)
{
	// this function handles and updates the position of the camera

	// handle pressing of W,A,S,D,space,Z keys
	this->HandlePositionChange(deltaTime);

	return;
} 

///////////////////////////////////////////////////////////


void EditorCamera::HandleMouseMovement(
	const int yawDelta,
	const int pitchDelta,
	const float deltaTime)
{
	// this function handles the changing of the camera rotation

	// update the value of pitch and yaw for this frame
	static float pitch = 0.0f;
	static float yaw = 0.0f;
	const float speed_mul_delta = this->rotationSpeed_ * deltaTime;

	pitch += (pitchDelta * speed_mul_delta); // up/down movement
	yaw += (yawDelta * speed_mul_delta);     // right/left movement
	

	// limit the pitch value in range (-PI/2 < pitch < PI/2)
	if (pitch > DirectX::XM_PIDIV2)
	{
		pitch = DirectX::XM_PIDIV2;
	}
	else if (pitch < -DirectX::XM_PIDIV2)
	{
		pitch = -DirectX::XM_PIDIV2;
	}

	// limit the yaw value in range (-2PI < yaw < 2PI)
	if (yaw > DirectX::XM_2PI)
	{
		yaw = -DirectX::XM_2PI;
	}
	else if (yaw < -DirectX::XM_2PI)
	{
		yaw = DirectX::XM_2PI;
	}
	
	// update the rotation angle
	this->SetRotationInRad({ pitch, yaw, 0.0f });

	return;
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//
//                                PRIVATE FUNCTIONS
//
//
/////////////////////////////////////////////////////////////////////////////////////////

// handles the changing of the camera position
void EditorCamera::HandlePositionChange(const float deltaTime)
{
	BYTE keyboardState[256];
	GetKeyboardState(keyboardState);

	const float movingSpeedMulFrameTime = movingSpeed_ * deltaTime;

	/////  handle the position changes  /////

	DirectX::XMVECTOR movementVec{ 0, 0, 0 };

	if (1 < keyboardState['W']) // pressed W (move forward)
	{
		movementVec += GetForwardVector();
	}
	if (1 < keyboardState['S']) // pressed S (move backward)
	{
		movementVec += GetBackwardVector();
	}
	if (1 < keyboardState['A']) // pressed A (move left)
	{
		movementVec += GetLeftVector();
	}
	if (1 < keyboardState['D']) // pressed D (move right)
	{
		movementVec += GetRightVector();
	}
	if (1 < keyboardState[' ']) // pressed space (move up)
	{
		movementVec += { 0.0f, 1.0f, 0.0f };
	}
	if (1 < keyboardState['Z']) // pressed Z (move down)
	{
		movementVec += { 0.0f, -1.0f, 0.0f };
	}


	// update the position of the camera
	this->AdjustPosition(movementVec * movingSpeedMulFrameTime);

	return;
}