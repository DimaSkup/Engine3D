////////////////////////////////////////////////////////////////////
// Filename:     Parallel_LightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Light/LightStore.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: Parallel_LightShaderClass
//////////////////////////////////
class Parallel_LightShaderClass final
{
public:
	Parallel_LightShaderClass();
	~Parallel_LightShaderClass();

	// initialize the shader class object
	bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext);

	// we call this rendering function from the model_to_shader mediator
	bool Render(ID3D11DeviceContext* pDeviceContext,
		const LightSourceDiffuseStore & diffuseLights,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,                     // each model has its own world matrix
		const DirectX::XMMATRIX & viewProj,                                       // common view_matrix * proj_matrix
		const DirectX::XMFLOAT3 & cameraPosition,
		const std::vector<ID3D11ShaderResourceView* const*> & ppDiffuseTextures,  // from the perspective of this shader each model has only one diffuse texture
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		const UINT vertexBufferStride,
		const UINT indexCount);

	const std::string & GetShaderName() const;

	// for controlling of different shader states
	void EnableDisableDebugNormals();
	void EnableDisableFogEffect();
	void SetFogParams(const float fogStart, const float fogRange, const DirectX::XMFLOAT3 & fogColor);

private:  // restrict a copying of this class instance
	Parallel_LightShaderClass(const Parallel_LightShaderClass & obj);
	Parallel_LightShaderClass & operator=(const Parallel_LightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		WCHAR* fxFilename);

private:
	SamplerState        samplerState_;   // a sampler for texturing

	ID3DX11Effect* pFX_ = nullptr;
	ID3DX11EffectTechnique* pTech_ = nullptr;
	ID3D11InputLayout* pInputLayout_ = nullptr;

	// variabled for the vertex shader
	ID3DX11EffectMatrixVariable* pfxWorld_ = nullptr;
	ID3DX11EffectMatrixVariable* pfxWorldViewProj_ = nullptr;
	
	// variables for the pixel shader
	ID3DX11EffectVectorVariable* pfxAmbientColor_ = nullptr;
	ID3DX11EffectScalarVariable* pfxAmbientLightStrength_ = nullptr;
	ID3DX11EffectVectorVariable* pfxDiffuseLightColor_ = nullptr;
	ID3DX11EffectScalarVariable* pfxDiffuseLightStrength_ = nullptr;
	ID3DX11EffectVectorVariable* pfxDiffuseLightDirection_ = nullptr;

	ID3DX11EffectVectorVariable* pfxCameraPos_ = nullptr;
	ID3DX11EffectScalarVariable* pfxIsFogEnabled_ = nullptr;
	ID3DX11EffectScalarVariable* pfxIsDebugNormals_ = nullptr;

	ID3DX11EffectScalarVariable* pfxFogStart_ = nullptr;
	ID3DX11EffectScalarVariable* pfxFogRange_ = nullptr;
	ID3DX11EffectVectorVariable* pfxFogColor_ = nullptr;

	const std::string className_{ "light_shader" };
};