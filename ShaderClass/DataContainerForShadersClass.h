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

//#include "../Camera/cameraclass.h"
#include "../Camera/EditorCamera.h"
#include "../Render/lightclass.h"


//////////////////////////////////
// Class name: DataContainerForShadersClass
//////////////////////////////////
class DataContainerForShadersClass
{
public:

	DataContainerForShadersClass(EditorCamera* pCamera);
	~DataContainerForShadersClass();

	static DataContainerForShadersClass* Get(void);  // get an instance of the class

	// each frame we update different parameters which are needed for shaders
	void Update(DirectX::XMMATRIX* pView, 
		DirectX::XMMATRIX* pProjection,
		DirectX::XMMATRIX* pOrtho,
		LightClass* pDiffuseLight);

	// set colours for the sky dome (these params are using by the SkyDomeShaderClass for rendering the sky dome)
	void SetSkyDomeApexColor(const DirectX::XMFLOAT4 & pSkyDomeApexColor);
	void SetSkyDomeCenterColor(const DirectX::XMFLOAT4 & pSkyDomeCenterColor);

	// GETTERS
	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetProjectionMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;
	const DirectX::XMFLOAT3 & GetCameraPosition() const;

	const LightClass* GetDiffuseLight() const;
	const DirectX::XMFLOAT4 & GetSkyDomeApexColor() const;
	const DirectX::XMFLOAT4 & GetSkyDomeCenterColor() const;

private:
	static DataContainerForShadersClass* pInstance_;

	DirectX::XMMATRIX* pViewMatrix_ = nullptr;
	DirectX::XMMATRIX* pProjectionMatrix_ = nullptr;
	DirectX::XMMATRIX* pOrthoMatrix_ = nullptr;

	EditorCamera* pCamera_ = nullptr;
	LightClass* pDiffuseLight_ = nullptr;

	DirectX::XMFLOAT4 pSkyDomeApexColor_{};
	DirectX::XMFLOAT4 pSkyDomeCenterColor_{};
};
