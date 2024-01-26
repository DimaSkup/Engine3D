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
#include "../Model/GameObject.h"

using namespace DirectX;


//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass : public GameObject
{
public:
	CameraClass(const float cameraSpeed, const float cameraSensitivity);
	~CameraClass();

	void SetProjectionValues(const float fovDegrees, const float aspectRatio, const float nearZ, const float farZ);


	// getters
	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	XMFLOAT3 GetRotationFloat3InDegrees();


	// functions for handling planar reflections
	void UpdateReflectionViewMatrix(const DirectX::XMFLOAT3 & reflectionPlanePos, const DirectX::XMFLOAT3 & relfectionPlaneRotation);
	void GetReflectionViewMatrix(XMMATRIX & reflectionViewMatrix);
	const XMMATRIX & CameraClass::GetReflectionViewMatrix();

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

protected:
	virtual void UpdateMatrix() override;


protected:
	//XMFLOAT3 lookAtPoint_;     // the camera's look at point
	//XMFLOAT3 up_;              // the camera's up direction
	XMMATRIX viewMatrix_;        // the current view matrix
	XMMATRIX projectionMatrix_;  // the current projection matrix
	XMMATRIX reflectionViewMatrix_;

	//XMVECTOR posVector_;         // the camera position (VECTOR)
	//XMVECTOR rotVector_;         // the camera rotation (VECTOR)
	XMVECTOR vecLookAt_;

	float pitch_ = 0.0f;         // the current value of camera's pitch
	float yaw_ = 0.0f;           // the current value of camera's yaw
	float roll_ = 0.0f;	         // the current value of camera's roll

	const float movingSpeed_ = 0.02f;              // a camera movement speed
	const float rotationSpeed_ = 0.01f;            // a camera turning speed
};