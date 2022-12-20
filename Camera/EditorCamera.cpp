////////////////////////////////////////////////////////////////////
// Filename:     EditorCamera.cpp
// Description:  an implementation of the EditorCamera class;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#include "EditorCamera.h"


// the class constructor initialize the private member variables to zero to start with
EditorCamera::EditorCamera(void)
{
	moveCommand_ = { 0.0f, 0.0f, 0.0f };

	m_frameTime = 0.0f;
	
	movingSpeed_ = 0.01f;
	rotationSpeed_ = SETTINGS::GetSettings()->CAMERA_SENSITIVITY;
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
void EditorCamera::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}



// handles and updates the position and orientation
// rotation.x -- it is a rotation around X-axis (vertical rotation)
// rotation.y -- it is a rotation around Y-axis (horizontal rotation)
void EditorCamera::HandleMovement(KeyboardEvent& kbe, MouseEvent& me, MouseClass& mouse)
{
	BYTE lpKeyState[256];
	GetKeyboardState(lpKeyState);


	this->HandlePosition(lpKeyState);
	this->HandleRotation(me, mouse);


	return;
} // HandleMovement()



/////////////////////////////////////////////////////////////////////////////////////////
//
//
//                                PRIVATE FUNCTIONS
//
//
/////////////////////////////////////////////////////////////////////////////////////////

bool EditorCamera::IsMovingNow()
{
	return (isForward_ || isBackward_ || isRight_ || isLeft_);
}

bool EditorCamera::IsRotationNow()
{
	return (isRotateUp_ || isRotateDown_ || isRotateRight_ || isRotateLeft_);
}




// handles the changing of the camera position
void EditorCamera::HandlePosition(const BYTE* keyboardState)
{

	isForward_  = (1 < (int)keyboardState['W']) ? true : false;   // W
	isLeft_     = (1 < (int)keyboardState['A']) ? true : false;   // A
	isBackward_ = (1 < (int)keyboardState['S']) ? true : false;   // S
	isRight_    = (1 < (int)keyboardState['D']) ? true : false;   // D
	isUp_       = (1 < (int)keyboardState[' ']) ? true : false;   // up
	isDown_     = (1 < (int)keyboardState['Z']) ? true : false;   // down


	// handle the position changes

	if (isForward_)
	{
		this->AdjustPosition(GetForwardVector() * rotationSpeed_);
	}
	if (isBackward_)
	{
		this->AdjustPosition(GetBackwardVector() * rotationSpeed_);
	}
	if (isLeft_)
	{
		this->AdjustPosition(GetLeftVector() * rotationSpeed_);
	}
	if (isRight_)
	{
		this->AdjustPosition(GetRightVector() * rotationSpeed_);
	}
	if (isUp_)
	{
		this->AdjustPosition(0.0f, rotationSpeed_, 0.0f);
	}
	if (isDown_)
	{
		this->AdjustPosition(0.0f, -rotationSpeed_, 0.0f);
	}


	return;
}

// handles the changing of the camera rotation
void EditorCamera::HandleRotation(MouseEvent& me, MouseClass& mouse)
{


	/*
	


	if (IsRotationNow())
	{
		// handle the rotation changes
		if (isRotateUp_)
		{
			yaw_ += m_turnSpeed;  // update the rotation using the turning speed

			if (yaw_ > DirectX::XM_PIDIV2)
			{
				yaw_ = DirectX::XM_PIDIV2;
			}
		}


		if (isRotateDown_)
		{
			yaw_ -= m_turnSpeed;  // update the rotation using the turning speed

			if (yaw_ < -DirectX::XM_PIDIV2)
			{
				yaw_ = -DirectX::XM_PIDIV2;
			}
		}


		if (isRotateLeft_)
		{
			pitch_ -= m_turnSpeed;  // update the rotation using the turning speed

			if (pitch_ < 0.0f)
			{
				pitch_ += DirectX::XM_2PI;
			}
		}


		if (isRotateRight_)
		{
			pitch_ += m_turnSpeed;  // update the rotation using the turning speed

			if (pitch_ > DirectX::XM_2PI)
			{
				pitch_ -= DirectX::XM_2PI;
			}
		}

		CalculateNewLookAtPoint();
	}
	*/

	return;
}
/*


void EditorCamera::calcNewPosition(void)
{
	if (isForward_) // if we're moving ahead
	{
		moveCommand_.y += 1.0f;
	}

	if (isBack_) // if we are moving backward
	{
		moveCommand_.y -= 1.0f;
	}

	if (isRight_)
	{
		moveCommand_.x -= 1.0f;
	}

	if (isLeft_)
	{
		moveCommand_.x += 1.0f;
	}

	// make sure that 45 degree cases are not faster
	DirectX::XMFLOAT3 command = moveCommand_;
	DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(&command);

	if (fabsf(command.x) > 0.1f || fabsf(command.y) > 0.1f || fabs(command.z) > 0.1f)
	{
		vector = DirectX::XMVector3Normalize(vector);
		DirectX::XMStoreFloat3(&command, vector);
	}

	// rotate command to align with our direction (world coordinates)
	DirectX::XMFLOAT3 wCommand;
	wCommand.x = command.x * cosf(pitch_) - command.y * sinf(pitch_);
	wCommand.y = command.x * sinf(pitch_) + command.y * cosf(pitch_);
	wCommand.z = command.z;

	// scale for sensitivity adjestment
	wCommand.x = wCommand.x * m_movementSpeed;
	wCommand.y = wCommand.y * m_movementSpeed;
	wCommand.z = wCommand.z * m_movementSpeed;

	// our velocity is based on the command;
	// also note that y is the up-down axis
	DirectX::XMFLOAT3 velocity;
	velocity.x = -wCommand.x;
	velocity.z = wCommand.y;
	velocity.y = wCommand.z;

	// integrate
	position_.x += velocity.x;
	position_.y += velocity.y;
	position_.z += velocity.z;

	moveCommand_ = { 0.0f, 0.0f, 0.0f };

	return;
} // calcNewPosition()

*/