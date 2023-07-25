////////////////////////////////////////////////////////////////////
// Filename:     TreeModel.cpp
// Description:  a tree model
// 
// Created:      24.07.23
////////////////////////////////////////////////////////////////////
#include "../Model/TreeModel.h"


// contains a pointer to the DEFAULT TREE MODEL instance
TreeModel* TreeModel::pDefaultTree_ = nullptr;


TreeModel::TreeModel()
{
	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();
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
bool TreeModel::Initialize(ID3D11Device* pDevice)
{
	// if the DEFAULT model is initialized
	if (TreeModel::pDefaultTree_ != nullptr)
	{
		this->InitializeNew(pDevice);
	}
	// the DEFAULT tree isn't initialized yet
	else
	{
		this->InitializeDefault(pDevice);
		TreeModel::pDefaultTree_ = this;   // set that the DEFAULT TREE was initialized
	}


	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// the default tree will be used for initialization of the other trees (TreeModel instances)
void TreeModel::InitializeDefault(ID3D11Device* pDevice)
{
	std::string treeID{ "tree" };
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

	// set what file we need to use to initialize this model
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	try 
	{
		this->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), treeID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize a DEFAULT TREE model");
	}

	return;
}


// initialization of a new TREE which basis on the DEFAULT tree
void TreeModel::InitializeNew(ID3D11Device* pDevice)
{
	// try to initialize a copy of the DEFAULT tree
	try
	{
		this->InitializeCopyOf(TreeModel::pDefaultTree_, pDevice, modelType_);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize a new TREE model");
	}

	return;
}