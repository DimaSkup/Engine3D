/////////////////////////////////////////////////////////////////////
// Filename:     cameraclass.h
// Description:  CameraClass tells the DirectX from where we are 
//               looking at the scene. Initializes the view matrix
//               which we use to render image
// Created:      07.04.22
// Revising:     20.12.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"        // log system

using namespace DirectX;


//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass 
{
public:
	CameraClass(const float cameraSpeed, const float cameraSensitivity);
	~CameraClass();

	void SetProjectionValues(const float fovDegrees, const float aspectRatio, const float nearZ, const float farZ);


	// getters
	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	const XMVECTOR & GetPositionVector() const;
	const XMFLOAT3 & GetPositionFloat3() const;
	const XMVECTOR & GetRotationVector() const;
	const XMFLOAT3 & GetRotationFloat3() const;
	XMFLOAT3 GetRotationFloat3InDegrees();
	

	// setters
	void SetPosition(const XMVECTOR & pos);
	void AdjustPosition(const XMVECTOR & pos);
	void SetPosition(const float x, const float y, const float z);
	void AdjustPosition(const float x, const float y, const float z);

	void SetRotation(const XMVECTOR & rot);
	void AdjustRotation(const XMVECTOR & rot);
	void SetRotation(const float x, const float y, const float z);
	void AdjustRotation(const float x, const float y, const float z);

	void SetLookAtPos(XMFLOAT3 lookAtPos);  // set a look at point position

	// moving directions
	const XMVECTOR & GetForwardVector();
	const XMVECTOR & GetRightVector();
	const XMVECTOR & GetBackwardVector();
	const XMVECTOR & GetLeftVector();

	// functions for handling planar reflections
	void UpdateReflectionViewMatrix(const DirectX::XMFLOAT3 & reflectionPlanePos, const DirectX::XMFLOAT3 & relfectionPlaneRotation);
	void GetReflectionViewMatrix(XMMATRIX & reflectionViewMatrix);
	const XMMATRIX & CameraClass::GetReflectionViewMatrix();

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

protected:
	void UpdateViewMatrix();


protected:
	//XMFLOAT3 lookAtPoint_;     // the camera's look at point
	//XMFLOAT3 up_;              // the camera's up direction
	XMMATRIX viewMatrix_;        // the current view matrix
	XMMATRIX projectionMatrix_;  // the current projection matrix
	XMMATRIX reflectionViewMatrix_;


	//void CalculateNewLookAtPoint();  // if we did some moving or rotation we need to recalculate the look at point
	XMVECTOR posVector_;         // the camera position (VECTOR)
	XMVECTOR rotVector_;         // the camera rotation (VECTOR)
	XMFLOAT3 pos_;               // the camera position 
	XMFLOAT3 rot_;               // the camera rotation

	XMVECTOR vecLookAt_;
	XMVECTOR vecForward_;
	XMVECTOR vecLeft_;
	XMVECTOR vecRight_;
	XMVECTOR vecBackward_;

	const XMVECTOR DEFAULT_FORWARD_VECTOR_  = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR_       = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR_ = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR_     = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR_    = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	float pitch_ = 0.0f;         // the current value of camera's pitch
	float yaw_ = 0.0f;           // the current value of camera's yaw
	float roll_ = 0.0f;	         // the current value of camera's roll

	const float movingSpeed_ = 0.02f;              // a camera movement speed
	const float rotationSpeed_ = 0.01f;            // a camera turning speed
};