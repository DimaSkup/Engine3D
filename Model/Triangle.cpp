////////////////////////////////////////////////////////////////////
// Filename:    Triangle.cpp
// Description: realization of a triangle model functional
// Revising:    22.11.22
////////////////////////////////////////////////////////////////////
#include "Triangle.h"

// initialize a triangle
bool Triangle::Initialize(ID3D11Device* pDevice, string modelName, DirectX::XMFLOAT4 color)
{
	Log::Debug(THIS_FUNC, modelName.c_str());
	bool result = false;

	// setup the triangle
	triangle.push_back(VERTEX(-0.5f, -0.5f, 0.0f, 0, 0, 0, 0, 0, color.x, color.y, color.z));  // bottom left (position / texture / normal / colour)
	triangle.push_back(VERTEX( 0.0f,  0.5f, 0.0f, 0, 0, 0, 0, 0, color.x, color.y, color.z));  // top middle
	triangle.push_back(VERTEX( 0.5f, -0.5f, 0.0f, 0, 0, 0, 0, 0, color.x, color.y, color.z));  // bottom right

	result = ModelClass::Initialize(pDevice, triangle.data(), 3, modelName);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize a triangle");
		return false;
	}

	// set up the world matrix for this triangle
	modelWorldMatrix_ = DirectX::XMMatrixIdentity();

	return true;
}