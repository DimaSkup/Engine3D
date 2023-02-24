////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: an implementation of a plane model
//
// Created:     19.02.23
/////////////////////////////////////////////////////////////////////
#include "Plane.h"

bool Plane::isDefaultInit_ = false;
size_t Plane::planesCounter_ = 1;

Plane::Plane()
{
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// initialization of the model
bool Plane::Initialize(ID3D11Device* pDevice)
{
	bool result = false;

	// if the DEFAULT PLANE model is initialized we can use its data to make BASIC copies of this model
	if (this->IsDefaultPlaneInit())             
	{
		std::string modelId{ modelType_ + "(" + std::to_string(Plane::planesCounter_) + ")" }; // generate an id for the model
		result = this->InitializeNew(pDevice, modelId);
		COM_ERROR_IF_FALSE(result, "can't initialize a new plane");
	}
	else                                             // a DEFAULT SPHERE model isn't initialized yet
	{
		result = this->InitializeDefault(pDevice);   // so init it
		COM_ERROR_IF_FALSE(result, "can't initialize a default plane");
	}

	return true;
}



bool Plane::IsDefaultPlaneInit() const
{
	return Plane::isDefaultInit_;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// the default plane will be used for initialization of the other new planes
bool Plane::InitializeDefault(ID3D11Device* pDevice)
{
	bool result = false;

	// set what kind of model we want to init
	this->SetModelType(this->GetPathToDefaultModelsDir() + modelType_);

	// initialize the model
	result = ModelClass::Initialize(pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + modelType_);

	// add this model to the list of the default models
	ModelListClass::Get()->AddDefaultModel(this, modelType_);

	Plane::isDefaultInit_ = true; // set that this default model was initialized
	Log::Debug(THIS_FUNC, "the default PLANE is initialized");

	return true;
} // InitializeDefault()


// initialization of a new basic plane which basis on the default plane
bool Plane::InitializeNew(ID3D11Device* pDevice, const std::string & modelId)
{
	bool result = false;

	// initialize some stuff
	ModelListClass* pModelList = ModelListClass::Get();
	ModelClass* pDefaultSphere = pModelList->GetDefaultModelByID(modelType_);

	// initialize a new basic model
	result = ModelClass::Initialize(pDefaultSphere, pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a new basic " + modelType_);

	// add this model to the list of models which will be rendered on the scene
	pModelList->AddModelForRendering(this, modelId);

	Plane::planesCounter_++;
	Log::Debug(THIS_FUNC, modelId.c_str());

	return true;
} // InitializeNew()