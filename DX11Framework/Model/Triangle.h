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
	Triangle(ID3D11Device* pDevice,	ID3D11DeviceContext* pDeviceContext);
	~Triangle();

	virtual bool Initialize(const std::string & filePath, ModelInitializerInterface* pModelInitializer) override;
	
	void SetColor(const float red, const float green, const float blue, const float alpha);
	void SetColor(const DirectX::XMFLOAT4 & color);
};
