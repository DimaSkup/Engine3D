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
#include "ModelDefault.h"


//////////////////////////////////
// Class name: SkyDomeClass
//////////////////////////////////
class SkyDomeClass : public ModelDefault
{
public:
	SkyDomeClass();
	~SkyDomeClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;

	const DirectX::XMFLOAT4 & GetApexColor() const;
	const DirectX::XMFLOAT4 & GetCenterColor() const;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	std::string modelType_{ "sky_dome" };  // a type name of the current model
	static bool isDefaultInit_;            // defines whether we initialized the default model or not
	//DirectX::XMFLOAT4 apexColor_{ 1.0f, 49.0f / 255.0f, 5.0f / 255.0f, 1.0f }; // (orange) // the colour at the top of the sky dome
	//DirectX::XMFLOAT4 centerColor_{ 1.0f, 0.0f, 0.0f, 1.0f }; // (red) // the colour at the center of the sky dome


	DirectX::XMFLOAT4 apexColor_{ 0.0f, 0.15f, 0.66f, 1.0f };
	DirectX::XMFLOAT4 centerColor_{ 0.81f, 0.38f, 0.66f, 1.0f };
};
