/////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: this class is a representation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "ModelDefault.h"


//////////////////////////////////
// Class name: Sphere
//////////////////////////////////
class Sphere : public ModelDefault
{
public:
	Sphere();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);
	std::string Sphere::GetID();        // generate an id for the model

private:
	std::string modelType_{ "sphere" }; // a type name of the current model
	static bool isDefaultInit_;         // defines whether we initialized the default sphere or not
	static size_t spheresCounter_;      // this variable is necessary because for each copy of the default sphere we must have a unique ID
};
