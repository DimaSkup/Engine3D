////////////////////////////////////////////////////////////////////
// Filename:    Sphere.cpp
// Description: an implementation of a sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"


Sphere::Sphere(ModelInitializerInterface* pModelInitializer)
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
		Log::Error(THIS_FUNC, "can't create a sphere model");
		COM_ERROR_IF_FALSE(false, "can't create a sphere model");
	}
}

Sphere::~Sphere()
{
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////


bool Sphere::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// as this model type (Sphere) is default we have to get a path to the 
	// default models directory to get a data file
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

	// generate and set a path to the data file
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	bool result = Model::Initialize(this->GetModelDataObj()->GetPathToDataFile(),
		pDevice,
		pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't initialize a sphere model");

	return true;
}