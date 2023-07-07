/////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: this class is a representation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"

//////////////////////////////////
// Class name: Sphere
//////////////////////////////////
class Sphere : public Model
{
public:
	Sphere();
	~Sphere();

	virtual bool Initialize(ID3D11Device* pDevice) override;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	std::string modelType_{ "sphere" }; // a type name of the current model
	static bool isDefaultInit_;         // defines whether we initialized the default sphere or not
};
