////////////////////////////////////////////////////////////////////
// Filename:     TreeModel.cpp
// Description:  a tree model
// 
// Created:      24.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/TreeModel.h"



TreeModel::TreeModel(ModelInitializerInterface* pModelInitializer)
{
	this->SetModelInitializer(pModelInitializer);
	this->AllocateMemoryForElements();
	this->GetModelDataObj()->SetID(modelType_);
}

TreeModel::~TreeModel()
{
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// initialization of the model
bool TreeModel::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		// as this model type (TreeModel) is default we have to get a path to the 
		// default models directory to get a data file
		std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

		// generate and set a path to the data file
		this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

		// initialize the model
		bool result = Model::Initialize(this->GetModelDataObj()->GetPathToDataFile(),
			pDevice,
			pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a tree model");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a tree model");
		return false;
	}

	return true;
}