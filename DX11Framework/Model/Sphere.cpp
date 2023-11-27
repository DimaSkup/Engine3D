////////////////////////////////////////////////////////////////////
// Filename:    Sphere.cpp
// Description: an implementation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"


Sphere::Sphere(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);

	// set that this model has a sphere type
	this->modelType_ = "sphere";
}

Sphere::~Sphere()
{
}