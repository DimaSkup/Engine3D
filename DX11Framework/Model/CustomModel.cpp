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

	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();
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
bool CustomModel::Initialize(ID3D11Device* pDevice)
{
	Log::Debug(THIS_FUNC_EMPTY);

	std::string ID{ "custom_model" };

	// initialize the model
	bool result = this->InitializeFromFile(pDevice, 
		this->GetModelDataObj()->GetPathToDataFile(), 
		ID);
	COM_ERROR_IF_FALSE(result, "can't initialize a new " + ID);


	return true;
}