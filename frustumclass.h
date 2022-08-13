////////////////////////////////////////////////////////////////////
// Filename:     frustumclass.h
// Description:  this class incapsulates calculations for the six 
//               planes of the view frustum; and functions for 
//               checking if either a point, cube, spehere, or rectangle
//               are inside the viewing frustum or not.
//
// Revising:     13.08.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3dx10math.h>
#include <DirectXMath.h>

//////////////////////////////////
// Class name: FrustumClass
//////////////////////////////////
class FrustumClass
{
public:
	FrustumClass(void);
	FrustumClass(const FrustumClass& copy);
	~FrustumClass(void);


	void ConstructFrustum(float screenDepth, DirectX::XMMATRIX projectionMatrix, DirectX::XMMATRIX viewMatrix);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

	// memory allocation (is necessary because of the XM-structures in this class)
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:
	DirectX::XMFLOAT4 m_planes[6];
};
