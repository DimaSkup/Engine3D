/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Revising: 07.04.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

CameraClass::CameraClass(void)
{
	SetPosition({ 0.0f, 0.0f, 0.0f });
	SetRotation({ 0.0f, 0.0f, 0.0f });
	SetDirectionUp({ 0.0f, 1.0f, 0.0f });
}

// we don't use the copy construction and destruction in this class
CameraClass::CameraClass(const CameraClass& another) {}
CameraClass::~CameraClass(void) {}


/////////////////////////////////////////////////////////////////////
//
//                   PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////

void CameraClass::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}

void CameraClass::SetRotation(DirectX::XMFLOAT3 lookAtPoint)
{
	m_rotation = lookAtPoint;
}

void CameraClass::SetDirectionUp(DirectX::XMFLOAT3 up)
{
	m_up = up;
}

DirectX::XMFLOAT3 CameraClass::GetPosition(void)
{
	return m_position;
}

DirectX::XMFLOAT3 CameraClass::GetRotation(void)
{
	return m_rotation;
}

DirectX::XMFLOAT3 CameraClass::GetDirectionUp(void)
{
	return m_up;
}

void CameraClass::SetViewParameters(DirectX::XMFLOAT3 newPosition,
	                                DirectX::XMFLOAT3 newLookAtPoint,
	                                DirectX::XMFLOAT3 newUp)
{
	// set up the camera position parameters
	SetPosition(newPosition);

	// set up the camera look at point parameters
	SetRotation(newLookAtPoint);

	// set up the camera's up direction
	SetDirectionUp(newUp);
}

void CameraClass::Render(void)
{
	//float yaw, pitch, roll;
	//D3DXMATRIX rotationMatrix;
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);              // Setup the vector that points upwards
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_position);  // Setup the position of the camera
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&m_rotation);//{ m_rotX, m_rotY, m_rotZ };          // Setup where the camera is looking by default

	//static FLOAT angle = 0;
	//angle += 0.001;
	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians
	//pitch = m_rotX * 0.0174532925f;
	//yaw = m_rotY * 0.0174532925f;
	//roll = m_rotZ * 0.0174532925f;


	// Create the rotation matrix from the yaw, pitch, and roll values
	//D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	//D3DXMatrixRotationY(&rotationMatrix, angle);

	// Transform the lookAt and up vector by the rotation matrix
	// so the view is correctly rotated at the origin
	//D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	//D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer
	//lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors
	m_viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);

	return;
}

void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}