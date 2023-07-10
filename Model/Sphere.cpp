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
	std::string debugMsg{ "destroyment of the " + this->GetModelDataObj()->GetID() };
	Log::Debug(THIS_FUNC, debugMsg.c_str());
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

	// if the DEFAULT SPHERE model is initialized we can use its data to make copies of it
	if (Sphere::pDefaultSphere_)
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic sphere");

		// set that this DEFAULT model was initialized
		Sphere::pDefaultSphere_ = this;
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
	std::string sphereID{ "sphere" };
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

	// set what kind of model we want to init
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	result = this->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), sphereID);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + sphereID);

	return true;
} // InitializeDefault()


// initialization of a new SPHERE which basis on the DEFAULT sphere
bool Sphere::InitializeNew(ID3D11Device* pDevice)
{
	// try to initialize a copy of the DEFAULT instance of this model
	bool result = this->InitializeCopyOf(Sphere::pDefaultSphere_, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a copy of the DEFAULT " + modelType_);
	
	return true;
} // InitializeNew()
