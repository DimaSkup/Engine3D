////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#include "Cube.h"

// contains a pointer to the DEFAULT CUBE instance
Cube* Cube::pDefaultCube_ = nullptr;


// a default constructor
Cube::Cube()
{
	// allocate memory for the model's common elements
	this->AllocateMemoryForElements();
}

Cube::~Cube()
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
bool Cube::Initialize(ID3D11Device* pDevice)
{
	bool result = false;

	// if the DEFAULT model is initialized we can use its data to make BASIC copies of this model
	if (Cube::pDefaultCube_ != nullptr)
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic cube");

		Cube::pDefaultCube_ = this;    // set that this DEFAULT model was initialized
	}
	// the DEFAULT cube isn't initialized yet
	else   
	{
		result = this->InitializeDefault(pDevice);      // so init it
		COM_ERROR_IF_FALSE(result, "can't initialize a default cube");
	}

	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//                           PRIVATE FUNCTIONS
// 
/////////////////////////////////////////////////////////////////////////////////////////

// the default cube will be used for initialization of the other basic cubes
bool Cube::InitializeDefault(ID3D11Device* pDevice)
{
	bool result = false;
	std::string cubeID{ "cube" };
	std::string defaultModelsDirPath{ Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };


	// set what kind of model we want to init
	this->GetModelDataObj()->SetPathToDataFile(defaultModelsDirPath + modelType_);

	// initialize the model
	result = this->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), cubeID);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + cubeID);

	return true;
}


// initialization of a new CUBE which basis on the DEFAULT cube
bool Cube::InitializeNew(ID3D11Device* pDevice)
{
	// try to initialize a copy of the DEFAULT instance of this model
	bool result = this->InitializeCopyOf(Cube::pDefaultCube_, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a copy of the DEFAULT " + modelType_);

	return true;
} // InitializeNew()
