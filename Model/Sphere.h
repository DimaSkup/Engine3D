/////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: this class is a representation of a 3D sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "modelclass.h"
#include "modellistclass.h"

//////////////////////////////////
// Class name: Square
//////////////////////////////////
class Sphere : public ModelClass
{
public:
	Sphere();

	virtual bool Initialize(ID3D11Device* pDevice) override;
	//bool Initialize(ID3D11Device* pDevice, const std::string& modelId);
	//bool Initialize(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId);

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNewBasicSphere(ID3D11Device* pDevice);

private:
	static bool isDefaultInit_;
	static size_t spheresCounter_;
};
