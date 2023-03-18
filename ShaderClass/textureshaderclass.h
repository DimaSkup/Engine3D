////////////////////////////////////////////////////////////////////
// Filename:     textureshaderclass.h
// Description:  this class will be used to draw the 3D models
//               using the texture vertex and pixel shaders.
//               It renders only a texture;
//
// Revising:     09.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TextureShaderClass : public ShaderClass
{
public:
	TextureShaderClass(void);
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass(void);

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;
	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		ID3D11ShaderResourceView* const* textureArray,
		DataContainerForShadersClass* pDataForShader) override;

	virtual const std::string & GetShaderName() const override
	{
		return className_;
	}

private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
						   HWND hwnd,
		                   WCHAR* vsFilename, 
						   WCHAR* psFilename);

	bool SetShadersParameters(ID3D11DeviceContext*, 
		                     DirectX::XMMATRIX world,
		                     DirectX::XMMATRIX view,
		                     DirectX::XMMATRIX projection, 
							 ID3D11ShaderResourceView* texture,
							 float alpha = 1.0f);
	void RenderShaders(ID3D11DeviceContext*, int);

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_VS>       matrixConstBuffer_;
	ConstantBuffer<ConstantAlphaBuffer_TexturePS> alphaConstBuffer_;
};
