////////////////////////////////////////////////////////////////////
// Filename:     SkyDomeClass.cpp
// Description:  this is a model class for the sky dome model
// 
// Created:      15.04.23
////////////////////////////////////////////////////////////////////
#include "SkyDomeClass.h"


// contains a pointer to the DEFAULT sky dome instance
SkyDomeClass* SkyDomeClass::pDefaultSkyDome_ = nullptr;



SkyDomeClass::SkyDomeClass()
{
	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();

	// setup colours of the sky dome
	apexColor_ = { 0.0f, 0.15f, 0.66f, 1.0f };
	centerColor_ = { 0.81f, 0.38f, 0.66f, 1.0f };
}

SkyDomeClass::~SkyDomeClass()
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
bool SkyDomeClass::Initialize(ID3D11Device* pDevice)
{
	bool result = false;

	// if the DEFAULT model is initialized we can use its data to make BASIC copies of this model
	if (SkyDomeClass::pDefaultSkyDome_ != nullptr)
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic model");

		SkyDomeClass::pDefaultSkyDome_ = this;    // set that this default model was initialized
	}
	// the DEFAULT sky dome isn't initialized yet
	else  
	{
		result = this->InitializeDefault(pDevice);      // so init it
		COM_ERROR_IF_FALSE(result, "can't initialize a default model");
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// the default cube will be used for initialization of the other basic cubes
bool SkyDomeClass::InitializeDefault(ID3D11Device* pDevice)
{
	bool result = false;
	std::string skyDomeID{ "sky_dome" };
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };


	// set what kind of model we want to init
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	result = this->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), skyDomeID);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + skyDomeID);

	return true;
}


// initialization of a new sky dome which basis on the DEFAULT sky dome
bool SkyDomeClass::InitializeNew(ID3D11Device* pDevice)
{
	// try to initialize a copy of the DEFAULT instance of this model
	bool result = this->InitializeCopyOf(SkyDomeClass::pDefaultSkyDome_, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a copy of the DEFAULT " + modelType_);


	return true;
} // InitializeNew()




//
// GETTERS
//

// returns the colour of the sky dome at the very top
const DirectX::XMFLOAT4 & SkyDomeClass::GetApexColor() const
{
	return apexColor_;
}

// returns the colour of the sky dome at the horizon (or 0.0f to be exact)
const DirectX::XMFLOAT4 & SkyDomeClass::GetCenterColor() const
{
	return centerColor_;
}