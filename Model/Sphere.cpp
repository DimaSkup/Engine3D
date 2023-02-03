////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: an implementation of a 3D sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"


size_t Sphere::spheresCounter_ = 0;

Sphere::Sphere()
{
}

// initialize a 3D sphere model
bool Sphere::Initialize(ID3D11Device* pDevice, const std::string& modelId)
{
	Log::Debug(THIS_FUNC_EMPTY);

	// set what kind of model we want to init
	this->SetModel("internal/sphere");

	bool result = ModelClass::Initialize(pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a 3D sphere object");

	/*
	string debugMsg = modelId + " is initialized successfully";
	Log::Debug(THIS_FUNC, debugMsg.c_str());
	*/

	return true;
}


bool Sphere::Initialize(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId)
{
	Log::Debug(THIS_FUNC, modelId.c_str());

	// set what kind of model we want to init
	this->SetModel("internal/sphere");


	ModelClass::Initialize(pModel, pDevice, modelId);

/*
	bool result = false;
	ModelClass* pModel = nullptr;    // a pointer to the model for easier using

									 // add a new model to the models list
	ModelListClass::Get()->AddModel(new ModelClass(), modelId);
	pModel = pGraphics->pModelList_->GetModelByID(modelId);

	// initialize the model
	pModel->SetModel(modelName);
	result = pModel->Initialize(pGraphics->pD3D_->GetDevice(), modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize the models list object");

	// check the result
	COM_ERROR_IF_FALSE(result, { "can't initialize the ModelClass object: " + modelId });

*/

	return true;
}