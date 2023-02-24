////////////////////////////////////////////////////////////////////
// Filename:    Cube.cpp
// Description: an implementation of a cube model
// Revising:    14.02.23
/////////////////////////////////////////////////////////////////////
#include "Cube.h"

bool Cube::isDefaultInit_ = false;
size_t Cube::cubesCounter_ = 1;


// a default constructor
Cube::Cube()
{
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

	if (Cube::isDefaultInit_)                  // if the DEFAULT CUBE model is initialized we can use its data to make BASIC copies of this model
	{
		result = this->InitializeNewBasicCube(pDevice);
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
	this->SetModelType("internal/" + modelType_);

	// initialize the model
	result = ModelClass::Initialize(pDevice, modelType_);
	COM_ERROR_IF_FALSE(result, "can't initialize a DEFAULT " + modelType_);

	// add this model to the list of the default models
	ModelListClass::Get()->AddDefaultModel(this, modelType_.c_str());

	Cube::isDefaultInit_ = true; // set that this default model was initialized

	return true;
}


// initialization of a new basic sphere which basis on the default sphere
bool Cube::InitializeNewBasicCube(ID3D11Device* pDevice)
{
	bool result = false;

	// initialize some stuff
	std::string modelId{ modelType_ + "(" + std::to_string(Cube::cubesCounter_) + ")" };
	ModelListClass* pModelList = ModelListClass::Get();
	ModelClass* pDefaultCube = pModelList->GetDefaultModelByID(modelType_.c_str());

	// initialize a new basic model
	result = ModelClass::Initialize(pDefaultCube, pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a new basic " + modelType_);

	// add this model to the list of models which will be rendered on the scene
	pModelList->AddModelForRendering(this, modelId);

	Cube::cubesCounter_++;
	Log::Debug(THIS_FUNC, modelId.c_str());

	return true;
} // InitializeNew()