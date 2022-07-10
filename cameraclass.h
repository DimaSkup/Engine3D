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
#include <d3dx10math.h>
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

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	DirectX::XMFLOAT3 GetPosition(void);
	DirectX::XMFLOAT3 GetRotation(void);

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
	float m_posX, m_posY, m_posZ;
	float m_rotX, m_rotY, m_rotZ;
	DirectX::XMMATRIX m_viewMatrix;
};