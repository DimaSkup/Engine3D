////////////////////////////////////////////////////////////////////
// Filename:    Sphere.cpp
// Description: an implementation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"


// a static pointer to the DEFAULT sphere
Sphere* Sphere::pDefaultSphere_ = nullptr;     


Sphere::Sphere()
{
	this->AllocateMemoryForElements();
}

Sphere::~Sphere()
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
	// if the DEFAULT SPHERE model is initialized we can use its data to make copies of it
	if (Sphere::pDefaultSphere_)
	{
		this->InitializeNew(pDevice);
	}
	// a DEFAULT SPHERE model isn't initialized yet
	else                                    
	{
		this->InitializeDefault(pDevice);   
		Sphere::pDefaultSphere_ = this;   // set that the DEFAULT sphere was initialized
	}

	return true;
}





/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// the default sphere will be used for initialization of the other basic spheres
void Sphere::InitializeDefault(ID3D11Device* pDevice)
{
	std::string sphereID{ "sphere" };
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

	// set what file we need to use to initialize this model
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	try
	{
		this->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), sphereID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize a DEFAULT sphere");
	}

	return;
} // InitializeDefault()


// initialization of a new SPHERE which basis on the DEFAULT sphere
void Sphere::InitializeNew(ID3D11Device* pDevice)
{
	// try to initialize a copy of the DEFAULT sphere
	try
	{
		this->InitializeCopyOf(Sphere::pDefaultSphere_, pDevice, modelType_);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize a new SPHERE model");
	}

	return;
} // InitializeNew()
