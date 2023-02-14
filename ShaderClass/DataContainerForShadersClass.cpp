////////////////////////////////////////////////////////////////////
// Filename:    DataContainerForShadersClass.cpp
// 
// Created:     10.02.23
////////////////////////////////////////////////////////////////////
#include "DataContainerForShadersClass.h"

// static data
DataContainerForShadersClass* DataContainerForShadersClass::pInstance_ = nullptr;


// a constructor 
DataContainerForShadersClass::DataContainerForShadersClass(CameraClass* pCamera)
{
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