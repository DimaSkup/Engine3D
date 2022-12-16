////////////////////////////////////////////////////////////////////
// Filename:     EditorCamera.cpp
// Description:  an implementation of the EditorCamera class;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#include "EditorCamera.h"


// the class constructor initialize the private member variables to zero to start with
EditorCamera::EditorCamera(void)
{
	//camera_.SetPosition({ 0.0f, 0.0f, -7.0f });


	position_ = { 0.0f, 0.0f, -10.0f }; // x,y,z position
	moveCommand_ = { 0.0f, 0.0f, 0.0f };

	m_frameTime = 0.0f;
	m_turnSpeed = 0.0f;
	m_movementSpeed = 0.1f;

	pitch_ = 0.0f;
	yaw_ = 0.0f;

	m_turnSpeed = SETTINGS::GetSettings()->CAMERA_SENSITIVITY;  // setup the camera rotation speed
}

// we don't use the copy constructor and destructor in this class
EditorCamera::EditorCamera(const EditorCamera& copy) {}
EditorCamera::~EditorCamera(void) {}




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
void EditorCamera::HandleMovement(KeyboardEvent& kbe, MouseEvent& me)
{
	
		


	//if (kbe.IsPress())   KeyPressed(kbe.GetKeyCode());
	//if (kbe.IsRelease()) KeyReleased(kbe.GetKeyCode());
	BYTE lpKeyState[256];
	GetKeyboardState(lpKeyState);


	this->HandlePosition(lpKeyState);
	this->HandleRotation(me, lpKeyState);


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
	return (isForward_ || isBack_ || isRight_ || isLeft_);
}

bool EditorCamera::IsRotationNow()
{
	return (isRotateUp_ || isRotateDown_ || isRotateRight_ || isRotateLeft_);
}




// handles the changing of the camera position
void EditorCamera::HandlePosition(const BYTE* keyboardState)
{

	isForward_ = (1 < (int)keyboardState['W']) ? true : false;   // W
	isLeft_    = (1 < (int)keyboardState['A']) ? true : false;   // A
	isBack_    = (1 < (int)keyboardState['S']) ? true : false;   // S
	isRight_   = (1 < (int)keyboardState['D']) ? true : false;   // D


	// handle the position changes
	if (IsMovingNow())
	{
		//Log::Print("WASD: %d %d %d %d", (int)isForward_, (int)isLeft_, (int)isBack_, (int)isRight_);
		this->calcNewPosition();
		CalculateNewLookAtPoint();
	}

	return;
}

// handles the changing of the camera rotation
void EditorCamera::HandleRotation(MouseEvent& me, const BYTE* keyboardState)
{

	if (false)
	{

		int changeX = me.GetPosX();
		int changeY = me.GetPosY();

		std::string outmsg{ "X: " };
		outmsg += std::to_string(changeX);
		outmsg += ", Y: ";
		outmsg += std::to_string(changeY);
		outmsg += "\n";
		

		isRotateLeft_ = (changeX < 0) ? true : false;   // left
		isRotateRight_ = (changeX > 0) ? true : false;   // right
		isRotateUp_ = (changeY < 0) ? true : false;   // up
		isRotateDown_ = (changeY > 0) ? true : false;   // down


		Log::Debug("l r u d: %d %d %d %d", (int)isRotateLeft_, (int)isRotateRight_, (int)isRotateUp_, (int)isRotateDown_);
	}

	if (true)
	{
		isRotateUp_ = (1 < (int)keyboardState[KEY_UP]) ? true : false;   // up
		isRotateDown_ = (1 < (int)keyboardState[KEY_DOWN]) ? true : false;   // down
		isRotateLeft_ = (1 < (int)keyboardState[KEY_LEFT]) ? true : false;   // left
		isRotateRight_ = (1 < (int)keyboardState[KEY_RIGHT]) ? true : false;   // right
	}

	




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

	return;
}


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