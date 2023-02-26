/////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: this class is a representation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "modelclass.h"
#include "modellistclass.h"

//////////////////////////////////
// Class name: Sphere
//////////////////////////////////
class Sphere : public ModelClass
{
public:
	Sphere();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	std::string modelType_{ "sphere" };
	static bool isDefaultInit_;
	static size_t spheresCounter_;
};
