////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: an implementation of a plane model
//
// Created:     19.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "Plane.h"

Plane::Plane()
{
	// ATTENTION: this construct exists because a Plane class is a parent for
	// the SpriteClass
}

Plane::Plane(ModelInitializerInterface* pModelInitializer)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);
		this->AllocateMemoryForElements();
		this->GetModelDataObj()->SetID(modelType_);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't create a plane model");
	}
}

Plane::~Plane()
{
	std::string debugMsg{ "destroyment of the " + this->GetModelDataObj()->GetID() };
	Log::Debug(THIS_FUNC, debugMsg.c_str());
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////

// initialization of the model
bool Plane::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// as this model type (Plane) is default we have to get a path to the 
	// default models directory to get a data file
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

	// generate and set a path to the data file
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	bool result = Model::Initialize(this->GetModelDataObj()->GetPathToDataFile(),
		pDevice,
		pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't initialize a plane model");

	return true;
}