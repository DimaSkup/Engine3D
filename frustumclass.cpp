////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
// Revising: 13.08.22
////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

FrustumClass::FrustumClass(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

// we don't use the copy constructor and destructor in this class
FrustumClass::FrustumClass(const FrustumClass& copy) {}
FrustumClass::~FrustumClass(void) {}


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
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	float zMinimum = 0.0f;
	float r = 0.0f;
	DirectX::XMMATRIX matrix;  // the matrix of the view frustum
	DirectX::XMFLOAT4X4 fMatrix; // we need it to get access to the values of the projection matrix

	// calculate the minimum Z distance in the frustum
	DirectX::XMStoreFloat4x4(&fMatrix, projectionMatrix);
	zMinimum = -(fMatrix._43) / fMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix.r[2] = DirectX::XMVectorSetZ(projectionMatrix.r[2], r);
	projectionMatrix.r[3] = DirectX::XMVectorSetZ(projectionMatrix.r[3], -r * zMinimum);

	// create the frustum matrix from the view matrix and updated projection matrix
	matrix = DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);

	// -------------------- calculate planes of frustum ----------------------------- //
	DirectX::XMStoreFloat4x4(&fMatrix, matrix); // to get access to particular values of the matrix

	// calculate NEAR plane of frustum
	DirectX::XMVectorSetX(m_planes[0], fMatrix._14 + fMatrix._13);
	DirectX::XMVectorSetY(m_planes[0], fMatrix._24 + fMatrix._23);
	DirectX::XMVectorSetZ(m_planes[0], fMatrix._34 + fMatrix._33);
	DirectX::XMVectorSetW(m_planes[0], fMatrix._44 + fMatrix._43);
	m_planes[0] = DirectX::XMPlaneNormalize(m_planes[0]);

	// calculate FAR plane of frustum
	DirectX::XMVectorSetX(m_planes[1], fMatrix._14 - fMatrix._13);
	DirectX::XMVectorSetY(m_planes[1], fMatrix._24 - fMatrix._23);
	DirectX::XMVectorSetZ(m_planes[1], fMatrix._34 - fMatrix._33);
	DirectX::XMVectorSetW(m_planes[1], fMatrix._44 - fMatrix._43);
	m_planes[1] = DirectX::XMPlaneNormalize(m_planes[1]);

	// calculate LEFT plane of frustum
	DirectX::XMVectorSetX(m_planes[2], fMatrix._14 + fMatrix._11);
	DirectX::XMVectorSetY(m_planes[2], fMatrix._24 + fMatrix._21);
	DirectX::XMVectorSetZ(m_planes[2], fMatrix._34 + fMatrix._31);
	DirectX::XMVectorSetW(m_planes[2], fMatrix._44 + fMatrix._41);
	m_planes[2] = DirectX::XMPlaneNormalize(m_planes[2]);

	// calculate RIGHT plane of frustum
	DirectX::XMVectorSetX(m_planes[3], fMatrix._14 - fMatrix._11);
	DirectX::XMVectorSetY(m_planes[3], fMatrix._24 - fMatrix._21);
	DirectX::XMVectorSetZ(m_planes[3], fMatrix._34 - fMatrix._31);
	DirectX::XMVectorSetW(m_planes[3], fMatrix._44 - fMatrix._41);
	m_planes[3] = DirectX::XMPlaneNormalize(m_planes[3]);

	// calculate TOP plane of frustum
	DirectX::XMVectorSetX(m_planes[4], fMatrix._14 - fMatrix._12);
	DirectX::XMVectorSetY(m_planes[4], fMatrix._24 - fMatrix._22);
	DirectX::XMVectorSetZ(m_planes[4], fMatrix._34 - fMatrix._32);
	DirectX::XMVectorSetW(m_planes[4], fMatrix._44 - fMatrix._42);
	m_planes[4] = DirectX::XMPlaneNormalize(m_planes[4]);

	// calculate BOTTOM plane of frustum
	DirectX::XMVectorSetX(m_planes[5], fMatrix._14 + fMatrix._12);
	DirectX::XMVectorSetY(m_planes[5], fMatrix._24 + fMatrix._22);
	DirectX::XMVectorSetZ(m_planes[5], fMatrix._34 + fMatrix._32);
	DirectX::XMVectorSetW(m_planes[5], fMatrix._44 + fMatrix._42);
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