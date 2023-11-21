////////////////////////////////////////////////////////////////////
// Filename:     TreeModel.cpp
// Description:  a tree model
// 
// Created:      24.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/TreeModel.h"



TreeModel::TreeModel(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();
	this->modelType_ = "tree";    // a type name of the current model
}

TreeModel::~TreeModel()
{
}