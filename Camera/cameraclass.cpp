/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Created:  04.04.22
// Revising: 20.12.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	this->pos_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector_ = XMLoadFloat3(&this->pos_);
	this->rot_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector_ = XMLoadFloat3(&this->rot_);

	this->UpdateViewMatrix();
}


// set up the projection matrix
void CameraClass::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	//float fovRadians = static_cast<float>(D3DX_PI) / 4.0f;
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix_ = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}


// ---------------------------- PUBLIC FUNCTIONS ------------------------------------- //


//
// ---- GETTERS ----
//

const XMMATRIX & CameraClass::GetViewMatrix() const
{
	return this->viewMatrix_;
}

const XMMATRIX & CameraClass::GetProjectionMatrix() const
{
	return this->projectionMatrix_;
}


// get pos vector
const XMVECTOR & CameraClass::GetPositionVector() const
{
	return this->posVector_;
}

// get pos float3
const XMFLOAT3 & CameraClass::GetPositionFloat3() const
{
	return this->pos_;
}

// get rotation vector
const XMVECTOR & CameraClass::GetRotationVector() const
{
	return this->rotVector_;
}

// get rotation float3
const XMFLOAT3 & CameraClass::GetRotationFloat3() const
{
	return this->rot_;
}




//
// ---- SETTERS ----
//

void CameraClass::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos_, pos);
	this->posVector_ = pos;
	this->UpdateViewMatrix();
}

void CameraClass::SetPosition(float x, float y, float z)
{
	this->pos_ = XMFLOAT3(x, y, z);
	this->posVector_ = XMLoadFloat3(&this->pos_);
	this->UpdateViewMatrix();
}


void CameraClass::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector_ += pos;
	XMStoreFloat3(&this->pos_, this->posVector_);
	this->UpdateViewMatrix();
}

void CameraClass::AdjustPosition(float x, float y, float z)
{
	this->pos_.x += x;
	this->pos_.y += y;
	this->pos_.z += z;
	this->posVector_ = XMLoadFloat3(&this->pos_);
	this->UpdateViewMatrix();
}



void CameraClass::SetRotation(const XMVECTOR & rot)
{
	this->rotVector_ = rot;
	XMStoreFloat3(&this->rot_, rot);
	this->UpdateViewMatrix();
}

void CameraClass::SetRotation(float x, float y, float z)
{
	this->rot_ = XMFLOAT3(x, y, z);
	this->rotVector_ = XMLoadFloat3(&this->rot_);
	this->UpdateViewMatrix();
}


void CameraClass::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector_ += rot;
	XMStoreFloat3(&this->rot_, this->rotVector_);
	this->UpdateViewMatrix();
}

void CameraClass::AdjustRotation(float pitch, float yaw, float roll)
{
	this->rot_.x = pitch;
	this->rot_.y = yaw;
	this->rot_.z = roll;
	this->rotVector_ = XMLoadFloat3(&this->rot_);
	this->UpdateViewMatrix();
}


void CameraClass::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	// verify that look at pos is not the same as camera position. They cannot be the same as that wouldn't make sense and would result in undefined behaviour
	if (lookAtPos.x == this->pos_.x &&
		lookAtPos.y == this->pos_.y &&
		lookAtPos.z == this->pos_.z)
		return;

	lookAtPos.x = this->pos_.x - lookAtPos.x;
	lookAtPos.y = this->pos_.y - lookAtPos.y;
	lookAtPos.z = this->pos_.z - lookAtPos.z;


	// calculate the pitch
	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	// calculate the yaw
	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}



const XMVECTOR & CameraClass::GetForwardVector()
{
	return this->vecForward_;
}

const XMVECTOR & CameraClass::GetRightVector()
{
	return this->vecRight_;
}

const XMVECTOR & CameraClass::GetBackwardVector()
{
	return this->vecBackward_;
}

const XMVECTOR & CameraClass::GetLeftVector()
{
	return this->vecLeft_;
}





// memory allocation (we need it because we use DirectX::XM-objects)
void* CameraClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void CameraClass::operator delete(void* p)
{
	_aligned_free(p);
}





// ---------------------------- PRIVATE FUNCTIONS ------------------------------------ //

// updates view matrix and also updates the movement vectors
void CameraClass::UpdateViewMatrix()
{
	// calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot_.x, this->rot_.y, this->rot_.z);
	//calculate unit vector of camera target based on camera forward value transformed by camera rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR_, camRotationMatrix);
	// adjust camera target to be offset by the camera's current position
	camTarget += this->posVector_;

	// calculate up direction based on the current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR_, camRotationMatrix);
	// rebuild view matrix
	this->viewMatrix_ = XMMatrixLookAtLH(this->posVector_, camTarget, upDir);



	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot_.y, 0.0f);
	this->vecForward_  = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR_, vecRotationMatrix);
	this->vecBackward_ = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR_, vecRotationMatrix);
	this->vecLeft_     = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR_, vecRotationMatrix);
	this->vecRight_    = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR_, vecRotationMatrix);
}

/*

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
	position_ = position;
}

// set the camera rotation angles (takes in radians)
// and calculates the currect "look at" point
void CameraClass::SetRotation(DirectX::XMFLOAT2 rotation)
{
	// calculate a look at point for this frame by passed rotation
	pitch_ = rotation.x;
	yaw_ = rotation.y;
}


// set a look at point coordinates
void CameraClass::SetLookAtPoint(DirectX::XMFLOAT3 lookAtPoint)
{

	lookAtPoint_ = lookAtPoint;
}

// set the up direction of the camera
void CameraClass::SetDirectionUp(DirectX::XMFLOAT3 up)
{
	up_ = up;
}




// --------------------------------- GETTERS ----------------------------------------- //

// get the current position of the camera (return a XMFLOAT3 object)
const DirectX::XMFLOAT3& CameraClass::GetPosition()
{
	return position_;
}


// get the current position of the camera (set the input parameter with the current position values)
void CameraClass::GetPosition(DirectX::XMFLOAT3& position)
{
	position = position_;
}

// get the current rotation angles of the camera (in radians)
DirectX::XMFLOAT2 CameraClass::GetRotation(void)
{
	return DirectX::XMFLOAT2{ pitch_, yaw_ };
}

// get the current look at point coordinates
DirectX::XMFLOAT3 CameraClass::GetLookAtPoint(void)
{
	return lookAtPoint_;
}

// get the current up direction of the camera
DirectX::XMFLOAT3 CameraClass::GetDirectionUp(void)
{
	return up_;
}

// returns the camera view matrix
void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = viewMatrix_;
}



// ---------------------------------- RENDER ----------------------------------------- //

// calculates the camera view matrix
void CameraClass::Render(void)
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->GetDirectionUp());    // Setup the vector that points upwards
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&this->GetPosition()); // Setup the position of the camera
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&this->GetLookAtPoint());   // Setup where the camera is looking at

	viewMatrix_ = DirectX::XMMatrixLookAtLH(position, lookAt, up);

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

		viewMatrix_ *= transform;
	}
	


	return;
}




/////////////////////////////////////////////////////////////////////////////////////////

// if we did some moving or rotation we need to recalculate the look at point
void CameraClass::CalculateNewLookAtPoint()
{
	float y = sinf(yaw_);     // vertical
	float r = cosf(yaw_);     // in the plane
	float z = r * cosf(pitch_);   // fwd-back
	float x = r * sinf(pitch_);   // left-right

	DirectX::XMFLOAT3 result(x, y, z);

	result.x += position_.x;
	result.y += position_.y;
	result.z += position_.z;

	this->SetLookAtPoint({ result.x, result.y, result.z }); // store the current look at point

	return;
}

*/


