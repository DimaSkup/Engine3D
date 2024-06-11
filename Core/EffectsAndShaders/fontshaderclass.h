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

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"

#include "../Engine/Log.h"



namespace ConstBuffersTypes
{
	// a constant matrix buffer structure for the font vertex shader
	struct ConstantMatrixBuffer_FontVS
	{
		DirectX::XMMATRIX worldViewProj;
	};

	// a constant buffer which contains colours are used inside the font pixel shader
	struct ConstantPixelBuffer_FontPS
	{
		DirectX::XMFLOAT3 pixelColor;         // UI text colour
		float padding;
	};
}


//////////////////////////////////
// Class name: FontShaderClass
//////////////////////////////////
class FontShaderClass final
{
public:
	FontShaderClass();
	~FontShaderClass();

	bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);

	// Public rendering API

	void PrepareForRendering(ID3D11DeviceContext* pDeviceContext);
	void Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);
	
	// Public modification API
	void SetWorldViewOrtho(ID3D11DeviceContext* pDeviceContext, const DirectX::XMMATRIX& WVO);  // set new matrix: world * basic_view * ortho
	void SetFontColor(ID3D11DeviceContext* pDeviceContext, const DirectX::XMFLOAT3 & textColor);
	void SetFontTexture(ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* const* ppFontTexture);
	
	// Public query API
	inline const std::string & GetShaderName() const { return className_; }

private:  // restrict a copying of this class instance
	FontShaderClass(const FontShaderClass & obj);
	FontShaderClass & operator=(const FontShaderClass & obj);

private:
	// initializes the HLSL shaders, input layout, sampler state and buffers
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	ConstantBuffer<ConstBuffersTypes::ConstantMatrixBuffer_FontVS> matrixBuffer_;
	ConstantBuffer<ConstBuffersTypes::ConstantPixelBuffer_FontPS>  pixelBuffer_;   // text colour for the pixel shader

	std::string className_{ "font_shader_class" };
};