////////////////////////////////////////////////////////////////////
// Filename:     SkyPlaneShaderClass.h
// Description:  this is the shader used for rendering the clouds
//               on the sky plane
// Created:      27.06.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"     // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"

//////////////////////////////////
// Class name: SkyPlaneShaderClass
//////////////////////////////////
class SkyPlaneShaderClass : public ShaderClass
{
public:
	SkyPlaneShaderClass(void);
	~SkyPlaneShaderClass(void);

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		ID3D11ShaderResourceView* const* textureArray,  // an array of this model's textures
		DataContainerForShadersClass* pDataForShader) override;

	virtual const std::string & GetShaderName() const override;

private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* texture2, 
		float firstTranslationX,
		float firstTranslationZ,
		float secondTranslationX,
		float secondTranslationZ,
		float brightness);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantSkyPlaneBuffer_PS>    skyBuffer_;
};