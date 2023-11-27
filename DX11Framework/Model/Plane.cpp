////////////////////////////////////////////////////////////////////////////////////////////
// Filename:    Plane.h
// Description: an implementation of a plane model
//
// Created:     19.02.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "Plane.h"

Plane::Plane(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	try
	{
		this->SetModelInitializer(pModelInitializer);
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
