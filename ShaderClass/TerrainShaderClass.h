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

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "../Render/lightclass.h"

//#include <d3dcompiler.h>

//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TerrainShaderClass : public ShaderClass
{
public:
	TerrainShaderClass(void);
	TerrainShaderClass(const TerrainShaderClass& anotherObj);
	~TerrainShaderClass(void);

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		ID3D11ShaderResourceView* const* textureArray,
		DataContainerForShadersClass* pDataForShader) override;

	virtual const std::string & GetShaderName() const _NOEXCEPT override;

private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* normalMap,
		//const DirectX::XMFLOAT3 & cameraPosition,
		const DirectX::XMFLOAT4 & diffuseColor,
		const DirectX::XMFLOAT3 & lightDirection,
		const DirectX::XMFLOAT4 & ambientColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantTerrainLightBuffer_TerrainPS>  lightBuffer_;
	//ConstantBuffer<ConstantCameraBuffer_LightVS> cameraBuffer_;
};