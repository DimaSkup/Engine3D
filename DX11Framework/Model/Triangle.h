/////////////////////////////////////////////////////////////////////
// Filename:    Triangle.h
// Description: this class is a representation of a 3D triangle model
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
	Triangle(ModelInitializerInterface* pModelInitializer);
	~Triangle();

	virtual bool Initialize(ID3D11Device* pDevice) override;
	
	void SetColor(const float red, const float green, const float blue, const float alpha);
	void SetColor(const DirectX::XMFLOAT4 & color);

private:
	//std::vector<VERTEX> triangleVertices_;
};