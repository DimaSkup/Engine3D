////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <map>

#include "../Engine/Log.h"
#include "../Light/LightHelper.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"        // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include <assimp/material.h>



//**********************************************************************************
//                DECLARATIONS OF STRUCTURES FOR CONST BUFFERS
//**********************************************************************************

namespace ConstBuffersTypes
{
	struct constBufferPerObj
	{
		// a structure for data which is changed for each geometry object (each model)

		DirectX::XMMATRIX  world;
		DirectX::XMMATRIX  worldInvTranspose;
		DirectX::XMMATRIX  worldViewProj;
		DirectX::XMMATRIX  texTransform;
		Material           material;
	};

	struct constBufferPerFrame
	{
		// a structure for data which is changed each frame

		DirectionalLight   dirLights[3];
		PointLight         pointLights;
		SpotLight          spotLights;
		DirectX::XMFLOAT3  cameraPosW;    // eye position in the world
	};

	struct constBufferRareChanged
	{
		// a structure for data which is rarely changed
		float debugNormals;
		float fogEnabled;
		float turnOnFlashLight;
		float numOfDirLights;
	};
}


//**********************************************************************************
// Class name: LightShaderClass
//**********************************************************************************
class LightShaderClass final
{
public:
	LightShaderClass();
	~LightShaderClass();

	// initialize the shader class object
	bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext);

	// prepare the rendering pipeline to render using this kind of shaders
	void PrepareForRendering(ID3D11DeviceContext* pDeviceContext);

	// setup light sources params for this frame
	void SetLights(
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMFLOAT3 & cameraPos,               // eyePos
		const std::vector<DirectionalLight> & dirLights,
		const std::vector<PointLight> & pointLights,
		const std::vector<SpotLight> & spotLights);

	void RenderGeometry(
		ID3D11DeviceContext* pDeviceContext,
		const Material& material,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMMATRIX & texTransform,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,
		const std::map<aiTextureType, ID3D11ShaderResourceView* const*> & textures,
		const UINT indexCount);

	const std::string & GetShaderName() const;

	// for controlling of different shader states
	void EnableDisableDebugNormals(ID3D11DeviceContext* pDeviceContext);
	void EnableDisableFogEffect(ID3D11DeviceContext* pDeviceContext);
	void ChangeFlashLightState(ID3D11DeviceContext* pDeviceContext);
	void SetNumberOfDirectionalLights_ForRendering(ID3D11DeviceContext* pDeviceContext, const UINT numOfLights);
	void SetFogParams(const float fogStart, const float fogRange, const DirectX::XMFLOAT3 & fogColor);

private:  // restrict a copying of this class instance
	LightShaderClass(const LightShaderClass & obj);
	LightShaderClass & operator=(const LightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;    // a sampler for texturing

	ConstantBuffer<ConstBuffersTypes::constBufferPerObj>      constBuffPerObj_;
	ConstantBuffer<ConstBuffersTypes::constBufferPerFrame>    constBuffPerFrame_;
	ConstantBuffer<ConstBuffersTypes::constBufferRareChanged> constBuffRareChanged_;

	const std::string className_{ "light_shader" };
};