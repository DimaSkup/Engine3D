/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Revising: 24.09.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

CameraClass::CameraClass(void)
{
	SetPosition({ 0.0f, 0.0f, 0.0f });    // x,y,z position
	SetRotation({ 0.0f, 0.0f });          // pitch and yaw
	SetDirectionUp({ 0.0f, 1.0f, 0.0f }); //
}

// we don't use the copy construction and destruction in this class
CameraClass::CameraClass(const CameraClass& another) {}
CameraClass::~CameraClass(void) {}

// memory allocation
void* CameraClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void CameraClass::operator delete(void* p)
{
	_aligned_free(p);
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                             PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// --------------------------------- SETTERS ----------------------------------------- //

// set the current position of the camera
void CameraClass::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}

// set the camera rotation angles (takes in radians)
// and calculates the currect "look at" point
void CameraClass::SetRotation(DirectX::XMFLOAT2 rotation)
{
	// calculate a look at point for this frame by passed rotation
	m_pitch = rotation.x;
	m_yaw = rotation.y;

	float y = sinf(m_pitch);     // vertical
	float r = cosf(m_pitch);     // in the plane
	float z = r * cosf(m_yaw);   // fwd-back
	float x = r * sinf(m_yaw);   // left-right

	DirectX::XMFLOAT3 result(x, y, z);

	result.x += m_position.x;
	result.y += m_position.y;
	result.z += m_position.z;

	this->SetLookAtPoint({ result.x, result.y, result.z }); // store the current look at point
}


// set a look at point coordinates
void CameraClass::SetLookAtPoint(DirectX::XMFLOAT3 lookAtPoint)
{
	m_lookAtPoint = lookAtPoint;
}

// set the up direction of the camera
void CameraClass::SetDirectionUp(DirectX::XMFLOAT3 up)
{
	m_up = up;
}




// --------------------------------- GETTERS ----------------------------------------- //

// get the current position of the camera
DirectX::XMFLOAT3 CameraClass::GetPosition(void)
{
	return m_position;
}

// get the current rotation angles of the camera (in radians)
DirectX::XMFLOAT2 CameraClass::GetRotation(void)
{
	return DirectX::XMFLOAT2{ m_pitch, m_yaw };
}

// get the current look at point coordinates
DirectX::XMFLOAT3 CameraClass::GetLookAtPoint(void)
{
	return m_lookAtPoint;
}

// get the current up direction of the camera
DirectX::XMFLOAT3 CameraClass::GetDirectionUp(void)
{
	return m_up;
}

// returns the camera view matrix
void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}



// ---------------------------------- RENDER ----------------------------------------- //

// calculates the camera view matrix
void CameraClass::Render(void)
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->GetDirectionUp());    // Setup the vector that points upwards
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&this->GetPosition()); // Setup the position of the camera
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&this->GetLookAtPoint());   // Setup where the camera is looking at

	m_viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);

	// set camera rotation
	if (false)
	{
		static FLOAT angle = 0;
		angle += 0.005f;

		if (angle >= 360.f)
			angle = 0.0f;

		float zPos = sinf(angle) * 10.0f;

		DirectX::XMMATRIX changePos = DirectX::XMMatrixTranslation(4.0f, 4.0f, 10.0f);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixIdentity();
		//DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);
		DirectX::XMMATRIX transform = rotation * changePos;

		m_viewMatrix *= transform;
	}
	


	return;
}



// IT WAS IN THE RENDER() FUNCTION SO SOME OF THIS SHIT CAN BE USEFUL LATER

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
//m_viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);