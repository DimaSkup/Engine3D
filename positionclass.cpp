////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
// Revising: 24.09.22
////////////////////////////////////////////////////////////////////
#include "positionclass.h"


// the class constructor initialize the private member variables to zero to start with
PositionClass::PositionClass(void)
{
	m_position = { 0.0f, 0.0f, -10.0f }; // x,y,z position
	m_moveCommand = { 0.0f, 0.0f, 0.0f };

	m_frameTime = 0.0f;
	m_turnSpeed = 0.0f;
	m_movementSpeed = 0.1f;
	
	m_pitch = 0.0f;
	m_yaw = 0.0f;
}

// we don't use the copy constructor and destructor in this class
PositionClass::PositionClass(const PositionClass& copy) {}
PositionClass::~PositionClass(void) {}




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
// this class to move the viewing position
void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}

// set the current position
DirectX::XMFLOAT3 PositionClass::GetPosition(void)
{
	return DirectX::XMFLOAT3{ m_position.x, m_position.y, m_position.z };
}

// get the current rotation (in radians)
DirectX::XMFLOAT2 PositionClass::GetRotation(void)
{
	return DirectX::XMFLOAT2{ m_pitch, m_yaw };
}


// set the current position
void PositionClass::SetPosition(float posX, float posY, float posZ)
{
	m_position = { posX, posY, posZ };
}

// set the current rotation (in radians)
void PositionClass::SetRotation(float pitch, float yaw)
{
	m_pitch = pitch;
	m_yaw = yaw;
}


// handles and updates the position and orientation
// rotation.x -- it is a rotation around X-axis (vertical rotation)
// rotation.y -- it is a rotation around Y-axis (horizontal rotation)
void PositionClass::HandleMovement(InputClass* input)
{
	int activeKeyCode = input->GetActiveKeyCode();
	
	
	// handle the position changes
	KeyPressed(activeKeyCode);

	

	this->calcNewPosition();
	//KeyReleased(activeKeyCode)


	// handle the rotation changes
	switch (activeKeyCode)
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

	return;
} // HandleMovement()



/////////////////////////////////////////////////////////////////////////////////////////
//
//
//                                PRIVATE FUNCTIONS
//
//
/////////////////////////////////////////////////////////////////////////////////////////

void PositionClass::calcNewPosition(void)
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
	m_position.x += velocity.x;
	m_position.y += velocity.y;
	m_position.z += velocity.z;
	Log::Get()->Print("MOVE: %f %f %f", m_moveCommand.x, m_moveCommand.y, m_moveCommand.z);
	m_moveCommand = { 0.0f, 0.0f, 0.0f };

	return;
}

// calculate the camera movement speed which is based on the frame time,
// direction of  movement, input devices movement, ect;
// takes as an input parameter a flag according to which we define increasing or
// decreasing of the camera movement speed;
void PositionClass::calcTurnSpeed(bool increase)
{
	m_turnSpeed = (increase) ? 0.002f : 0.0f;

	/*
	float maxTurnSpeed = m_frameTime * 0.10f;
	float stride = m_frameTime * 0.01f;

	if (increase)  // increasing of the camera movement speed
	{
		m_turnSpeed += stride;

		// if the current turn speed is higher that the maximum camera speed
		if (m_turnSpeed > maxTurnSpeed)  
		{
			m_turnSpeed = maxTurnSpeed;
		}
	}
	else  // decreasing of the camera movement speed
	{
		m_turnSpeed -= stride;

		// if the current turn speed is lower that the minimum camera speed
		if (m_turnSpeed < 0.0f)
		{
			m_turnSpeed = 0.0f;
		}
	}

	return;
	*/
}  // calcCurTurnSpeed()


void PositionClass::KeyPressed(UINT keyCode)
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