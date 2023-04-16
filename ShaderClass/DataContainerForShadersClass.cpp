////////////////////////////////////////////////////////////////////
// Filename:    DataContainerForShadersClass.cpp
// 
// Created:     10.02.23
////////////////////////////////////////////////////////////////////
#include "DataContainerForShadersClass.h"

// static data
DataContainerForShadersClass* DataContainerForShadersClass::pInstance_ = nullptr;


// a constructor 
DataContainerForShadersClass::DataContainerForShadersClass(EditorCamera* pCamera)
{
	Log::Debug(THIS_FUNC_EMPTY);

	if (pInstance_ == nullptr)
	{
		pInstance_ = this;
		this->pCamera_ = pCamera;
	}
	else
	{
		COM_ERROR_IF_FALSE(false, "you can't create more than only one instance of this class");
	}
}


// returns a static pointer to the instance of this class
DataContainerForShadersClass* DataContainerForShadersClass::Get()
{
	return pInstance_;
}


// each frame we update different parameters which are needed for shaders
void DataContainerForShadersClass::Update(DirectX::XMMATRIX* pView,
	DirectX::XMMATRIX* pProjection,
	DirectX::XMMATRIX* pOrtho,
	LightClass* pDiffuseLight)
{
	this->pViewMatrix_ = pView;
	this->pProjectionMatrix_ = pProjection;
	this->pOrthoMatrix_ = pOrtho;
	this->pDiffuseLight_ = pDiffuseLight;
}



///////////////
//
// SETTERS
//
///////////////

// set colours for the sky dome (these params are using by the SkyDomeShaderClass for rendering the sky dome)
void DataContainerForShadersClass::SetSkyDomeApexColor(const DirectX::XMFLOAT4 & pSkyDomeApexColor)
{
	pSkyDomeApexColor_ = pSkyDomeApexColor;
}

void DataContainerForShadersClass::SetSkyDomeCenterColor(const DirectX::XMFLOAT4 & pSkyDomeCenterColor)
{
	pSkyDomeCenterColor_ = pSkyDomeCenterColor;
}


///////////////
//
// GETTERS
//
///////////////

const DirectX::XMMATRIX & DataContainerForShadersClass::GetViewMatrix() const
{
	return *pViewMatrix_;
}

const DirectX::XMMATRIX & DataContainerForShadersClass::GetProjectionMatrix() const
{
	return *pProjectionMatrix_;
}

const DirectX::XMMATRIX & DataContainerForShadersClass::GetOrthoMatrix() const
{
	return *pOrthoMatrix_;
}

const DirectX::XMFLOAT3 & DataContainerForShadersClass::GetCameraPosition() const
{
	return pCamera_->GetPositionFloat3();
}

const LightClass* DataContainerForShadersClass::GetDiffuseLight() const
{
	return pDiffuseLight_;
}

const DirectX::XMFLOAT4 & DataContainerForShadersClass::GetSkyDomeApexColor() const
{
	return pSkyDomeApexColor_;
}

const DirectX::XMFLOAT4 & DataContainerForShadersClass::GetSkyDomeCenterColor() const
{
	return pSkyDomeCenterColor_;
}