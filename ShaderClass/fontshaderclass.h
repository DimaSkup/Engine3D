////////////////////////////////////////////////////////////////////
// Filename:     fontshaderclass.h
// Description:  this is a class for rendering fonts images
//               using HLSL shaders.
//
// Revising:     23.07.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <fstream>
#include <DirectXMath.h>

#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"

#include "../Engine/Log.h"


//////////////////////////////////
// Class name: FontShaderClass
//////////////////////////////////
class FontShaderClass final : public ShaderClass
{
public:
	FontShaderClass();
	~FontShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	// for calling a render function using an instance of the FontShaderClass
	bool Render(ID3D11DeviceContext* deviceContext, 
		UINT indexCount,
		const DirectX::XMMATRIX & world, 
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & ortho,
		ID3D11ShaderResourceView* const texture,
		const DirectX::XMFLOAT4 & textColor);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;

private:  // restrict a copying of this class instance
	FontShaderClass(const FontShaderClass & obj);
	FontShaderClass & operator=(const FontShaderClass & obj);

private:
	// initializes the HLSL shaders, input layout, sampler state and buffers
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	// sets up parameters for the vertex and pixel shaders
	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & ortho,
		ID3D11ShaderResourceView* const texture,
		const DirectX::XMFLOAT4 & pixelColor);

	void RenderShaders(ID3D11DeviceContext* pDeviceContext, const UINT indexCount); 

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_FontVS> matrixBuffer_;
	ConstantBuffer<ConstantPixelBuffer_FontPS>  pixelBuffer_;   // text colour for the pixel shader
};