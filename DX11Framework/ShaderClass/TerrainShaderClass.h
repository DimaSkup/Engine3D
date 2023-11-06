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
const int _NUM_POINT_LIGHTS_ON_TERRAIN = 4;    


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TerrainShaderClass : public ShaderClass
{
// STRUCTURES
private:
	// there are two structures for the diffuse colour and light position arrays
	// that are used in the vertex and pixel shader to create point lighting
	struct PointLightColorBufferType
	{
		DirectX::XMFLOAT4 diffuseColor[_NUM_POINT_LIGHTS_ON_TERRAIN];
	};

	struct PointLightPositionBufferType
	{
		DirectX::XMFLOAT4 lightPosition[_NUM_POINT_LIGHTS_ON_TERRAIN];
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
		ID3D11ShaderResourceView* const* pTextureArray,  // contains terrain diffuse textures and normal maps
		LightClass* pDiffuseLightSources,
		const DirectX::XMFLOAT4* pPointLightsPositions,
		const DirectX::XMFLOAT4* pPointLightsColors);

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
		ID3D11ShaderResourceView* const* pTextureArray,  // contains terrain diffuse textures and normal maps
		LightClass* pDiffuseLightSources,
		const DirectX::XMFLOAT4* pPointLightsPositions,
		const DirectX::XMFLOAT4* pPointLightsColors);

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
};