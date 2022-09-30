/////////////////////////////////////////////////////////////////////
// Filename:    cameraclass.cpp
// Revising:    01.04.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

// Initialize the position and rotation of the camera to be at the origin of the scene
CameraClass::CameraClass(void)
{
	m_posX = 0.0f;
	m_posY = 0.0f;
	m_posZ = 0.0f;

	m_rotX = 0.0f;
	m_rotY = 0.0f;
	m_rotZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass(void)
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;

	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotX = x;
	m_rotY = y;
	m_rotZ = z;

	return;
}

D3DXVECTOR3 CameraClass::GetPosition(void)
{
	return D3DXVECTOR3(m_posX, m_posY, m_posZ);
}

D3DXVECTOR3 CameraClass::GetRotation(void)
{
	return D3DXVECTOR3(m_rotX, m_rotY, m_rotZ);
}

// uses the position and rotation of the camera to build and update the view matrix
void CameraClass::Render(void)
{
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;
	D3DXVECTOR3 up = { 0.0f, 1.0f, 0.0f };              // Setup the vector that points upwards
	D3DXVECTOR3 position = { m_posX, m_posY, m_posZ };  // Setup the position of the camera
	//D3DXVECTOR3 position = { 0.0f, 0.0f, 10.0f };
	D3DXVECTOR3 lookAt = { 0.0f, 0.0f, 1.0f };          // Setup where the camera is looking by default

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians
	pitch = m_rotX * 0.0174532925f;
	yaw   = m_rotY * 0.0174532925f;
	roll  = m_rotZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values
	//D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix
	// so the view is correctly rotated at the origin
	//D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	//D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer
	//lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
	
	return;
} // Render

// return the view matrix
void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}