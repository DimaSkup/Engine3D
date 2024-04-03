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
private:
	struct DEFAULT_VECTORS
	{
		const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR_ = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_UP_VECTOR_ = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR_ = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR_ = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR_ = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	};

public:
	CameraClass();
	~CameraClass();

	void Initialize(const float cameraSpeed, const float cameraSensitivity);

	void SetProjectionValues(const float fovDegrees, const float aspectRatio, const float nearZ, const float farZ);


	// getters
	inline const float GetCameraDepth() const
	{
		// return the value how far the camera can seen
		return cameraDepth_;
	}

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	const XMVECTOR & GetPosition() const;
	const XMVECTOR & GetRotation() const;
	const XMVECTOR & GetLookAt() const;

	void GetPositionFloat3(_Inout_ DirectX::XMFLOAT3 & position);
	XMFLOAT3 GetRotationFloat3InDegrees();

	// get directions vectors
	const DirectX::XMVECTOR & GetForwardVector()  const;
	const DirectX::XMVECTOR & GetRightVector()    const;
	const DirectX::XMVECTOR & GetBackwardVector() const;
	const DirectX::XMVECTOR & GetLeftVector()     const;

	
	void SetPosition(const DirectX::XMVECTOR & newPosition);
	void SetRotationInRad(const DirectX::XMVECTOR & newAngle);
	void SetRotationInDeg(const DirectX::XMVECTOR & newAngle);

	void AdjustPosition(const DirectX::XMVECTOR & translationVector);
	void AdjustRotationInRad(const DirectX::XMVECTOR & angle);
	void AdjustRotationInDeg(const DirectX::XMVECTOR & angle);

	// functions for handling planar reflections
	void UpdateReflectionViewMatrix(const DirectX::XMFLOAT3 & reflectionPlanePos, const DirectX::XMFLOAT3 & relfectionPlaneRotation);
	void GetReflectionViewMatrix(XMMATRIX & reflectionViewMatrix);
	const XMMATRIX & CameraClass::GetReflectionViewMatrix();

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

	void UpdateViewMatrix();


protected:
	//XMFLOAT3 lookAtPoint_;     // the camera's look at point
	//XMFLOAT3 up_;              // the camera's up direction
	DirectX::XMMATRIX viewMatrix_;        // the current view matrix
	DirectX::XMMATRIX projectionMatrix_;  // the current projection matrix
	DirectX::XMMATRIX reflectionViewMatrix_;

	DirectX::XMVECTOR posVector_;         // the camera position (VECTOR)
	DirectX::XMVECTOR rotVector_;         // the camera rotation (VECTOR)
	DirectX::XMVECTOR vecLookAt_;

	DirectX::XMVECTOR vecForward_;      // the current forward direction of the game obj
	DirectX::XMVECTOR vecLeft_;
	DirectX::XMVECTOR vecRight_;
	DirectX::XMVECTOR vecBackward_;

	const DEFAULT_VECTORS defaultVectors_;

	
	//float pitch_ = 0.0f;         // the current value of camera's pitch
	//float yaw_ = 0.0f;           // the current value of camera's yaw
	//float roll_ = 0.0f;	         // the current value of camera's roll

	float movingSpeed_ = 0.02f;              // a camera movement speed
	float rotationSpeed_ = 0.01f;            // a camera turning speed
	float cameraDepth_ = 0.0f;
};