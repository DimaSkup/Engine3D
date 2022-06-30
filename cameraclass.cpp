/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Revising: 07.04.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

CameraClass::CameraClass(void)
{
	m_posX = m_posY = m_posZ = 0.0f;
	m_rotX = m_rotY = m_rotZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& another)
{
}

CameraClass::~CameraClass(void)
{
}


/////////////////////////////////////////////////////////////////////
//
//                   PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////

void CameraClass::SetPosition(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotX = x;
	m_rotY = y;
	m_rotZ = z;
}

DirectX::XMFLOAT3 CameraClass::GetPosition(void)
{
	return DirectX::XMFLOAT3{ m_posX, m_posY, m_posZ };
}

DirectX::XMFLOAT3 CameraClass::GetRotation(void)
{
	return DirectX::XMFLOAT3{ m_rotX, m_rotY, m_rotZ };
}

void CameraClass::Render(void)
{
	//float yaw, pitch, roll;
	//D3DXMATRIX rotationMatrix;
	DirectX::XMVECTOR up = { 0.0f, 1.0f, 0.0f };              // Setup the vector that points upwards
	DirectX::XMVECTOR position = { m_posX, m_posY, m_posZ };  // Setup the position of the camera
	DirectX::XMVECTOR lookAt = { 0.0f, 0.0f, 1.0f };          // Setup where the camera is looking by default

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