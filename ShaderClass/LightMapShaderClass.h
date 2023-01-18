////////////////////////////////////////////////////////////////////
// Filename:     LightMapShaderClass.h
// Description:  a class for initialization of light map texture shaders
//               and work with it;
//
// Created:      13.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"


//////////////////////////////////
// Class name: LightMapShaderClass
//////////////////////////////////
class LightMapShaderClass
{
public:
	LightMapShaderClass();
	LightMapShaderClass(const LightMapShaderClass& copy);
	~LightMapShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd);
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
				const DirectX::XMMATRIX & world,
				const DirectX::XMMATRIX & view,
				const DirectX::XMMATRIX & projection,
				ID3D11ShaderResourceView** textureArray);


private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
						   HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShadersParameters(ID3D11DeviceContext* pDeviceContext, 
							  const DirectX::XMMATRIX& world,
							  const DirectX::XMMATRIX& view,
							  const DirectX::XMMATRIX& projection,
							  ID3D11ShaderResourceView** textureArray);
	void RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;
};