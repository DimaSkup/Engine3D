////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#include "Cube.h"

bool Cube::isDefaultInit_ = false;


// a default constructor
Cube::Cube()
{
}

Cube::Cube(const Cube & obj)
{
}

Cube::~Cube()
{
	std::string debugMsg{ "destroyment of the " + this->GetID() };
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
	if (Cube::isDefaultInit_)                  
	{
		result = this->InitializeNew(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize a new basic cube");
	}
	else                                       // the DEFAULT cube isn't initialized yet
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

	// set what kind of model we want to init
	this->SetModelType(GetPathToDefaultModelsDir() + modelType_);

	// initialize the model
	result = ModelClass::Initialize(pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + modelType_);

	Cube::isDefaultInit_ = true; // set that this default model was initialized

	return true;
}


// initialization of a new basic sphere which basis on the default sphere
bool Cube::InitializeNew(ID3D11Device* pDevice)
{
	bool result = false;

	std::string modelId{ modelType_ };  // in this case we 
	result = ModelDefault::InitializeCopy(this, pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a new copy of the default CUBE");

	return true;
} // InitializeNew()
