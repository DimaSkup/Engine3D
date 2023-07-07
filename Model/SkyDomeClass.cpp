////////////////////////////////////////////////////////////////////
// Filename:     SkyDomeClass.cpp
// Description:  this is a model class for the sky dome model
// 
// Created:      15.04.23
////////////////////////////////////////////////////////////////////

#include "SkyDomeClass.h"

bool SkyDomeClass::isDefaultInit_ = false;   // is the default sky dome model already initialized?


SkyDomeClass::SkyDomeClass()
{
	apexColor_ = { 0.0f, 0.15f, 0.66f, 1.0f };
	centerColor_ = { 0.81f, 0.38f, 0.66f, 1.0f };
}

SkyDomeClass::~SkyDomeClass()
{
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


// 
bool SkyDomeClass::Initialize(ID3D11Device* pDevice)
{
	assert(pDevice != nullptr);

	bool result = false;

	// if the DEFAULT model is initialized we can use its data to make BASIC copies of this model
	if (SkyDomeClass::isDefaultInit_)
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic model");

		SkyDomeClass::isDefaultInit_ = true; // set that this default model was initialized
	}
	// the DEFAULT cube isn't initialized yet
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

	// set what kind of model we want to init
	this->SetPathToDataFile(GetPathToDefaultModelsDir() + modelType_);

	// initialize the model
	result = ModelClass::Initialize(pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + modelType_);

	return true;
}


// initialization of a new basic sphere which basis on the default sphere
bool SkyDomeClass::InitializeNew(ID3D11Device* pDevice)
{
	bool result = false;

	result = ModelDefault::InitializeCopy(this, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a new copy of the default SKY DOME");

	return true;
} // InitializeNew()


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