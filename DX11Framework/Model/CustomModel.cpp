////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomModel.h"


// a default constructor
CustomModel::CustomModel(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();
	this->modelType_ = "custom_model";
}

CustomModel::~CustomModel()
{
}