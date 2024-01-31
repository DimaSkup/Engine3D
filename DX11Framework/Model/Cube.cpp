////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#include "Cube.h"


Cube::Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	// set that this model has a cube type
	this->modelType_ = "cube";
}

Cube::~Cube()
{
}