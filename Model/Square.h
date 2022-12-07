/////////////////////////////////////////////////////////////////////
// Filename:    Square.h
// Description: this class is a representation of a 2D square model
// Revising:    07.12.22
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "modelclass.h"
//#include <vector>
#include <array>

//////////////////////////////////
// Class name: Square
//////////////////////////////////
class Square : public ModelClass
{
public:
	Square(float red, float green, float blue);

	bool Initialize(ID3D11Device* pDevice, string modelName);

private:
	std::array<VERTEX, 6> squareVertices_;
	DirectX::XMFLOAT3 color_;
};
