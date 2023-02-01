////////////////////////////////////////////////////////////////////
// Filename:    Sphere.h
// Description: an implementation of a 3D sphere model
// Revising:    01.02.23
/////////////////////////////////////////////////////////////////////
#include "Sphere.h"

Sphere::Sphere()
{
}

// initialize a 3D sphere model
bool Sphere::Initialize(ID3D11Device* pDevice, const std::string& modelId)
{
	Log::Debug(THIS_FUNC_EMPTY);

	// set what kind of model we want to init
	this->SetModel("internal/sphere");

	bool result = ModelClass::Initialize(pDevice, modelId);
	COM_ERROR_IF_FALSE(result, "can't initialize a 3D sphere object");

	/*
	string debugMsg = modelId + " is initialized successfully";
	Log::Debug(THIS_FUNC, debugMsg.c_str());
	*/

	return true;
}
