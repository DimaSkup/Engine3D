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
#include "../Engine/Log.h"


//////////////////////////////////
// Class name: FontShaderClass
//////////////////////////////////
class FontShaderClass
{
public:
	FontShaderClass(void);
	FontShaderClass(const FontShaderClass& copy);
	~FontShaderClass(void);

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		        DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX ortho,
		        ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 pixelColor);

	// memory allocation (is necessary because of XM-structures)
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:
	// initializes the shaders, input layout, sampler state and buffers
	bool InitializeShaders(ID3D11Device* device, HWND hwnd,
		                   WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShaders(void); // releases the memory from the shaders, buffers, input layout, sampler state, etc.

	// sets up parameters for the vertex and pixel shaders
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, 
		                     DirectX::XMMATRIX world,
		                     DirectX::XMMATRIX view,
		                     DirectX::XMMATRIX ortho,
		                     ID3D11ShaderResourceView* texture,
		                     DirectX::XMFLOAT4 pixelColor);
	void RenderShaders(ID3D11DeviceContext* deviceContext, int indexCount); 

private:
	// contains matrices which are used inside the vertex shader
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX ortho;
	};

	// contains colours which are used inside the pixel shader
	struct PixelBufferType
	{
		DirectX::XMFLOAT4 pixelColor;
	};
	 
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	ID3D11SamplerState* pSamplerState_ = nullptr;
	ID3D11Buffer*       pMatrixBuffer_ = nullptr;
	ID3D11Buffer*       pPixelBuffer_ = nullptr;
};