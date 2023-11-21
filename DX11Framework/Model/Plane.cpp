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
		this->modelType_ = "plane";
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't create a plane model");
		COM_ERROR_IF_FALSE(false, "can't create a plane model");
	}
}

Plane::~Plane()
{
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////


bool Plane::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		// initialize the model
		bool result = Model::Initialize(filePath,
			pDevice,
			pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize a plane model");

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a plane model");
		return false;
	}

	return true;
}