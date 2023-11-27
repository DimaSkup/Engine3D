////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#include "Cube.h"


Cube::Cube(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);

	// set that this model has a cube type
	this->modelType_ = "cube";
}

Cube::~Cube()
{
}