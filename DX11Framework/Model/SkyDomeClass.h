////////////////////////////////////////////////////////////////////
// Filename:     SkyDomeClass.h
// Description:  this is a model class for the sky dome model
// 
// Created:      15.04.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Model/Model.h"


//////////////////////////////////
// Class name: SkyDomeClass
//////////////////////////////////
class SkyDomeClass : public Model
{
public:
	SkyDomeClass(ModelInitializerInterface* pModelInitializer);
	~SkyDomeClass();

	virtual bool Initialize(const std::string & filePath,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext) override;

	const DirectX::XMFLOAT4 & GetApexColor() const;
	const DirectX::XMFLOAT4 & GetCenterColor() const;

private:
	// default apex and center colours of the sky dome (black)
	DirectX::XMFLOAT4 apexColor_{ 1.0f, 1.0f, 1.0f, 1.0f };     // top
	DirectX::XMFLOAT4 centerColor_{ 1.0f, 1.0f, 1.0f, 1.0f };   // horizon
};
