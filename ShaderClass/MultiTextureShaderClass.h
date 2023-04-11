////////////////////////////////////////////////////////////////////
// Filename:     MultiTextureShaderClass.h
// Description:  a class for initialization of multi texture shaders
//               and work with it;
// Created:      09.01.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <DirectXMath.h>
#include <fstream>

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"
#include "../Engine/log.h"

using namespace std;

/////////////////////////////////////////
// Class name: MultiTextureShaderClass
/////////////////////////////////////////
class MultiTextureShaderClass : public ShaderClass
{
public:
	MultiTextureShaderClass();
	MultiTextureShaderClass(const MultiTextureShaderClass& copy);
	~MultiTextureShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		ID3D11ShaderResourceView* const* textureArray,
		DataContainerForShadersClass* pDataForShader) override;

	virtual const std::string & GetShaderName() const override;

private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShadersParameters(ID3D11DeviceContext* pDeviceContext, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, ID3D11ShaderResourceView* const* textureArray);
	void RenderShaders(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixConstBuffer_;
	SamplerState samplerState_;
};
