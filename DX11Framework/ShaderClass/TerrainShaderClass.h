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
#include <fstream>
#include <DirectXMath.h>
#include <vector>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "../Render/lightclass.h"


//////////////////////////////////
// GLOBALS
//////////////////////////////////
const int _MAX_NUM_POINT_LIGHTS_ON_TERRAIN = 6;


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TerrainShaderClass : public ShaderClass
{
// STRUCTURES
private:
	// a constant light buffer structure for the terrain lighting
	struct ConstantTerrainLightBuffer_TerrainPS
	{
		DirectX::XMFLOAT3 ambientColor;       // a common light of the terrain
		DirectX::XMFLOAT3 diffuseColor;       // color of the main directed light
		DirectX::XMFLOAT3 lightDirection;     // a direction of the diffuse light
	};

	// there are two structures for the diffuse colour and light position arrays
	// that are used in the vertex and pixel shader to create point lighting
	struct PointLightPositionBufferType
	{
		DirectX::XMFLOAT4 lightPosition[_MAX_NUM_POINT_LIGHTS_ON_TERRAIN];
		size_t numPointLights = 0;   // actual number of point light sources on the scene at the moment 
	};

	struct PointLightColorBufferType
	{
		DirectX::XMFLOAT3 diffuseColor[_MAX_NUM_POINT_LIGHTS_ON_TERRAIN];
		size_t numPointLights = 0;   // actual number of point light sources on the scene at the moment 
	};

	// params for controlling the rendering process
	// in the pixel shader
	struct ConstantBufferPerFrame_PS
	{
		DirectX::XMFLOAT3 fogColor;  // the colour of the fog (usually it's a degree of grey)
		float fogStart;              // how far from us the fog starts
		float fogRange;              // distance from the fog start position where the fog completely hides the surface point
		bool  fogEnabled;
		bool  useAlphaClip;
	};



public:
	TerrainShaderClass();
	~TerrainShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,  // contains terrain diffuse textures and normal maps
		const std::vector<LightClass*>* ptrToDiffuseLightsArr,
		const std::vector<LightClass*>* ptrToPointLightsArr,
		const DirectX::XMFLOAT3 & cameraPosition,
		const DirectX::XMFLOAT3 & fogColor,
		const float fogStart,
		const float fogRange,
		const bool  fogEnabled);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	TerrainShaderClass(const TerrainShaderClass & obj);
	TerrainShaderClass & operator=(const TerrainShaderClass & obj);


private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,  // contains terrain diffuse textures and normal maps
		const std::vector<LightClass*> & diffuseLightsArr,
		const std::vector<LightClass*> & pointLightsArr,
		const DirectX::XMFLOAT3 & cameraPosition,
		const DirectX::XMFLOAT3 & fogColor,
		const float fogStart,
		const float fogRange,
		const bool  fogEnabled);


	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>               matrixBuffer_;
	ConstantBuffer<ConstantTerrainLightBuffer_TerrainPS>  diffuseLightBuffer_;

	ConstantBuffer<PointLightPositionBufferType>       pointLightPositionBuffer_;
	ConstantBuffer<PointLightColorBufferType>          pointLightColorBuffer_;
	ConstantBuffer<ConstantCameraBuffer_LightVS>       cameraBuffer_;
	ConstantBuffer<ConstantBufferPerFrame_PS>          bufferPerFrame_;
};