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
	Sphere(ModelInitializerInterface* pModelInitializer);
	~Sphere();

	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override;

private:
	std::string modelType_{ "sphere" };     // a type name of the current model
};
