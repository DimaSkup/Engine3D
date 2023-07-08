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
	SkyDomeClass();
	~SkyDomeClass();

	virtual bool Initialize(ID3D11Device* pDevice) override;

	const DirectX::XMFLOAT4 & GetApexColor() const;
	const DirectX::XMFLOAT4 & GetCenterColor() const;

private:
	bool InitializeDefault(ID3D11Device* pDevice);
	bool InitializeNew(ID3D11Device* pDevice);

private:
	// a static pointer to the DEFAULT sky dome: we use it for not loading sky dome 
	// data each time when we create a new sky dome model so we just use data of 
	// the DEFAULT sky dome
	static SkyDomeClass* pDefaultSkyDome_;

	Model* pModel_ = nullptr;              // for using all the model's functional
	std::string modelType_{ "sky_dome" };  // a type name of the current model

	// default apex and center colours of the sky dome (black)
	DirectX::XMFLOAT4 apexColor_{ 1.0f, 1.0f, 1.0f, 1.0f };     // top
	DirectX::XMFLOAT4 centerColor_{ 1.0f, 1.0f, 1.0f, 1.0f };   // horizon
};
