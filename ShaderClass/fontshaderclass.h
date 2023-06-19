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
class FontShaderClass final
{
public:
	FontShaderClass(void);
	~FontShaderClass(void);

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* deviceContext, 
				UINT indexCount,
		        const DirectX::XMMATRIX & world, 
				const DirectX::XMMATRIX & view,
				const DirectX::XMMATRIX & ortho,
		        ID3D11ShaderResourceView* texture, 
				const DirectX::XMFLOAT4 & textColor);

private:  // restrict a copying of this class instance
	FontShaderClass(const FontShaderClass & obj);
	FontShaderClass & operator=(const FontShaderClass & obj);


private:
	// initializes the HLSL shaders, input layout, sampler state and buffers
	bool InitializeShaders(ID3D11Device* pDevice,
						   ID3D11DeviceContext* pDeviceContext,
		                   WCHAR* vsFilename, WCHAR* psFilename);

	// sets up parameters for the vertex and pixel shaders
	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & ortho,
		ID3D11ShaderResourceView* texture,
		const DirectX::XMFLOAT4 & pixelColor);

	void RenderShaders(ID3D11DeviceContext* pDeviceContext, UINT indexCount); 

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_FontVS> matrixBuffer_;
	ConstantBuffer<ConstantPixelBuffer_FontPS>  pixelBuffer_;   // text colour for the pixel shader
};