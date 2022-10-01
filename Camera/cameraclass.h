/////////////////////////////////////////////////////////////////////
// Filename:     cameraclass.h
// Description:  CameraClass tells the DirectX from where we are 
//               looking at the scene. Initializes the view matrix
//               which we use to render image
// Revising:     07.04.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../LogSystem/Log.h"        // log system


//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass 
{
public:
	CameraClass(void);
	CameraClass(const CameraClass&);
	~CameraClass(void);

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT2 rotation); // set camera rotation angles (takes in radians)
	void SetLookAtPoint(DirectX::XMFLOAT3 lookAtPoint); // set a look at point
	void SetDirectionUp(DirectX::XMFLOAT3 up);

	DirectX::XMFLOAT3 GetPosition(void);
	DirectX::XMFLOAT3 GetLookAtPoint(void);  // get a look at point coordinates
	DirectX::XMFLOAT2 GetRotation(void);     // get the camera rotation angles (in radians)
	DirectX::XMFLOAT3 GetDirectionUp(void);

	void Render(void);	// calculate camera position and generate the view matrix
	void GetViewMatrix(DirectX::XMMATRIX&);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	DirectX::XMFLOAT3 m_position;     // the camera position
	DirectX::XMFLOAT3 m_lookAtPoint;  // the camera's look at point
	DirectX::XMFLOAT3 m_up;           // the camera's up direction
	DirectX::XMMATRIX m_viewMatrix;   // the current view matrix

	// the camera orientation angles (in radians)
	float m_pitch;
	float m_yaw;
};