////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     SkyDomeClass.cpp
// Description:  this is a model class for the sky dome model
// 
// Created:      15.04.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "SkyDomeClass.h"


SkyDomeClass::SkyDomeClass(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);

	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();

	// setup the model's id
	this->GetModelDataObj()->SetID(modelType_);

	// setup colours of the sky dome
	apexColor_ = { 1.0f, (1.0f / 255.0f) * 69.0f, 0.0f, 1.0f };
	centerColor_ = { 1.0f, (1.0f / 255.0f) * 140.f, 0.1f, 1.0f };
}

SkyDomeClass::~SkyDomeClass()
{
	std::string debugMsg{ "destroyment of the " + this->GetModelDataObj()->GetID() };
	Log::Debug(THIS_FUNC, debugMsg.c_str());
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////


bool SkyDomeClass::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		// as this model type (sky dome) is default we have to get a path to the 
		// default models directory to get a data file
		std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

		// generate and set a path to the data file
		this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

		// initialize the model
		bool result = Model::Initialize(this->GetModelDataObj()->GetPathToDataFile(),
			pDevice,
			pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a sky dome model");

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a sky dome  model");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////


//
// GETTERS
//

const DirectX::XMFLOAT4 & SkyDomeClass::GetApexColor() const
{
	// returns the colour of the sky dome at the very top
	return apexColor_;
}

///////////////////////////////////////////////////////////

const DirectX::XMFLOAT4 & SkyDomeClass::GetCenterColor() const
{
	// returns the colour of the sky dome at the horizon (or 0.0f to be exact)
	return centerColor_;
}