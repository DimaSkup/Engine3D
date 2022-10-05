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

#include "../ShaderClass/shaderclass.h"
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
	HRESULT compileShaderFromFile(WCHAR* shaderFilename, LPCSTR functionName,
		                          LPCSTR shaderModel, ID3DBlob** shaderOutput);

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

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pInputLayout;
	ID3D11SamplerState* m_pSamplerState;

	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pPixelBuffer;
};