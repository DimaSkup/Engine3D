////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: an implementation of a 3D sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"

bool Sphere::isDefaultInit_ = false;
size_t Sphere::spheresCounter_ = 1;

Sphere::Sphere()
{
}

// for initialization of the default basic sphere model
bool Sphere::Initialize(ID3D11Device* pDevice)
{
	if (Sphere::isDefaultInit_)  // a DEFAULT SPHERE model is initialized so we use its data to make a BASIC copy 
	{
		this->InitializeNewBasicSphere(pDevice);
	}
	else // a DEFAULT SPHERE model isn't initialized yet
	{
		this->InitializeDefault(pDevice);
	}

	return true;
}










/////////////////////////////////////////////////////////////////////////////////////////


// this default sphere will be used for initialization of the other basic spheres
bool Sphere::InitializeDefault(ID3D11Device* pDevice)
{
	Log::Print("INIT A DEFAULT SPHERE");
	bool result = false;

	// set what kind of model we want to init
	this->SetModel("internal/sphere");

	result = ModelClass::Initialize(pDevice, "sphere");
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT SPHERE");

	ModelListClass::Get()->AddDefaultModel(this, "sphere");

	Sphere::isDefaultInit_ = true; // set that the default sphere was initialized
	return true;
}


bool::Sphere::InitializeNewBasicSphere(ID3D11Device* pDevice)
{
	bool result = false;

	// initialize some stuff
	std::string modelId = "sphere(" + std::to_string(Sphere::spheresCounter_) + ")";
	ModelListClass* pModelList = ModelListClass::Get();
	ModelClass* pDefaultSphere = pModelList->GetDefaultModelByID("sphere");

	pModelList->AddModel(this, modelId);
	

	// initialize a new basic sphere
	result = ModelClass::Initialize(pDefaultSphere, pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a new basic sphere");

	Log::Debug(THIS_FUNC, modelId.c_str());
	Sphere::spheresCounter_++;

	return true;
}