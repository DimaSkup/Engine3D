////////////////////////////////////////////////////////////////////
// Filename:    DataContainerForShadersClass.h
// Description: contains different data which used by different 
//              shaders for the rendering process
// 
// Created:     10.02.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>

#include "../Render/lightclass.h"


//////////////////////////////////
// Class name: DataContainerForShadersClass
//////////////////////////////////
class DataContainerForShadersClass
{
public:
	DataContainerForShadersClass();

	void Update(DirectX::XMMATRIX* pView, 
		DirectX::XMMATRIX* pProjection,
		DirectX::XMMATRIX* pOrtho,
		DirectX::XMFLOAT3 & pCameraPos,
		LightClass* pDiffuseLight);

	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetProjectionMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;
	const DirectX::XMFLOAT3 & GetCameraPosition() const;

	const LightClass* GetDiffuseLight() const;

private:
	DirectX::XMMATRIX* pViewMatrix_ = nullptr;
	DirectX::XMMATRIX* pProjectionMatrix_ = nullptr;
	DirectX::XMMATRIX* pOrthoMatrix_ = nullptr;

	DirectX::XMFLOAT3* pCameraPosition_ = nullptr;

	LightClass* pDiffuseLight_ = nullptr;
};
