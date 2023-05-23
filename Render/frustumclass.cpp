////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
// Revising: 13.08.22
////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

FrustumClass::FrustumClass(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
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


void FrustumClass::Initialize(float screenDepth)
{
	screenDepth_ = screenDepth;
	return;
}

// is called each frame by the GraphicsClass. Calculates the matrix of the view frustum
// at that frame and then calculates the six planes_ that form the view frustum
void FrustumClass::ConstructFrustum(DirectX::XMMATRIX projectionMatrix, 
	                                DirectX::XMMATRIX viewMatrix)
{
	float zMinimum = 0.0f;
	float r = 0.0f;
	DirectX::XMMATRIX finalMatrix;
	DirectX::XMFLOAT4X4 fProjMatrix; // we need it to get access to the values of the projection matrix


	
	// convert the projection matrix into a 4x4 float type
	DirectX::XMStoreFloat4x4(&fProjMatrix, projectionMatrix);

	// calculate the minimum Z distance in the frustum
	zMinimum = -(fProjMatrix._43) / fProjMatrix._33;
	r = screenDepth_ / (screenDepth_ - zMinimum);

	// load the updated values back into the projection matrix
	fProjMatrix._33 = r;
	fProjMatrix._43 = -r * zMinimum;
	projectionMatrix = DirectX::XMLoadFloat4x4(&fProjMatrix);

	// create the frustum matrix from the view matrix and updated projection matrix
	finalMatrix = DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);

	// convert the final matrix into a 4x4 float type
	DirectX::XMStoreFloat4x4(&fProjMatrix, finalMatrix); 

	
	// calculate NEAR plane of frustum
	planes_[0].m128_f32[0] = fProjMatrix._14 + fProjMatrix._13;
	planes_[0].m128_f32[1] = fProjMatrix._24 + fProjMatrix._23;
	planes_[0].m128_f32[2] = fProjMatrix._34 + fProjMatrix._33;
	planes_[0].m128_f32[3] = fProjMatrix._44 + fProjMatrix._43;
	planes_[0] = DirectX::XMPlaneNormalize(planes_[0]);


	// calculate FAR plane of frustum
	planes_[1].m128_f32[0] = fProjMatrix._14 - fProjMatrix._13;
	planes_[1].m128_f32[1] = fProjMatrix._24 - fProjMatrix._23;
	planes_[1].m128_f32[2] = fProjMatrix._34 - fProjMatrix._33;
	planes_[1].m128_f32[3] = fProjMatrix._44 - fProjMatrix._43;
	planes_[1] = DirectX::XMPlaneNormalize(planes_[1]);

	// calculate LEFT plane of frustum
	planes_[2].m128_f32[0] = fProjMatrix._14 + fProjMatrix._11;
	planes_[2].m128_f32[1] = fProjMatrix._24 + fProjMatrix._21;
	planes_[2].m128_f32[2] = fProjMatrix._34 + fProjMatrix._31;
	planes_[2].m128_f32[3] = fProjMatrix._44 + fProjMatrix._41;
	planes_[2] = DirectX::XMPlaneNormalize(planes_[2]);

	// calculate RIGHT plane of frustum
	planes_[3].m128_f32[0] = fProjMatrix._14 - fProjMatrix._11;
	planes_[3].m128_f32[1] = fProjMatrix._24 - fProjMatrix._21;
	planes_[3].m128_f32[2] = fProjMatrix._34 - fProjMatrix._31;
	planes_[3].m128_f32[3] = fProjMatrix._44 - fProjMatrix._41;
	planes_[3] = DirectX::XMPlaneNormalize(planes_[3]);

	// calculate TOP plane of frustum
	planes_[4].m128_f32[0] = fProjMatrix._14 - fProjMatrix._12;
	planes_[4].m128_f32[1] = fProjMatrix._24 - fProjMatrix._22;
	planes_[4].m128_f32[2] = fProjMatrix._34 - fProjMatrix._32;
	planes_[4].m128_f32[3] = fProjMatrix._44 - fProjMatrix._42;
	planes_[4] = DirectX::XMPlaneNormalize(planes_[4]);

	// calculate BOTTOM plane of frustum
	planes_[5].m128_f32[0] = fProjMatrix._14 + fProjMatrix._12;
	planes_[5].m128_f32[1] = fProjMatrix._24 + fProjMatrix._22;
	planes_[5].m128_f32[2] = fProjMatrix._34 + fProjMatrix._32;
	planes_[5].m128_f32[3] = fProjMatrix._44 + fProjMatrix._42;
	planes_[5] = DirectX::XMPlaneNormalize(planes_[5]);

	return;
}  // ConstructFrustum


// checks if a single point is inside the viewing frustum
bool FrustumClass::CheckPoint(float x, float y, float z)
{
	// check if the point is inside all six planes_ of the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		DirectX::XMVECTOR vectorOfPoint{ x, y, z };
		DirectX::XMVECTOR dotProductVector = DirectX::XMPlaneDotCoord(planes_[i], vectorOfPoint);
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
// It then checks if any on of the corner points are inside all 6 planes_ of 
// the viewing frustum. If it does find a point inside all six planes_ of the viewing
// frustum it returns true, otherwise it returns false.
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	float dotProduct = 0.0f;   // here we put the dot product results

	// check if any one point of the cube is in the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		// --- FAR SIDE OF THE CUBE --- //

		// far left bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - radius, yCenter - radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }
	
		//  far right bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + radius, yCenter - radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }


		// far left upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - radius, yCenter + radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }

		// far right upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + radius, yCenter + radius, zCenter - radius);
		if (dotProduct >= 0.0f) { continue; }




		// --- NEAR SIDE OF THE CUBE --- //

		// near left bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - radius, yCenter - radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }

		// near right bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + radius, yCenter - radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }


		// near left upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - radius, yCenter + radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }

		// near right upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + radius, yCenter + radius, zCenter + radius);
		if (dotProduct >= 0.0f) { continue; }

		// this point is outside of the viewing frustum
		return false;  
	}

	return true;   // this cube is in the viewing frustum
} // CheckCube() 



// CheckSphere() checks if the radius of the sphere from the centre point is inside
// all six planes_ of the viewing frustum. If it is outside any of them then the sphere
// cannot be seen and the function will return false. If it is inside all six the function
// returns true that the sphere can be seen.
bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	float dotProduct = 0.0f;   // here we put the dot product results

	// check if the radius of the sphere is inside the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		dotProduct = this->planeDotCoord(planes_[i], xCenter, yCenter, zCenter);

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

	// check if any of the 6 planes_ of the rectangle are inside the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		// --- FAR SIDE OF THE RECTANGLE --- //

		// far left bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - xSize, yCenter - ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }

		// far right bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + xSize, yCenter - ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }


		// far left upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - xSize, yCenter + ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }

		// far right upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + xSize, yCenter + ySize, zCenter - zSize);
		if (dotProduct >= 0.0f) { continue; }


		// --- NEAR SIDE OF THE RECTANGLE --- //

		// near left bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - xSize, yCenter - ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// near right bottom point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + xSize, yCenter - ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// near left upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter - xSize, yCenter + ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// near right upper point
		dotProduct = this->planeDotCoord(planes_[i], xCenter + xSize, yCenter + ySize, zCenter + zSize);
		if (dotProduct >= 0.0f) { continue; }

		// this point is outside of the viewing frustum
		return false;
	}

	return true;
} // CheckRectangle()



// CheckRectangle2 function works the same as the CheckRectangle function but it 
// uses the maximum and minimum dimensions instead of a center point and widths.
// It performs a dot product of the six viewing frustum planes_ and the six sides of the
// rectangle. If it determines any part of rectangle is in the viewing frustum then
// it returns true. If it goes through all six planes_ of the rectangle and doesn't find 
// any instead of viewing frustum then it returns false
bool FrustumClass::CheckRectangle2(float maxWidth, float maxHeight, float maxDepth,
	                               float minWidth, float minHeight, float minDepth)
{
	float dotProduct = 0.0f;

	// check if any of the 6 planes_ of the rectangle are inside the view frustum
	for (UINT i = 0; i < 6; i++)
	{
		dotProduct = this->planeDotCoord(planes_[i], minWidth, minHeight, minDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], maxWidth, minHeight, minDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], minWidth, maxHeight, minDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], maxWidth, maxHeight, minDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], minWidth, minHeight, maxDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], maxWidth, minHeight, maxDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], minWidth, maxHeight, maxDepth);
		if (dotProduct >= 0.0f) { continue; }

		dotProduct = this->planeDotCoord(planes_[i], maxWidth, maxHeight, maxDepth);
		if (dotProduct >= 0.0f) { continue; }


		// this point is outside of the viewing frustum
		return false;
	}

	return true;
}


// ----- memory allocation ----- //
void* FrustumClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error(THIS_FUNC, "can't allocate memory for the class object");
	throw std::bad_alloc{};
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
	DirectX::XMVECTOR vectorOfPoint { x, y, z, 1.0f }; // create a vector for the point
	DirectX::XMVECTOR dotProductVector = DirectX::XMPlaneDotCoord(plane, vectorOfPoint); // compute the dot product

	// return a dot product result
	return DirectX::XMVectorGetX(dotProductVector);
}