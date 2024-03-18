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
// Class name: LightShaderClass
//////////////////////////////////
class LightShaderClass final
{
public:
	struct ConstantMatrixBuffer_LightVS
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProj;
	};

	// a constant light buffer structure for the light pixel shader
	struct ConstantLightBuffer_LightPS
	{
		DirectX::XMFLOAT3 ambientColor;         // a common light of the scene
		float             ambientLightStrength; // the power of ambient light
		DirectX::XMFLOAT3 diffuseColor;         // color of the main directed light
		float             diffuseLightStrenght; // the power/intensity of the diffuse light
		DirectX::XMFLOAT3 lightDirection;       // a direction of the diffuse light
		float             padding_2;
	};

	struct ConstantBufferPerFrame_LightPS
	{
		float  fogEnabled;
		float debugNormals;
		float fogStart;              // how far from us the fog starts
		float fogRange;              // (1 / range) inversed distance from the fog start position where the fog completely hides the surface point

		DirectX::XMFLOAT3 fogColor;  // the colour of the fog (usually it's a degree of grey)		
	};


public:
	LightShaderClass();
	~LightShaderClass();

	// initialize the shader class object
	bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext);

	// we call this rendering function from the model_to_shader mediator
	bool Render(ID3D11DeviceContext* pDeviceContext,
		const LightSourceDiffuseStore & diffuseLights,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,                     // each model has its own world matrix
		const DirectX::XMMATRIX & viewProj,                                       // common view_matrix * proj_matrix
		const DirectX::XMFLOAT3 & cameraPosition,
		const DirectX::XMFLOAT3 & fogColor,
		const std::vector<ID3D11ShaderResourceView* const*> & ppDiffuseTextures,  // from the perspective of this shader each model has only one diffuse texture
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		const UINT vertexBufferStride,
		const UINT indexCount,
		const float fogStart,
		const float fogRange,
		const bool  fogEnabled);

	const std::string & GetShaderName() const;

private:  // restrict a copying of this class instance
	LightShaderClass(const LightShaderClass & obj);
	LightShaderClass & operator=(const LightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_LightVS>   matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_LightPS>    lightBuffer_;
	ConstantBuffer<ConstantCameraBufferType>       cameraBuffer_;
	ConstantBuffer<ConstantBufferPerFrame_LightPS> bufferPerFrame_;

	const std::string className_{ "light_shader" };
};