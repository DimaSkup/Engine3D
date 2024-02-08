/////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
// Created:  04.04.22
// Revising: 20.12.22
/////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass(const float cameraSpeed, const float cameraSensitivity)
	: posVector_{0, 0, 0},
	  rotVector_{0, 0, 0},
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

const XMVECTOR & CameraClass::GetPosition() const
{
	return posVector_;
}

const XMVECTOR & CameraClass::GetRotation() const
{
	return rotVector_;
}


/////////////////////////////////////////////////

XMFLOAT3 CameraClass::GetPositionFloat3()
{
	XMFLOAT3 float3;
	XMStoreFloat3(&float3, posVector_);
	return float3;
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


void CameraClass::SetPosition(const DirectX::XMVECTOR & newPosition)
{
	posVector_ = newPosition;
}

void CameraClass::SetRotationInRad(const DirectX::XMVECTOR & newAngle)
{
	rotVector_ = newAngle;
}

void CameraClass::SetRotationInDeg(const DirectX::XMVECTOR & newAngle)
{
	const float ax = DirectX::XMConvertToRadians(XMVectorGetX(newAngle));
	const float ay = DirectX::XMConvertToRadians(XMVectorGetY(newAngle));
	const float az = DirectX::XMConvertToRadians(XMVectorGetZ(newAngle));

	rotVector_ = { ax, ay, az };
}

void CameraClass::AdjustPosition(const DirectX::XMVECTOR & translationVector)
{
	posVector_ += translationVector;
}

void CameraClass::AdjustRotationInRad(const DirectX::XMVECTOR & angle)
{
	rotVector_ += angle;
}

void CameraClass::AdjustRotationInDeg(const DirectX::XMVECTOR & angle)
{
	const float ax = DirectX::XMConvertToRadians(XMVectorGetX(angle));
	const float ay = DirectX::XMConvertToRadians(XMVectorGetY(angle));
	const float az = DirectX::XMConvertToRadians(XMVectorGetZ(angle));

	rotVector_ += { ax, ay, az };
}



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


// 
// GETTERS for directions vectors
// 
const DirectX::XMVECTOR & CameraClass::GetForwardVector()  const { return this->vecForward_; }
const DirectX::XMVECTOR & CameraClass::GetRightVector()    const { return this->vecRight_; }
const DirectX::XMVECTOR & CameraClass::GetBackwardVector() const { return this->vecBackward_; }
const DirectX::XMVECTOR & CameraClass::GetLeftVector()     const { return this->vecLeft_; }



// memory allocation (we need it because we use DirectX::XM-objects)
void* CameraClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error(LOG_MACRO, "can't allocate the memory for object");
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
	const XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYawFromVector(rotVector_);
	//XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rotation_.x, this->rotation_.y, this->rotation_.z);

	//calculate unit vector of camera target based on camera forward value transformed by camera rotation matrix
	XMVECTOR newVecLookAt = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR_, camRotationMatrix);

	// translate the rotated camera position to the location of the viewer
	newVecLookAt = XMVectorAdd(posVector_, newVecLookAt);

	// calculate up direction based on the current rotation
	const XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR_, camRotationMatrix);

	// each time when we modify rotation of the camera we have to update
	// its basic direction vectors
	const DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, XMVectorGetY(rotVector_), 0.0f);
	this->vecForward_ = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR_, vecRotationMatrix);
	this->vecBackward_ = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR_, vecRotationMatrix);
	this->vecLeft_ = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR_, vecRotationMatrix);
	this->vecRight_ = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR_, vecRotationMatrix);


	// update some data of this camera	
	this->viewMatrix_ = XMMatrixLookAtLH(posVector_, newVecLookAt, upDir); // rebuild view matrix
	this->vecLookAt_ = newVecLookAt;                                       // update the look at position
	
	return;

} // end UpdateViewMatrix