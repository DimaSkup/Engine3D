////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     SkyDomeClass.cpp
// Description:  this is a model class for the sky dome model
// 
// Created:      15.04.23
////////////////////////////////////////////////////////////////////////////////////////////
#include "SkyDomeClass.h"



SkyDomeClass::SkyDomeClass(ModelInitializerInterface* pModelInitializer,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	: Model(pDevice, pDeviceContext)
{
	this->SetModelInitializer(pModelInitializer);

	// setup a type name of the current model
	this->modelType_ = "sky_dome";  

	// setup colours of the sky dome
	apexColor_ = { 1.0f, (1.0f / 255.0f) * 69.0f, 0.0f, 1.0f };
	centerColor_ = { 1.0f, (1.0f / 255.0f) * 140.f, 0.1f, 1.0f };
}

SkyDomeClass::~SkyDomeClass()
{
	std::string debugMsg{ "destroyment of the sky dome"};
	Log::Debug(THIS_FUNC, debugMsg.c_str());
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
// 
////////////////////////////////////////////////////////////////////////////////////////////


const DirectX::XMFLOAT4 & SkyDomeClass::GetApexColor() const
{
	// returns the colour of the sky dome at the very top
	return apexColor_;
}

///////////////////////////////////////////////////////////

const DirectX::XMFLOAT4 & SkyDomeClass::GetCenterColor() const
{
	// returns the colour of the sky dome at the horizon (or 0.0f to be exact)
	return centerColor_;
}