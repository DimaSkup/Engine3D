////////////////////////////////////////////////////////////////////
// Filename:    ModelDefault.cpp
// Description: implementation of the ModelDefault class functional
//
// Created:     28.02.23
/////////////////////////////////////////////////////////////////////
#include "ModelDefault.h"


// initialize a copy model which is based on the default model instance
bool ModelDefault::InitializeCopy(ModelClass* pModelCopy, ID3D11Device* pDevice, const std::string & modelType)
{
	bool result = false;
	ModelListClass* pModelList = ModelListClass::Get();
	ModelClass* pBasicModel = pModelList->GetDefaultModelByID(modelType.c_str());  // get a pointer to the basic model object with such a model type

	// initialize a new basic model
	result = ModelClass::InitializeCopy(pBasicModel, pDevice, modelType);
	COM_ERROR_IF_FALSE(result, "can't initialize a new basic " + modelType);

	return true;
}