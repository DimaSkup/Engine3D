////////////////////////////////////////////////////////////////////
// Filename:     EditorCamera.cpp
// Description:  an implementation of the EditorCamera class;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#include "EditorCamera.h"


// the class constructor initialize the private member variables to zero to start with
EditorCamera::EditorCamera(void)
{
	position_ = { 0.0f, 0.0f, -10.0f }; // x,y,z position
	m_moveCommand = { 0.0f, 0.0f, 0.0f };

	m_frameTime = 0.0f;
	m_turnSpeed = 0.0f;
	m_movementSpeed = 0.1f;

	m_pitch = 0.0f;
	m_yaw = 0.0f;
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

// get the current position
void EditorCamera::GetPosition(DirectX::XMFLOAT3& position)
{
	position = position_;

	return;
}

// get the current rotation (in radians)
void EditorCamera::GetRotation(DirectX::XMFLOAT2& rotation)
{
	rotation.x = m_pitch;
	rotation.y = m_yaw;
	return;
}


// set the current position
void EditorCamera::SetPosition(float posX, float posY, float posZ)
{
	position_ = { posX, posY, posZ };
}

// set the current rotation (in radians)
void EditorCamera::SetRotation(float pitch, float yaw)
{
	m_pitch = pitch;
	m_yaw = yaw;
}


// handles and updates the position and orientation
// rotation.x -- it is a rotation around X-axis (vertical rotation)
// rotation.y -- it is a rotation around Y-axis (horizontal rotation)
void EditorCamera::HandleMovement(KeyboardClass& keyboard, MouseClass& mouse)
{
	this->HandlePosition(keyboard);
	this->HandleRotation(keyboard);

	return;
} // HandleMovement()



/////////////////////////////////////////////////////////////////////////////////////////
//
//
//                                PRIVATE FUNCTIONS
//
//
/////////////////////////////////////////////////////////////////////////////////////////

// handles the changing of the camera position
void EditorCamera::HandlePosition(KeyboardClass& keyboard)
{
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		// handle the position changes
		KeyPressed(keycode);


		this->calcNewPosition();
	}

	return;
}

// handles the changing of the camera rotation
void EditorCamera::HandleRotation(KeyboardClass& keyboard)
{
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		// handle the rotation changes
		switch (keycode)
		{
		case KEY_UP:
			this->calcTurnSpeed(true);

			m_pitch += m_turnSpeed;  // update the rotation using the turning speed

			if (m_pitch > DirectX::XM_PIDIV2)
			{
				m_pitch = DirectX::XM_PIDIV2;
			}
			break;

		case KEY_DOWN:
			this->calcTurnSpeed(true);

			m_pitch -= m_turnSpeed;  // update the rotation using the turning speed

			if (m_pitch < -DirectX::XM_PIDIV2)
			{
				m_pitch = -DirectX::XM_PIDIV2;
			}
			break;
		case KEY_LEFT:
			this->calcTurnSpeed(true);

			m_yaw -= m_turnSpeed;  // update the rotation using the turning speed

			if (m_yaw < 0.0f)
			{
				m_yaw += DirectX::XM_2PI;
			}
			break;
		case KEY_RIGHT:
			this->calcTurnSpeed(true);

			m_yaw += m_turnSpeed;  // update the rotation using the turning speed

			if (m_yaw > DirectX::XM_2PI)
			{
				m_yaw -= DirectX::XM_2PI;
			}
			break;
		default: // if we aren't pressing any keybutton so the camera movement speed is decreasing
			this->calcTurnSpeed(false);
		}
	}

	return;
}


void EditorCamera::calcNewPosition(void)
{

	// make sure that 45 degree cases are not faster
	DirectX::XMFLOAT3 command = m_moveCommand;
	DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(&command);

	if (fabsf(command.x) > 0.1f || fabsf(command.y) > 0.1f || fabs(command.z) > 0.1f)
	{
		vector = DirectX::XMVector3Normalize(vector);
		DirectX::XMStoreFloat3(&command, vector);
	}

	// rotate command to align with our direction (world coordinates)
	DirectX::XMFLOAT3 wCommand;
	wCommand.x = command.x * cosf(m_yaw) - command.y * sinf(m_yaw);
	wCommand.y = command.x * sinf(m_yaw) + command.y * cosf(m_yaw);
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

	OutputDebugStringA("kek");
	m_moveCommand = { 0.0f, 0.0f, 0.0f };

	return;
} // calcNewPosition()

// calculate the camera movement speed which is based on the frame time,
// direction of  movement, input devices movement, ect;
// takes as an input parameter a flag according to which we define increasing or
// decreasing of the camera movement speed;
void EditorCamera::calcTurnSpeed(bool increase)
{
	m_turnSpeed = (increase) ? 0.002f : 0.0f;
}


void EditorCamera::KeyPressed(UINT keyCode)
{
	// figure out the command from the keyboard
	switch (keyCode)
	{
	case KEY_W:           // forward
		m_moveCommand.y += 1.0f;
		break;
	case KEY_S:           // backward
		m_moveCommand.y -= 1.0f;
		break;

	case KEY_A:           // left
		m_moveCommand.x += 1.0f;
		break;
	case KEY_D:           // right
		m_moveCommand.x -= 1.0f;
		break;

		/*
		if (m_up)
		moveCommand.z += 1.0f;
		if (m_down)
		moveCommand.z -= 1.0f;
		*/
	}
} // KeyPressed()