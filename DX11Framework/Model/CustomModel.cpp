////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomModel.h"


// a default constructor
CustomModel::CustomModel(ID3D11Device* pDevice,	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->modelType_ = "custom_model";
}

CustomModel::~CustomModel()
{
}