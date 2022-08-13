////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
// Revising: 13.08.22
////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

FrustumClass::FrustumClass(void)
{
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

	// calculate near plane of frustum
	m_planes[0].x = fMatrix._14 + fMatrix._13;
	m_planes[0].y = fMatrix._24 + fMatrix._23;
	m_planes[0].z = fMatrix._34 + fMatrix._33;
	m_planes[0].w = fMatrix._44 + fMatrix._43;
	DirectX::XMPlaneNormalize(DirectX::XMLoatFloat)







	return;
}