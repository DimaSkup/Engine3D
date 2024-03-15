////////////////////////////////////////////////////////////////////
// Filename:     PointLightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with multiple POINT LIGHTS on it using HLSL shaders.
//
// Created:      28.08.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// GLOBALS
//////////////////////////////////
const int NUM_POINT_LIGHTS = 25;


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dx11effect.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Render/LightStore.h"

#include "shaderclass.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState (for texturing)


#if 0
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#endif



//////////////////////////////////
// Class name: PointLightShaderClass
//////////////////////////////////
class PointLightShaderClass final
{
private:
	struct ConstantMatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProj;
	};

	// there are two structures for the diffuse colour and light position arrays
	// that are used in the vertex and pixel shader
	struct PointLightColorBufferType
	{
		DirectX::XMFLOAT3 diffuseColor[NUM_POINT_LIGHTS];
	};

	struct PointLightPositionBufferType
	{
		DirectX::XMFLOAT3 lightPosition[NUM_POINT_LIGHTS];
	};

	// a constant buffer structure for the light pixel shader (contains data of diffuse light)
	struct DiffuseLightBufferType
	{
		DirectX::XMFLOAT3 ambientColor;         // a common light of the scene
		float             ambientLightStrength; // the power of ambient light
		DirectX::XMFLOAT3 diffuseColor;         // color of the main directed light
		float             padding_1;
		DirectX::XMFLOAT3 lightDirection;       // a direction of the diffuse light
		float             padding_2;
	};

public:
	PointLightShaderClass();
	~PointLightShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	void Render(ID3D11DeviceContext* pDeviceContext,
		const LightSourceDiffuseStore & diffuseLights,
		const LightSourcePointStore & pointLights,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMFLOAT3 & cameraPosition,
		const DirectX::XMFLOAT3 & fogColor,
		const std::vector<ID3D11ShaderResourceView* const*> & ppDiffuseTextures,
		ID3D11Buffer* vertexBufferPtr,
		ID3D11Buffer* indexBufferPtr,
		const UINT vertexBufferStride,
		const UINT indexCount,
		//const std::vector<ID3D11Buffer*> & vertexBuffersPtrs,
		//const std::vector<ID3D11Buffer*> & indexBuffersPtrs,
		//const std::vector<UINT> & vertexBuffersStrides,
		//const std::vector<UINT> & indexCounts,
		//const UINT numOfModels,
		const float fogStart,
		const float fogRange,
		const bool  fogEnabled);

#if 0
	void Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & viewProj,
		const LightSourcePointStore & pointLights,
		ID3D11ShaderResourceView* const* ppDiffuseTexture);
#endif

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	PointLightShaderClass(const PointLightShaderClass & obj);
	PointLightShaderClass & operator=(const PointLightShaderClass & obj);

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
	ID3DX11EffectVectorVariable* pfxPointLightPositions_ = nullptr;

	// variables for the pixel shader
	ID3DX11EffectVectorVariable* pfxPointLightColors_ = nullptr;
	ID3DX11EffectVectorVariable* pfxAmbientColor_ = nullptr;
	ID3DX11EffectScalarVariable* pfxAmbientLightStrength_ = nullptr;
	ID3DX11EffectVectorVariable* pfxDiffuseLightColor_ = nullptr;
	ID3DX11EffectVectorVariable* pfxDiffuseLightDirection_ = nullptr;

#if 0
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBufferType>      matrixBuffer_;
	ConstantBuffer<DiffuseLightBufferType>        diffuseLightBuffer_;
	ConstantBuffer<PointLightColorBufferType>     pointLightColorBuffer_;
	ConstantBuffer<PointLightPositionBufferType>  pointLightPositionBuffer_;
#endif

	const std::string className_{ "point_light_shader" };
};