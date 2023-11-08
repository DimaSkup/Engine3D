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
	this->GetModelDataObj()->SetID(modelType_);
}

CustomModel::~CustomModel()
{
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

// initialization of the model
bool CustomModel::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{

	// initialize the model
	bool result = Model::Initialize(filePath,
		pDevice,
		pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't initialize a model: " + this->GetModelDataObj()->GetID());

	return true;
}