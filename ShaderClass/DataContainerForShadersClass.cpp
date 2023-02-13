////////////////////////////////////////////////////////////////////
// Filename:    DataContainerForShadersClass.cpp
// 
// Created:     10.02.23
////////////////////////////////////////////////////////////////////
#include "DataContainerForShadersClass.h"

DataContainerForShadersClass::DataContainerForShadersClass(CameraClass* pCamera)
{
	this->pCamera_ = pCamera;
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