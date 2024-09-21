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

using u32 = uint32_t;


//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass
{
public:
	struct CameraInitParams
	{
		CameraInitParams(
			const u32 wndWidth,
			const u32 wndHeight,
			const float nearZ,
			const float farZ,
			const float fovDegrees,
			const float speed,
			const float sensitivity) 
			:
			wndWidth_(wndWidth),
			wndHeight_(wndHeight),
			nearZ_(nearZ),
			farZ_(farZ),
			fovDegrees_(fovDegrees),
			speed_(speed),
			sensitivity_(sensitivity),
			aspectRatio_((float)wndWidth / (float)wndHeight) {}

		u32   wndWidth_ = 800;
		u32   wndHeight_ = 600;
		float nearZ_ = 1;                       // near Z-coordinate of the frustum
		float farZ_  = 100;                     // far Z-coordinate of the frustum
		float fovDegrees_ = 90;                 // field of view
		float speed_ = 1;                       // camera movement speed
		float sensitivity_ = 1;                 // camera rotation speed
		float aspectRatio_ = 4.0f / 3.0f;
	};

private:

	struct DEFAULT_VECTORS
	{
		const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR_  = { 0, 0, 1, 0 };
		const DirectX::XMVECTOR DEFAULT_UP_VECTOR_       = { 0, 1, 0, 0 };
		const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR_ = { 0, 0, -1, 0 };
		const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR_     = { -1, 0, 0, 0 };
		const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR_    = { 1, 0, 0, 0 };
	};

public:
	CameraClass();
	~CameraClass();

	void Initialize(const CameraInitParams& params);

	void UpdateViewMatrix();

	void SetProjectionValues(
		const float fov,        // field of view (in radians)
		const float aspectRatio,
		const float nearZ, 
		const float farZ);

	void GetPositionFloat3(DirectX::XMFLOAT3& position);
	DirectX::XMFLOAT3 GetRotationFloat3InDegrees();

	void SetRotationInDeg(const DirectX::XMVECTOR& newAngle);
	void AdjustRotationInDeg(const DirectX::XMVECTOR& angle);

	// functions for handling planar reflections
	void UpdateReflectionViewMatrix(
		const DirectX::XMFLOAT3& reflectionPlanePos, 
		const DirectX::XMFLOAT3& relfectionPlaneRotation);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


public:  // INLINE SETTERS METHODS

	inline void SetPosition        (const DirectX::XMVECTOR& newPosition) { posVector_ = newPosition; }
	inline void SetRotationInRad   (const DirectX::XMVECTOR& newAngle)    { rotVector_ = newAngle; }
	inline void AdjustPosition     (const DirectX::XMVECTOR& translation) { posVector_ = DirectX::XMVectorAdd(posVector_, translation); }
	inline void AdjustRotationInRad(const DirectX::XMVECTOR& angle)       { rotVector_ = DirectX::XMVectorAdd(rotVector_, angle); }

public:  // INLINE GETTERS METHODS

	inline const DirectX::XMMATRIX& GetViewMatrix()       const { return viewMatrix_; }
	inline const DirectX::XMMATRIX& GetProjectionMatrix() const { return projectionMatrix_; }

	inline const DirectX::XMVECTOR& GetPosition()         const { return posVector_; }
	inline const DirectX::XMVECTOR& GetRotation()         const { return rotVector_; }
	inline const DirectX::XMVECTOR& GetLookAt()           const { return vecLookAt_; }
	inline const float GetCameraDepth()                   const { return cameraDepth_; }

	// get directions vectors
	inline const DirectX::XMVECTOR& GetForwardVector()    const { return vecForward_; }
	inline const DirectX::XMVECTOR& GetRightVector()      const { return vecRight_; }
	inline const DirectX::XMVECTOR& GetBackwardVector()   const { return vecBackward_; }
	inline const DirectX::XMVECTOR& GetLeftVector()       const { return vecLeft_; }

	inline void GetReflectionViewMatrix(DirectX::XMMATRIX& reflectionViewMatrix) { reflectionViewMatrix = reflectionViewMatrix_; }
	inline const DirectX::XMMATRIX& GetReflectionViewMatrix() const { return reflectionViewMatrix_; }


protected:
	//DirectX::XMFLOAT3 lookAtPoint_;       // the camera's look at point
	//DirectX::XMFLOAT3 up_;                // the camera's up direction
	DirectX::XMMATRIX viewMatrix_;          // the current view matrix
	DirectX::XMMATRIX projectionMatrix_;    // the current projection matrix
	DirectX::XMMATRIX reflectionViewMatrix_;

	DirectX::XMVECTOR posVector_;           // the camera position (VECTOR)
	DirectX::XMVECTOR rotVector_;           // the camera rotation (VECTOR)
	DirectX::XMVECTOR vecLookAt_;

	DirectX::XMVECTOR vecForward_;          // the current forward direction of the game obj
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