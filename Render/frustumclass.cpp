////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
// Revising: 13.08.22
////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

FrustumClass::FrustumClass(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

FrustumClass::FrustumClass(const FrustumClass& copy) 
{
}

FrustumClass::~FrustumClass(void) 
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// is called each frame by the GraphicsClass. Calculates the matrix of the view frustum
// at that frame and then calculates the six planes that form the view frustum
void FrustumClass::ConstructFrustum(float screenDepth,
	                                DirectX::XMMATRIX projectionMatrix, 
	                                DirectX::XMMATRIX viewMatrix)
{
	float zMinimum = 0.0f;
	float r = 0.0f;
	DirectX::XMMATRIX matrix;  // the matrix of the view frustum
	DirectX::XMMATRIX finalMatrix;
	DirectX::XMFLOAT4X4 fProjMatrix; // we need it to get access to the values of the projection matrix


	
	// convert the projection matrix into a 4x4 float type
	DirectX::XMStoreFloat4x4(&fProjMatrix, projectionMatrix);

	// calculate the minimum Z distance in the frustum
	zMinimum = -(fProjMatrix._43) / fProjMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);

	// load the updated values back into the projection matrix
	fProjMatrix._33 = r;
	fProjMatrix._43 = -r * zMinimum;
	projectionMatrix = DirectX::XMLoadFloat4x4(&fProjMatrix);

	// create the frustum matrix from the view matrix and updated projection matrix
	finalMatrix = DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);

	// convert the final matrix into a 4x4 float type
	DirectX::XMStoreFloat4x4(&fProjMatrix, finalMatrix); 

	
	// calculate NEAR plane of frustum
	m_planes[0].m128_f32[0] = fProjMatrix._14 + fProjMatrix._13;
	m_planes[0].m128_f32[1] = fProjMatrix._24 + fProjMatrix._23;
	m_planes[0].m128_f32[2] = fProjMatrix._34 + fProjMatrix._33;
	m_planes[0].m128_f32[3] = fProjMatrix._44 + fProjMatrix._43;
	m_planes[0] = DirectX::XMPlaneNormalize(m_planes[0]);


	// calculate FAR plane of frustum
	m_planes[1].m128_f32[0] = fProjMatrix._14 - fProjMatrix._13;
	m_planes[1].m128_f32[1] = fProjMatrix._24 - fProjMatrix._23;
	m_planes[1].m128_f32[2] = fProjMatrix._34 - fProjMatrix._33;
	m_planes[1].m128_f32[3] = fProjMatrix._44 - fProjMatrix._43;
	m_planes[1] = DirectX::XMPlaneNormalize(m_planes[1]);

	// calculate LEFT plane of frustum
	m_planes[2].m128_f32[0] = fProjMatrix._14 + fProjMatrix._11;
	m_planes[2].m128_f32[1] = fProjMatrix._24 + fProjMatrix._21;
	m_planes[2].m128_f32[2] = fProjMatrix._34 + fProjMatrix._31;
	m_planes[2].m128_f32[3] = fProjMatrix._44 + fProjMatrix._41;
	m_planes[2] = DirectX::XMPlaneNormalize(m_planes[2]);

	// calculate RIGHT plane of frustum
	m_planes[3].m128_f32[0] = fProjMatrix._14 - fProjMatrix._11;
	m_planes[3].m128_f32[1] = fProjMatrix._24 - fProjMatrix._21;
	m_planes[3].m128_f32[2] = fProjMatrix._34 - fProjMatrix._31;
	m_planes[3].m128_f32[3] = fProjMatrix._44 - fProjMatrix._41;
	m_planes[3] = DirectX::XMPlaneNormalize(m_planes[3]);

	// calculate TOP plane of frustum
	m_planes[4].m128_f32[0] = fProjMatrix._14 - fProjMatrix._12;
	m_planes[4].m128_f32[1] = fProjMatrix._24 - fProjMatrix._22;
	m_planes[4].m128_f32[2] = fProjMatrix._34 - fProjMatrix._32;
	m_planes[4].m128_f32[3] = fProjMatrix._44 - fProjMatrix._42;
	m_planes[4] = DirectX::XMPlaneNormalize(m_planes[4]);

	// calculate BOTTOM plane of frustum
	m_planes[5].m128_f32[0] = fProjMatrix._14 + fProjMatrix._12;
	m_planes[5].m128_f32[1] = fProjMatrix._24 + fProjMatrix._22;
	m_planes[5].m128_f32[2] = fProjMatrix._34 + fProjMatrix._32;
	m_planes[5].m128_f32[3] = fProjMatrix._44 + fProjMatrix._42;
	m_planes[5] = DirectX::XMPlaneNormalize(m_planes[5]);

	return;
}  // ConstructFrustum


// checks if a single point is inside the viewing frustum
bool FrustumClass::CheckPoint(float x, float y, float z)
{
	// check if the point is inside all six planes of the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		DirectX::XMVECTOR vectorOfPoint{ x, y, z };
		DirectX::XMVECTOR dotProductVector = DirectX::XMPlaneDotCoord(m_planes[i], vectorOfPoint);
		float dotProduct = DirectX::XMVectorGetX(dotProductVector);

		if (dotProduct < 0.0f)
		{
			return false;
		}
	}

	return true;
}



// this function checks if any of the eight corner points of the cube are inside 
// the viewing frustum. It only requires as input the centre point of the cube 
// and the radius, it uses those to calculate the 8 corner points of the cube.
// It then checks if any on of the corner points are inside all 6 planes of 
// the viewing frustum. If it does find a point inside all six planes of the viewing
// frustum it returns true, otherwise it returns false.
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	float dotProduct = 0.0f;   // here we put the dot product results

	// check if any one point of the cube is in the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		// --- FAR SIDE OF THE CUBE --- //

		// far left bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - radius, yCenter - radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }
	
		//  far right bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + radius, yCenter - radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }


		// far left upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - radius, yCenter + radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }

		// far right upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + radius, yCenter + radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }




		// --- NEAR SIDE OF THE CUBE --- //

		// near left bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - radius, yCenter - radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }

		// near right bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + radius, yCenter - radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }


		// near left upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - radius, yCenter + radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }

		// near right upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + radius, yCenter + radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }

		// this point is outside of the viewing frustum
		return false;  
	}

	return true;   // this cube is in the viewing frustum
} // CheckCube() 



// CheckSphere() checks if the radius of the sphere from the centre point is inside
// all six planes of the viewing frustum. If it is outside any of them then the sphere
// cannot be seen and the function will return false. If it is inside all six the function
// returns true that the sphere can be seen.
bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	float dotProduct = 0.0f;   // here we put the dot product results

	// check if the radius of the sphere is inside the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		dotProduct = this->planeDotCoord(m_planes[i], xCenter, yCenter, zCenter);

		if (dotProduct < -radius)
		{
			return false; 
		}
	}

	return true;
} // CheckSphere()



// CheckRectangle() works the same as CheckCube() except that that is takes as input 
// the x radius, y radius, and z radius of the rectangle instead of just a single radius 
// of a cube. It can then calculate the 8 corner points of the rectangle and do the 
// frustum checks similar to the CheckCube() function.
bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter,
	                              float xSize, float ySize, float zSize)
{
	float dotProduct = 0.0f; // here we put the dot product result

	// check if any of the 6 planes of the rectangle are inside the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		// --- FAR SIDE OF THE RECTANGLE --- //

		// far left bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - xSize, yCenter - ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }

		// far right bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + xSize, yCenter - ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }


		// far left upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - xSize, yCenter + ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }

		// far right upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + xSize, yCenter + ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }


		// --- NEAR SIDE OF THE RECTANGLE --- //

		// near left bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - xSize, yCenter - ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// near right bottom point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + xSize, yCenter - ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// near left upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter - xSize, yCenter + ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// near right upper point
		dotProduct = this->planeDotCoord(m_planes[i], xCenter + xSize, yCenter + ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// this point is outside of the viewing frustum
		return false;
	}

	return true;
} // CheckRectangle()


// ----- memory allocation ----- //
void* FrustumClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);

	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the class object");
		return nullptr;
	}

	return ptr;
} // operator new

void FrustumClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //
// calculates a magnitude of dot product between some plane and some point (x, y, z)
float FrustumClass::planeDotCoord(const DirectX::XMVECTOR& plane, float x, float y, float z)
{
	DirectX::XMVECTOR vectorOfPoint { x, y, z }; // create a vector for the point
	DirectX::XMVECTOR dotProductVector = DirectX::XMPlaneDotCoord(plane, vectorOfPoint); // compute the dot product

	// return a dot product result
	return DirectX::XMVectorGetX(dotProductVector);
}