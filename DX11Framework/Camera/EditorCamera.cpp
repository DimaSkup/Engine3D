////////////////////////////////////////////////////////////////////
// Filename:     EditorCamera.cpp
// Description:  an implementation of the EditorCamera class;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#include "EditorCamera.h"


// the class constructor initialize the private member variables to zero to start with
EditorCamera::EditorCamera(const float cameraSpeed, const float cameraSensitivity)
	: CameraClass(cameraSpeed, cameraSensitivity)
{
	frameTime_ = 0.0f;
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

// this function is used to set the frame speed in this class. Later this class will use
// that frame time speed to calculate how fast the viewer should be moving and rotating.
// This function should always be called at the beginning of each frame before using
// this class to change the viewing position
void EditorCamera::SetFrameTime(const float time)
{
	if (time > 16.6f)   // if we have less than 60 frames per second (1000 miliseconds / 60 = 16.6)
		frameTime_ = 16.6f;
	else
		frameTime_ = time;

	return;
}



// handles and updates the position
// rotation.x -- it is a rotation around X-axis (vertical rotation)
// rotation.y -- it is a rotation around Y-axis (horizontal rotation)
void EditorCamera::HandleKeyboardEvents(const KeyboardEvent& kbe)
{
	// handle pressing of W,A,S,D,space,Z keys
	this->HandlePosition();

	return;
} // HandleMovement()



  // handles the changing of the camera rotation
void EditorCamera::HandleMouseEvents(const MouseEvent& me)
{
	int yawDelta = me.GetPosX();
	int pitchDelta = me.GetPosY();
	int rollDelta = 0;

	yaw_ += (yawDelta * this->rotationSpeed_ * frameTime_);      // yaw is calculated if we move the mouse in the right or left direction
	pitch_ += (pitchDelta * this->rotationSpeed_ * frameTime_);  // pitch is calculated if we move the mouse in upward or downward
	

	// limit the pitch value
	if (pitch_ > DirectX::XM_PIDIV2)
	{
		pitch_ = DirectX::XM_PIDIV2;
	}
	else if (pitch_ < -DirectX::XM_PIDIV2)
	{
		pitch_ = -DirectX::XM_PIDIV2;
	}

	// limit the yaw value
	if (yaw_ > DirectX::XM_2PI)
	{
		yaw_ = -DirectX::XM_2PI;
	}
	else if (yaw_ < -DirectX::XM_2PI)
	{
		yaw_ = DirectX::XM_2PI;
	}
	
	// update the rotation angle
	this->SetRotation(pitch_, yaw_, roll_);
	
	return;
} // end HandleMouseEvents




/////////////////////////////////////////////////////////////////////////////////////////
//
//
//                                PRIVATE FUNCTIONS
//
//
/////////////////////////////////////////////////////////////////////////////////////////

// handles the changing of the camera position
void EditorCamera::HandlePosition()
{
	BYTE keyboardState[256];
	GetKeyboardState(keyboardState);

	isForward_  = (1 < (int)keyboardState['W']);   // W
	isLeft_     = (1 < (int)keyboardState['A']);   // A
	isBackward_ = (1 < (int)keyboardState['S']);   // S
	isRight_    = (1 < (int)keyboardState['D']);   // D
	isUp_       = (1 < (int)keyboardState[' ']);   // up
	isDown_     = (1 < (int)keyboardState['Z']);   // down



	// handle the position changes
	if (isForward_)
	{
		this->AdjustPosition(GetForwardVector() * movingSpeed_ * frameTime_);
	}
	if (isBackward_)
	{
		this->AdjustPosition(GetBackwardVector() * movingSpeed_ * frameTime_);
	}
	if (isLeft_)
	{
		this->AdjustPosition(GetLeftVector() * movingSpeed_ * frameTime_);
	}
	if (isRight_)
	{
		this->AdjustPosition(GetRightVector() * movingSpeed_ * frameTime_);
	}
	if (isUp_)
	{
		this->AdjustPosition(0.0f, movingSpeed_ * frameTime_, 0.0f);
	}
	if (isDown_)
	{
		this->AdjustPosition(0.0f, -movingSpeed_ * frameTime_, 0.0f);
	}


	return;
}