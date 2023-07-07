/////////////////////////////////////////////////////////////////////
// Filename:    Triangle.h
// Description: this class is a representation of a 2D triangle model
// Revising:    07.12.22
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////

#include <vector>

#include "../Model/Model.h"


//////////////////////////////////
// Class name: Triangle
//////////////////////////////////
class Triangle : public Model
{
public:
	Triangle(float red, float green, float blue);

	bool Initialize(ID3D11Device* pDevice, string modelName);

private:
	std::vector<VERTEX> triangleVertices_;
	DirectX::XMFLOAT3 color_;
};
