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
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Render/LightStore.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"

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
		const std::vector<ID3D11Buffer*> & vertexBuffersPtrs,
		const std::vector<ID3D11Buffer*> & indexBuffersPtrs,
		const std::vector<UINT> & vertexBuffersStrides,
		const std::vector<UINT> & indexCounts,
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
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

#if 0
	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & viewProj,
		const LightSourcePointStore & pointLights,
		ID3D11ShaderResourceView* const* ppDiffuseTexture);

	void RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount);
#endif


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBufferType>  matrixBuffer_;
	ConstantBuffer<DiffuseLightBufferType>        diffuseLightBuffer_;
	ConstantBuffer<PointLightColorBufferType>     pointLightColorBuffer_;
	ConstantBuffer<PointLightPositionBufferType>  pointLightPositionBuffer_;
	//ConstantBuffer<ConstantLightBuffer_LightPS>  lightBuffer_;
	//ConstantBuffer<ConstantCameraBufferType> cameraBuffer_;

	const std::string className_{ "point_light_shader" };
};