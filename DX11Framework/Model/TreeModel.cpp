////////////////////////////////////////////////////////////////////
// Filename:     TreeModel.cpp
// Description:  a tree model
// 
// Created:      24.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/TreeModel.h"



TreeModel::TreeModel(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);
	this->modelType_ = "tree";    // a type name of the current model
}

TreeModel::~TreeModel()
{
}