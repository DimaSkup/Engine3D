////////////////////////////////////////////////////////////////////
// Filename:    Square.cpp
// Description: realization of a 2D square model functional
// Revising:    07.12.22
////////////////////////////////////////////////////////////////////
#include "Square.h"

Square::Square(float red, float green, float blue)
	:color_(red, green, blue)
{
}

// initialize a 2D Square
bool Square::Initialize(ID3D11Device* pDevice, string modelName)
{
	/*
	Log::Debug(THIS_FUNC, modelName.c_str());
	bool result = false;

	// setup the square
	squareVertices_[0] = VERTEX(-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,    0, 0, 0,   0, 0, 0,   0, 0, 0,  color_.x, color_.y, color_.z);  // bottom left (position / texture / normal / colour)
	squareVertices_[1] = VERTEX(-0.5f,  0.5f, 0.0f,    0.0f, 0.0f,    0, 0, 0,   0, 0, 0,   0, 0, 0,  color_.x, color_.y, color_.z);  // top left
	squareVertices_[2] = VERTEX( 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,    0, 0, 0,   0, 0, 0,   0, 0, 0,  color_.x, color_.y, color_.z);  // top right

	squareVertices_[3] = squareVertices_[0];                                                              // bottom left (position / texture / normal / colour)
	squareVertices_[4] = squareVertices_[2];                                                              // top right
	squareVertices_[5] = VERTEX( 0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  0, 0, 0,   0, 0, 0,   0, 0, 0,  color_.x, color_.y, color_.z);  // bottom right

	result = ModelClass::Initialize(pDevice, squareVertices_.data(), (int)squareVertices_.size(), modelName);
	if (!result)
	{
	Log::Error(THIS_FUNC, "can't initialize a triangle");
	return false;
	}
	*/

	return true;
}
