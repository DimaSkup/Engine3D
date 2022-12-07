////////////////////////////////////////////////////////////////////
// Filename:    Triangle.cpp
// Description: realization of a triangle model functional
// Revising:    22.11.22
////////////////////////////////////////////////////////////////////
#include "Triangle.h"

Triangle::Triangle(float red, float green, float blue)
	:color_( red, green, blue )
{
}

// initialize a triangle
bool Triangle::Initialize(ID3D11Device* pDevice, string modelName)
{
	Log::Debug(THIS_FUNC, modelName.c_str());
	bool result = false;

	// setup the triangle
	triangleVertices_.push_back(VERTEX(-0.5f, -0.5f, 0.0f, 0, 0, 0, 0, 0,  color_.x, color_.y, color_.z));  // bottom left (position / texture / normal / colour)
	triangleVertices_.push_back(VERTEX( 0.0f,  0.5f, 0.0f, 0, 0, 0, 0, 0,  color_.x, color_.y, color_.z));  // top middle
	triangleVertices_.push_back(VERTEX( 0.5f, -0.5f, 0.0f, 0, 0, 0, 0, 0,  color_.x, color_.y, color_.z));  // bottom right

	result = ModelClass::Initialize(pDevice, triangleVertices_.data(), 3, modelName);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize a triangle");
		return false;
	}

	return true;
}