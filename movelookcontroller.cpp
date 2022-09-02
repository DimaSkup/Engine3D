#include "movelookcontroller.h"

bool MoveLookController::Initialize(void)
{
	m_lookLastPoint = { 0, 0 };
	m_moveCommand = { 0.0f, 0.0f, 0.0f }; // need to init this as it is reset every time
	setOrientation(0.0f, 0.0f);           // look straight ahead when the app starts

	// set this class object as a listener of input devices events
	Window::Get()->GetInputManager()->AddInputListener(this);
} // Initialize()

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
DirectX::XMFLOAT3 MoveLookController::getPosition(void)
{
	return m_position;
}

// calculates and returns the point at which the camera controller is facing
DirectX::XMFLOAT3 MoveLookController::getLookAtPoint(void)
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
	// if the current mouse position isn't the same as before
	if ((mouseData.x != m_lookLastPoint.x) && (mouseData.y != m_lookLastPoint.y))
	{
		m_lookLastPoint = { mouseData.x, mouseData.y };
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
	// if the current mouse position isn't the same as before
	if ((mouseData.x != m_lookLastPoint.x) && (mouseData.y != m_lookLastPoint.y))
	{
		// look control
		DirectX::XMFLOAT2 mouseDelta;                   // how far did pointer move
		mouseDelta.x = mouseData.x - m_lookLastPoint.x;
		mouseDelta.y = mouseData.y - m_lookLastPoint.y;

		DirectX::XMFLOAT2 rotationDelta;                // scale for control sensivity
		rotationDelta.x = mouseDelta.x * ROTATION_GAIN;
		rotationDelta.y = mouseDelta.y * ROTATION_GAIN;

		m_lookLastPoint = { mouseData.x, mouseData.y }; // save for the next time through

		// update our orientation base on the command
		m_yaw -= rotationDelta.x;   // yaw is defined as CCW (wtf is this?) around the y-axis
		m_pitch -= rotationDelta.y; // mouse y increases down, but pitch increases up

		// limit the pitch so straight up or straight down
		m_pitch = static_cast<float>__max(-DirectX::XM_PI / 2.0f, m_pitch);
		m_pitch = static_cast<float>__min(+DirectX::XM_PI / 2.0f, m_pitch);
	}

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