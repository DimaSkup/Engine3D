#include "movelookcontroller.h"

bool MoveLookController::Initialize(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	m_lookLastPoint = { 0, 0 };
	m_moveCommand = { 0.0f, 0.0f, 0.0f }; // need to init this as it is reset every time
	setPosition({ 0.0f, 0.0f, -7.0f });
	setOrientation(0.0f, 0.0f);           // look straight ahead when the app starts

	// set this class object as a listener of input devices events
	Window::Get()->GetInputManager()->AddInputListener(this);

	return true;
} // Initialize()

// here we compute the new look point position info we want to pass to
// the app for updating the view matrix before projection into the viewport
void MoveLookController::Update(void)
{
	// poll our state bits that are set by the keyboard input events
	if (m_forward)
		m_moveCommand.y += 1.0f;
	if (m_back)
		m_moveCommand.y -= 1.0f;

	if (m_left)
		m_moveCommand.x += 1.0f;
	if (m_right)
		m_moveCommand.x -= 1.0f;

	if (m_up)
		m_moveCommand.z += 1.0f;
	if (m_down)
		m_moveCommand.z -= 1.0f;


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
	wCommand.x = wCommand.x * MOVEMENT_GAIN;
	wCommand.y = wCommand.y * MOVEMENT_GAIN;
	wCommand.z = wCommand.z * MOVEMENT_GAIN;

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

	// clear movement input accumulator for use during the next frame
	m_moveCommand = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

}


// memory allocation
void* MoveLookController::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the object");
		return nullptr;
	}

	return ptr;
}

void MoveLookController::operator delete(void* ptr)
{
	_aligned_free(ptr);
	return;
}

// accessor to set the position of the controller
void MoveLookController::setPosition(_In_ DirectX::XMFLOAT3 pos)
{
	m_position = pos;
}

// accessor to set the orientation of the controller
void MoveLookController::setOrientation(_In_ float pitch, _In_ float yaw)
{
	m_pitch = pitch;
	m_yaw = yaw;
}

// returns the position of the controller object
DirectX::XMFLOAT3 MoveLookController::GetPosition(void)
{
	return m_position;
}

// calculates and returns the point at which the camera controller is facing
DirectX::XMFLOAT3 MoveLookController::GetLookAtPoint(void)
{
	float y = sinf(m_pitch);     // vertical
	float r = cosf(m_pitch);     // in the plane
	float z = r * cosf(m_yaw);   // fwd-back
	float x = r * sinf(m_yaw);   // left-right

	DirectX::XMFLOAT3 result(x, y, z);

	result.x += m_position.x;
	result.y += m_position.y;
	result.z += m_position.z;

	// return m_position + DirectX::XMFLOAT3(x, y, z);
	return result;
} // getLookAtPoint()




bool MoveLookController::MousePressed(const MouseClickEvent& mouseData)
{
	DirectX::XMFLOAT2 position = { static_cast<float>(mouseData.x), static_cast<float>(mouseData.y) };

	// if the current mouse position isn't the same as before
	if ((position.x != m_lookLastPoint.x) && (position.y != m_lookLastPoint.y))
	{
		m_lookLastPoint = { position.x, position.y };
		m_activeMouseBtn = mouseData.code;
		m_lookLastDelta.x = m_lookLastDelta.y = 0;
	}

	return false;
} // MousePressed()

bool MoveLookController::MouseReleased(const MouseClickEvent& mouseData)
{
	// just do nothing

	return false;
} // MouseReleased()

// here we handle mouse moving event;
// this function is called from the InputManager class
bool MoveLookController::MouseMove(const MouseMoveEvent& mouseData)
{
	DirectX::XMFLOAT2 position = { static_cast<float>(mouseData.x), static_cast<float>(mouseData.y) };

	// look control
	DirectX::XMFLOAT2 mouseDelta;                   // how far did pointer move
	mouseDelta.x = position.x - m_lookLastPoint.x;
	mouseDelta.y = position.y - m_lookLastPoint.y;

	DirectX::XMFLOAT2 rotationDelta;                // scale for control sensivity
	rotationDelta.x = mouseDelta.x * ROTATION_GAIN;
	rotationDelta.y = mouseDelta.y * ROTATION_GAIN;

	m_lookLastPoint = { position.x, position.y }; // save for the next time through

	// update our orientation base on the command
	m_yaw += rotationDelta.x;   // yaw is defined as CCW (wtf is this?) around the y-axis
	m_pitch -= rotationDelta.y; // mouse y increases down, but pitch increases up

	// limit the pitch so straight up or straight down
	m_pitch = static_cast<float>__max(-DirectX::XM_PI / 2.0f, m_pitch);
	m_pitch = static_cast<float>__min(+DirectX::XM_PI / 2.0f, m_pitch);

	//Log::Get()->Debug("mouse pos: %d::%d", mouseData.x, mouseData.y);

	return false;
} // MouseMove()


bool MoveLookController::KeyPressed(const KeyButtonEvent& keyData)
{
	// figure out the command from the keyboard
	switch (keyData.code)
	{
	case KEY_W:           // forward
		m_forward = true;
		break;
	case KEY_A:           // left
		m_left = true;
		break;
	case KEY_S:           // backward
		m_back = true;
		break;
	case KEY_D:           // right
		m_right = true;
		break;
	}

	return false;
} // KeyPressed()


bool MoveLookController::KeyReleased(const KeyButtonEvent& keyData)
{
	// figure out the command from the keyboard
	switch (keyData.code)
	{
	case KEY_W:           // forward
		m_forward = false;
		break;
	case KEY_A:           // left
		m_left = false;
		break;
	case KEY_S:           // backward
		m_back = false;
		break;
	case KEY_D:           // right
		m_right = false;
		break;
	}

	return false;
} // KeyReleased