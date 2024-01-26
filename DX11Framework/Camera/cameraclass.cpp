/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Created:  04.04.22
// Revising: 20.12.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass(const float cameraSpeed, const float cameraSensitivity)
	: GameObject(),
	  movingSpeed_(cameraSpeed),          // a camera movement speed
	  rotationSpeed_(cameraSensitivity)   // a camera turning speed
{
	// create a view matrix for default params of the camera
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

XMFLOAT3 CameraClass::GetRotationFloat3InDegrees()
{
	// get rotation float3 (in degrees)

	return {
		DirectX::XMConvertToDegrees(this->rotation_.x),  // rotation around X-axis
		DirectX::XMConvertToDegrees(this->rotation_.y),  // rotation around Y-axis
		DirectX::XMConvertToDegrees(this->rotation_.z)   // rotation around Z-axis
	};
}

/////////////////////////////////////////////////

void CameraClass::UpdateReflectionViewMatrix(const DirectX::XMFLOAT3 & reflectionPlanePos,
	const DirectX::XMFLOAT3 & relfectionPlaneRotation)
{
	// NOTE: this function only works for the Y axis plane.
	//
	// this function builds a reflection view matrix the same way as
	// the regular UpdateViewMatrix function builds a view matrix. The main difference is
	// that we take as input the height of the object that will act as the Y axis plane
	// and then we use that height to invert the position.y variable for reflection. 
	// We also need to inver the pitch. This will build the reflection view matrix that we
	// can then use in the reflection shader.
	

	XMVECTOR positionVec(XMLoadFloat3(&reflectionPlanePos));
	XMVECTOR lookAtVec;               // where the camera is looking at
	XMVECTOR upVec;                   // upward direction
	XMMATRIX rotationMatrix;

	// local timer							
	DWORD dwTimeCur = GetTickCount();
	static DWORD dwTimeStart = dwTimeCur;

	// update the local timer
	float t = (dwTimeCur - dwTimeStart) / 1000.0f;

#if 0
	// set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotation in radians
	float pitch = relfectionPlaneRotation.x;//-XM_PIDIV2;
	float yaw   = relfectionPlaneRotation.y;
	float roll  = relfectionPlaneRotation.z;
#endif

#if 1
	float pitch = 0;
	float yaw = -XM_PI;//rot_.y;
	float roll  = this->rotation_.z;
#endif
	
	// create the rotation matrix from the yaw, pitch, and roll values
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// transform the lookAtVec and up vector by the rotation matrix so the view is correcly
	// rotated at the origin
	lookAtVec = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR_, rotationMatrix);
	upVec = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR_, rotationMatrix);

	// translate the rotated camera position to the location of the viewer
	lookAtVec = XMVectorAdd(positionVec, lookAtVec);

	// finally create the view matrix from the three updated vectors
	this->reflectionViewMatrix_ = XMMatrixLookAtLH(positionVec, lookAtVec, upVec);

	return;
} // end UpdateReflectionViewMatrix

///////////////////////////////////////////////////////////

void CameraClass::GetReflectionViewMatrix(XMMATRIX & reflectionViewMatrix)
{
	// stores the reflection view matrix into the input parameter
	reflectionViewMatrix = reflectionViewMatrix_;
	return;
}

///////////////////////////////////////////////////////////

const XMMATRIX & CameraClass::GetReflectionViewMatrix()
{
	// returns the reflection view matrix to any calling functions
	return this->reflectionViewMatrix_;
}






////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC SETTERS
//
////////////////////////////////////////////////////////////////////////////////////////////


void CameraClass::SetProjectionValues(const float fovDegrees,
	const float aspectRatio,
	const float nearZ, 
	const float farZ)
{
	// set up the projection matrix

	//float fovRadians = static_cast<float>(D3DX_PI) / 4.0f;
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix_ = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
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

void CameraClass::UpdateMatrix()
{
	//
	// updates the view matrix and also updates the movement vectors
	//

	// load the current position values into a vector
	XMVECTOR posVector(XMLoadFloat3(&this->position_));

	// calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rotation_.x, this->rotation_.y, this->rotation_.z);

	//calculate unit vector of camera target based on camera forward value transformed by camera rotation matrix
	this->vecLookAt_ = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR_, camRotationMatrix);

	// translate the rotated camera position to the location of the viewer
	this->vecLookAt_ = XMVectorAdd(posVector, vecLookAt_);

	// calculate up direction based on the current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR_, camRotationMatrix);

	// rebuild view matrix
	this->viewMatrix_ = XMMatrixLookAtLH(posVector, vecLookAt_, upDir);
	
	return;

} // end UpdateViewMatrix