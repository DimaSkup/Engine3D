// ********************************************************************************
// Filename: cameraclass.cpp
// 
// Created:  04.04.22
// ********************************************************************************
#include "cameraclass.h"
#include "../Engine/Log.h"    
#include "../Common/Assert.h"

using namespace DirectX;


CameraClass::CameraClass()
	: posVector_{0, 0, 0},
	  rotVector_{0, 0, 0}
	 
{
	// create a view matrix for default params of the camera
	UpdateViewMatrix();
}

CameraClass::~CameraClass()
{
}

void CameraClass::Initialize(const CameraInitParams& params)
{
	movingSpeed_ = params.speed_;           // a camera movement speed
	rotationSpeed_ = params.sensitivity_;   // a camera rotation speed

	SetProjectionValues(
		params.fovDegrees_, 
		params.aspectRatio_,
		params.nearZ_, 
		params.farZ_);
}


// ********************************************************************************
// 
//                              PUBLIC GETTTERS
// 
// ********************************************************************************


void CameraClass::GetPositionFloat3(_Inout_ DirectX::XMFLOAT3 & position)
{
	// store a position vector as XMFLOAT3 into the input parameter
	XMStoreFloat3(&position, posVector_);
}

XMFLOAT3 CameraClass::GetRotationFloat3InDegrees()
{
	// get rotation float3 (in degrees)
	return {
		DirectX::XMConvertToDegrees(rotVector_.m128_f32[0]),  // rotation around X-axis
		DirectX::XMConvertToDegrees(rotVector_.m128_f32[1]),  // rotation around Y-axis
		DirectX::XMConvertToDegrees(rotVector_.m128_f32[2])   // rotation around Z-axis
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
	float yaw = -XM_PI;                    // around y-axis
	float roll  = rotVector_.m128_f32[3];  // around z-axis
#endif
	
	// create the rotation matrix from the yaw, pitch, and roll values
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// transform the lookAtVec and up vector by the rotation matrix so the view is correcly
	// rotated at the origin
	const DEFAULT_VECTORS defaultVectors = defaultVectors_;
	lookAtVec = XMVector3TransformCoord(defaultVectors.DEFAULT_FORWARD_VECTOR_, rotationMatrix);
	upVec = XMVector3TransformCoord(defaultVectors.DEFAULT_UP_VECTOR_, rotationMatrix);

	// translate the rotated camera position to the location of the viewer
	lookAtVec = XMVectorAdd(positionVec, lookAtVec);

	// finally create the view matrix from the three updated vectors
	reflectionViewMatrix_ = XMMatrixLookAtLH(positionVec, lookAtVec, upVec);
}




// ********************************************************************************
//
//                               PUBLIC SETTERS
//
// ********************************************************************************

void CameraClass::SetRotationInDeg(const DirectX::XMVECTOR & newAngle)
{
	// takes as input angles in degrees

	const float ax = DirectX::XMConvertToRadians(XMVectorGetX(newAngle));
	const float ay = DirectX::XMConvertToRadians(XMVectorGetY(newAngle));
	const float az = DirectX::XMConvertToRadians(XMVectorGetZ(newAngle));

	rotVector_ = { ax, ay, az };
}

///////////////////////////////////////////////////////////

void CameraClass::AdjustRotationInDeg(const DirectX::XMVECTOR & angle)
{
	// takes as input angles in degrees

	const float ax = DirectX::XMConvertToRadians(XMVectorGetX(angle));
	const float ay = DirectX::XMConvertToRadians(XMVectorGetY(angle));
	const float az = DirectX::XMConvertToRadians(XMVectorGetZ(angle));

	rotVector_ += { ax, ay, az };
}

///////////////////////////////////////////////////////////

void CameraClass::SetProjectionValues(
	const float fov,                     // field of view (in radians)
	const float aspectRatio,
	const float nearZ, 
	const float farZ)
{
	// set up the projection matrix

	assert(fov > 0.0f);
	assert(aspectRatio > 0.0f);
	assert(nearZ > 0.0f);
	assert(farZ > nearZ);

	cameraDepth_ = farZ;

	projectionMatrix_ = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
}

/////////////////////////////////////////////////

void CameraClass::UpdateViewMatrix()
{
	//
	// updates the view matrix and also updates the movement vectors
	//

	const DirectX::XMVECTOR vecPosition(posVector_);
	const DirectX::XMVECTOR vecRotation(rotVector_);
	const DEFAULT_VECTORS defaultVectors = defaultVectors_;

	// calculate camera rotation matrix
	const XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYawFromVector(vecRotation);

	//calculate unit vector of camera target based on camera forward value transformed by camera rotation matrix
	XMVECTOR newVecLookAt = XMVector3TransformCoord(defaultVectors.DEFAULT_FORWARD_VECTOR_, camRotationMatrix);

	// translate the rotated camera position to the location of the viewer
	newVecLookAt = XMVectorAdd(vecPosition, newVecLookAt);

	// calculate up direction based on the current rotation
	//const XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR_, camRotationMatrix);

	// update some data of this camera	
	viewMatrix_ = XMMatrixLookAtLH(vecPosition, newVecLookAt, defaultVectors.DEFAULT_UP_VECTOR_);
	vecLookAt_ = newVecLookAt;

	// each time when we modify rotation of the camera we have to update
	// its basic direction vectors
	const DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, XMVectorGetY(vecRotation), 0.0f);
	vecForward_ = XMVector3TransformCoord(defaultVectors.DEFAULT_FORWARD_VECTOR_, vecRotationMatrix);
	vecBackward_ = XMVector3TransformCoord(defaultVectors.DEFAULT_BACKWARD_VECTOR_, vecRotationMatrix);
	vecLeft_ = XMVector3TransformCoord(defaultVectors.DEFAULT_LEFT_VECTOR_, vecRotationMatrix);
	vecRight_ = XMVector3TransformCoord(defaultVectors.DEFAULT_RIGHT_VECTOR_, vecRotationMatrix);
}

/////////////////////////////////////////////////

void* CameraClass::operator new(size_t i)
{
	// memory allocation (we need it because we use DirectX::XM-objects)
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error("can't allocate the memory for object");
	throw std::bad_alloc{};
}

void CameraClass::operator delete(void* p)
{
	_aligned_free(p);
}

