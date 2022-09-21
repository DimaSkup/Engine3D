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

#include "macros.h"
#include "log.h"        // log system


//////////////////////////////////
// Class name: CameraClass
//////////////////////////////////
class CameraClass 
{
public:
	CameraClass(void);
	CameraClass(const CameraClass&);
	~CameraClass(void);

	void SetViewParameters(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 lookAtPoint, DirectX::XMFLOAT3 up);
	//void CalcViewMatrixByRotationAngles(float xAxisAngle, float yAxisAngle, float zAxisAngle);

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation); // set a look at point
	void SetOrientation(DirectX::XMFLOAT3 orientation); // set camera rotation angles (takes in degrees)
	void SetDirectionUp(DirectX::XMFLOAT3 up);

	DirectX::XMFLOAT3 GetPosition(void);
	DirectX::XMFLOAT3 GetRotation(void);      // get a look at point coordinates
	DirectX::XMFLOAT3 GetOrientation(void);   // get camera rotation angles (in radians)
	DirectX::XMFLOAT3 GetDirectionUp(void);

	void Render(void);	// calculate camera position and generate the view matrix
	void GetViewMatrix(DirectX::XMMATRIX&);

	// memory allocation
	void* operator new(size_t i)
	{
		void* ptr = _aligned_malloc(i, 16);
		if (!ptr)
		{
			Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
			return nullptr;
		}

		return ptr;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_orientation; // the camera orientation angles (in radians)
	DirectX::XMFLOAT3 m_up;
	
	//float xAxisAngle, yAxisAngle, zAxisAngle;
	//float m_yaw, m_pitch, m_roll;
	
	DirectX::XMMATRIX m_viewMatrix;
};