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
	try
	{
		pModel_ = new Model();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't create an instance of CUBE");
	}
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
	if (pDefaultCube_ != nullptr)
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic cube");
	}
	else   // the DEFAULT cube isn't initialized yet
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
	std::string cubeID{ "cube" };

	// set what kind of model we want to init
	this->GetModelDataObj()->SetPathToDataFile(Settings::Get()->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") + modelType_);

	// initialize the model
	bool result = pModel_->InitializeFromFile(pDevice, this->GetModelDataObj()->GetPathToDataFile(), cubeID);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + cubeID);

	Cube::pDefaultCube_ = this; // set that this DEFAULT model was initialized

	return true;
}


// initialization of a new CUBE which basis on the DEFAULT cube
bool Cube::InitializeNew(ID3D11Device* pDevice)
{
	bool result = false;

	// try to initialize a copy of the DEFAULT instance of this model
	result = pModel_->InitializeCopyOf(Cube::pDefaultCube_, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a new copy of the default CUBE");

	return true;
} // InitializeNew()
