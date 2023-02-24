////////////////////////////////////////////////////////////////////
// Filename:    Sphere.cpp
// Description: an implementation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"

bool Sphere::isDefaultInit_ = false;
size_t Sphere::spheresCounter_ = 1;

Sphere::Sphere()
{
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// initialization of the model
bool Sphere::Initialize(ID3D11Device* pDevice)
{
	bool result = false;

	if (Sphere::isDefaultInit_)             // if the DEFAULT SPHERE model is initialized we can use its data to make BASIC copies of this model
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic sphere");
	}
	else                                    // a DEFAULT SPHERE model isn't initialized yet
	{
		result = this->InitializeDefault(pDevice);   // so init it
		COM_ERROR_IF_FALSE(result, "can't initialize a default sphere");
	}

	return true;
}





/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// the default sphere will be used for initialization of the other basic spheres
bool Sphere::InitializeDefault(ID3D11Device* pDevice)
{
	bool result = false;


	// set what kind of model we want to init
	this->SetModelType(this->GetPathToDefaultModelsDir() + modelType_);

	// initialize the model
	result = ModelClass::Initialize(pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + modelType_);

	// add this model to the list of the default models
	ModelListClass::Get()->AddDefaultModel(this, modelType_);

	Sphere::isDefaultInit_ = true; // set that this default model was initialized
	Log::Debug(THIS_FUNC, "the default model is initialized");

	return true;
} // InitializeDefault()


// initialization of a new basic sphere which basis on the default sphere
bool Sphere::InitializeNew(ID3D11Device* pDevice)
{
	bool result = false;

	// initialize some stuff
	std::string modelId{ modelType_ + "(" + std::to_string(Sphere::spheresCounter_) + ")" };
	ModelListClass* pModelList = ModelListClass::Get();
	ModelClass* pDefaultSphere = pModelList->GetDefaultModelByID(modelType_);
	
	// initialize a new basic model
	result = ModelClass::Initialize(pDefaultSphere, pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a new basic " + modelType_);

	// add this model to the list of models which will be rendered on the scene
	pModelList->AddModelForRendering(this, modelId);

	Sphere::spheresCounter_++;
	Log::Debug(THIS_FUNC, modelId.c_str());

	return true;
} // InitializeNew()