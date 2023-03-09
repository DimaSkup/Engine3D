////////////////////////////////////////////////////////////////////
// Filename:    ModelDefault.cpp
// Description: implementation of the ModelDefault class functional
//
// Created:     28.02.23
/////////////////////////////////////////////////////////////////////
#include "ModelDefault.h"


// initialize a copy model which is based on the default model instance
bool ModelDefault::InitializeCopy(ModelClass* pModelCopy, ID3D11Device* pDevice, const std::string & modelId, const std::string & modelType)
{
	bool result = false;
	ModelListClass* pModelList = ModelListClass::Get();
	ModelClass* pBasicModel = pModelList->GetDefaultModelByID(modelType.c_str());

	// initialize a new basic model
	result = ModelClass::InitializeCopy(pBasicModel, pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a new basic " + modelType);

	// add this model to the list of models which will be rendered on the scene
	pModelList->AddModelForRendering(pModelCopy, modelId);

	return true;
}