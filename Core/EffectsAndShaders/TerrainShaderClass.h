////////////////////////////////////////////////////////////////////
// Filename:     TerrainShaderClass.h
// Description:  this shader class is needed for rendering the terrain
// Created:      11.04.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <DirectXMath.h>
#include <vector>

#include "../Engine/macros.h"
#include "../Engine/Log.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"


//////////////////////////////////
// GLOBALS
//////////////////////////////////
const int _MAX_NUM_POINT_LIGHTS_ON_TERRAIN = 6;


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TerrainShaderClass final
{
// STRUCTURES
private:

	struct ConstantMatrixBuffer_VS
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		//DirectX::XMMATRIX worldViewProj;
	};

	// there are two structures for the diffuse colour and light position arrays
	// that are used in the vertex and pixel shader to create point lighting
	struct PointLightPositionBufferType
	{
		DirectX::XMFLOAT3 lightPosition[_MAX_NUM_POINT_LIGHTS_ON_TERRAIN];
		size_t numPointLights = 0;   // actual number of point light sources on the scene at the moment 
	};

	// a constant light buffer structure for the terrain lighting
	struct ConstantTerrainLightBuffer_TerrainPS
	{
		DirectX::XMFLOAT3 ambientColor;       // a common light of the terrain
		float padding_1;
		DirectX::XMFLOAT3 diffuseColor;       // color of the main directed light
		float padding_2;
		DirectX::XMFLOAT3 lightDirection;     // a direction of the diffuse light
	};

	struct PointLightColorBufferType
	{
		DirectX::XMFLOAT3 diffuseColor[_MAX_NUM_POINT_LIGHTS_ON_TERRAIN];
		size_t numPointLights = 0;   // actual number of point light sources on the scene at the moment 
	};

	// a constant camera buffer structure for the light vertex shader
	struct ConstantCameraBufferType
	{
		DirectX::XMFLOAT3 cameraPosition;
		//float padding = 0.0f;                        // we need the padding because the size of this struct must be a multiple of 16
	};

	// params for controlling the rendering process
	// in the pixel shader
	struct ConstantBufferPerFrame_PS
	{
		DirectX::XMFLOAT3 fogColor;  // the colour of the fog (usually it's a degree of grey)
		float padding_1;
		float fogStart;              // how far from us the fog starts
		float fogRange;              // distance from the fog start position where the fog completely hides the surface point
		float fogRange_inv;          // (1 / fogRange) inversed distance from the fog start position where the fog completely hides the surface point
		float fogEnabled;
		float debugNormals;
	};



public:
	TerrainShaderClass();
	~TerrainShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext);

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	TerrainShaderClass(const TerrainShaderClass & obj);
	TerrainShaderClass & operator=(const TerrainShaderClass & obj);


private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vs_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>               matrixBuffer_;
	ConstantBuffer<PointLightPositionBufferType>          pointLightPositionBuffer_;
	ConstantBuffer<ConstantTerrainLightBuffer_TerrainPS>  diffuseLightBuffer_;
	ConstantBuffer<PointLightColorBufferType>             pointLightColorBuffer_;
	ConstantBuffer<ConstantCameraBufferType>              cameraBuffer_;
	ConstantBuffer<ConstantBufferPerFrame_PS>             bufferPerFrame_;

	const std::string className_{ "terrain_shader" };
};