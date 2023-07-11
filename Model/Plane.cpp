////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: an implementation of a plane model
//
// Created:     19.02.23
/////////////////////////////////////////////////////////////////////
#include "Plane.h"


// contains a pointer to the DEFAULT PLANE instance
Plane* Plane::pDefaultPlane_ = nullptr;



Plane::Plane()
{
	this->AllocateMemoryForElements();
}

Plane::~Plane()
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
bool Plane::Initialize(ID3D11Device* pDevice)
{
	bool result = false;

	// if the DEFAULT PLANE model is initialized we can use its data to make BASIC copies of this model
	if (Plane::pDefaultPlane_ != nullptr)             
	{
		result = this->InitializeNew(pDevice, modelType_);
		COM_ERROR_IF_FALSE(result, "can't initialize a new plane");
	}
	else     // a DEFAULT plane model isn't initialized yet
	{
		result = this->InitializeDefault(pDevice);   // so init it
		COM_ERROR_IF_FALSE(result, "can't initialize a default plane");
	}

	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// the default plane will be used for initialization of the other new planes_
bool Plane::InitializeDefault(ID3D11Device* pDevice)
{
	bool result = false;
	std::string planeID{ "plane" };
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

	// set what kind of model we want to init
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	result = this->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), planeID);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + planeID);

	return true;
} // InitializeDefault()


// initialization of a new plane which basis on the DEFAULT plane
bool Plane::InitializeNew(ID3D11Device* pDevice, const std::string & modelId)
{
	// try to initialize a copy of the DEFAULT instance of this model
	bool result = this->InitializeCopyOf(Plane::pDefaultPlane_, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a copy of the DEFAULT " + modelType_);

	return true;
} // InitializeNew()