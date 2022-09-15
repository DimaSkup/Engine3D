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
// the viewing frustum. It only requires as input the center point of the cube 
// and the radius, it uses those to calculate the 8 corner points of the cube.
// It then checks if any on of the corner points are inside all 6 planes of 
// the viewing frustum. If it does find a point inside all six planes of the viewing
// frustum it returns true, otherwise it returns false.
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	DirectX::XMVECTOR pointVector;

	// check if any one point of the cube is in the view frustum
	for (size_t i = 0; i < 6; i++)
	{
		// far left down point
		pointVector = { xCenter - radius, yCenter - radius, zCenter - radius };
		if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], pointVector)) >= 0.0f)
		{
			continue;
		}

		//  far right down point
		point



	}
}




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
float planeDotCoord(DirectX::XMVECTOR plane, float x, float y, float z)
{

}