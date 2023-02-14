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

#include "../Camera/cameraclass.h"
#include "../Render/lightclass.h"


//////////////////////////////////
// Class name: DataContainerForShadersClass
//////////////////////////////////
class DataContainerForShadersClass
{
public:
	DataContainerForShadersClass(CameraClass* pCamera);

	static DataContainerForShadersClass* Get(void);

	void Update(DirectX::XMMATRIX* pView, 
		DirectX::XMMATRIX* pProjection,
		DirectX::XMMATRIX* pOrtho,
		LightClass* pDiffuseLight);

	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetProjectionMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;
	const DirectX::XMFLOAT3 & GetCameraPosition() const;

	const LightClass* GetDiffuseLight() const;

private:
	static DataContainerForShadersClass* pInstance_;

	DirectX::XMMATRIX* pViewMatrix_ = nullptr;
	DirectX::XMMATRIX* pProjectionMatrix_ = nullptr;
	DirectX::XMMATRIX* pOrthoMatrix_ = nullptr;

	CameraClass* pCamera_ = nullptr;
	LightClass* pDiffuseLight_ = nullptr;
};
