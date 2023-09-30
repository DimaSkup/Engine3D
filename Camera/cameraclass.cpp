/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Created:  04.04.22
// Revising: 20.12.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
	: pos_ (0.0f, 0.0f, 0.0f),
	  rot_ (0.0f, 0.0f, 0.0f)

{
	this->posVector_ = XMLoadFloat3(&this->pos_);
	this->rotVector_ = XMLoadFloat3(&this->rot_);

	this->UpdateViewMatrix();
}


CameraClass::~CameraClass()
{
}



////////////////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC GETTTERS
////////////////////////////////////////////////////////////////////////////////////////////

const XMMATRIX & CameraClass::GetViewMatrix() const
{
	// return the view matrix
	return this->viewMatrix_;
}

/////////////////////////////////////////////////

const XMMATRIX & CameraClass::GetProjectionMatrix() const
{
	return this->projectionMatrix_;
}

/////////////////////////////////////////////////

const XMVECTOR & CameraClass::GetPositionVector() const
{
	// get pos vector
	return this->posVector_;
}

/////////////////////////////////////////////////

const XMFLOAT3 & CameraClass::GetPositionFloat3() const
{
	// get pos XMFLOAT3
	return this->pos_;
}

/////////////////////////////////////////////////

const XMVECTOR & CameraClass::GetRotationVector() const
{
	// get rotation vector
	return this->rotVector_;
}

/////////////////////////////////////////////////

const XMFLOAT3 & CameraClass::GetRotationFloat3() const
{
	// get rotation float3 (in radians)
	return this->rot_;
}

/////////////////////////////////////////////////

// get rotation float3 (in degrees)
XMFLOAT3 CameraClass::GetRotationFloat3InDegrees()
{
	float rotX = DirectX::XMConvertToDegrees(this->rot_.x);
	float rotY = DirectX::XMConvertToDegrees(this->rot_.y);
	float rotZ = DirectX::XMConvertToDegrees(this->rot_.z);

	return { rotX, rotY, rotZ };
}

/////////////////////////////////////////////////

const XMVECTOR & CameraClass::GetForwardVector()
{
	return this->vecForward_;
}

/////////////////////////////////////////////////

const XMVECTOR & CameraClass::GetRightVector()
{
	return this->vecRight_;
}

/////////////////////////////////////////////////

const XMVECTOR & CameraClass::GetBackwardVector()
{
	return this->vecBackward_;
}

/////////////////////////////////////////////////

const XMVECTOR & CameraClass::GetLeftVector()
{
	return this->vecLeft_;
}




////////////////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////////////////


void CameraClass::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	// set up the projection matrix

	//float fovRadians = static_cast<float>(D3DX_PI) / 4.0f;
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix_ = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

/////////////////////////////////////////////////

void CameraClass::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos_, pos);
	this->posVector_ = pos;
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::SetPosition(float x, float y, float z)
{
	this->pos_ = XMFLOAT3(x, y, z);
	this->posVector_ = XMLoadFloat3(&this->pos_);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector_ += pos;
	XMStoreFloat3(&this->pos_, this->posVector_);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::AdjustPosition(float x, float y, float z)
{
	this->pos_.x += x;
	this->pos_.y += y;
	this->pos_.z += z;
	this->posVector_ = XMLoadFloat3(&this->pos_);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::SetRotation(const XMVECTOR & rot)
{
	this->rotVector_ = rot;
	XMStoreFloat3(&this->rot_, rot);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::SetRotation(float x, float y, float z)
{
	this->rot_ = XMFLOAT3(x, y, z);
	this->rotVector_ = XMLoadFloat3(&this->rot_);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector_ += rot;
	XMStoreFloat3(&this->rot_, this->rotVector_);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

void CameraClass::AdjustRotation(float pitch, float yaw, float roll)
{
	this->rot_.x = pitch;
	this->rot_.y = yaw;
	this->rot_.z = roll;
	this->rotVector_ = XMLoadFloat3(&this->rot_);
	this->UpdateViewMatrix();
}

/////////////////////////////////////////////////

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

/////////////////////////////////////////////////






// memory allocation (we need it because we use DirectX::XM-objects)
void* CameraClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error(THIS_FUNC, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

void CameraClass::operator delete(void* p)
{
	_aligned_free(p);
}






////////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


void CameraClass::UpdateViewMatrix()
{
	//
	// updates the view matrix and also updates the movement vectors
	//

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

	return;

} // end UpdateViewMatrix