////////////////////////////////////////////////////////////////////
// Filename:     frustumclass.h
// Description:  this class incapsulates calculations for the six 
//               planes_ of the view frustum; and functions for 
//               checking if either a point, cube, spehere, or rectangle
//               are inside the viewing frustum or not.
//
// Revising:     13.08.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>

#include "../Engine/Log.h"

//////////////////////////////////
// Class name: FrustumClass
//////////////////////////////////
class FrustumClass
{
public:
	FrustumClass(void);
	~FrustumClass(void);

	void Initialize(float screenDepth);
	void ConstructFrustum(DirectX::XMMATRIX projectionMatrix, DirectX::XMMATRIX viewMatrix);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(const DirectX::XMFLOAT3 & centerPos, const float radius = 1.0f);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);
	bool CheckRectangle2(float, float, float, float, float, float);

	// memory allocation (is necessary because of the XM-structures in this class)
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:  // restrict a copying of this class instance
	FrustumClass(const FrustumClass & obj);
	FrustumClass & operator=(const FrustumClass & obj);

private:
	float planeDotCoord(const DirectX::XMVECTOR& plane, float x, float y, float z);

private:
	float screenDepth_ = 0.0f;
	DirectX::XMVECTOR planes_[6];
};
