////////////////////////////////////////////////////////////////////
// Filename:     fontshaderclass.h
// Description:  this is a class for rendering fonts images
//               using HLSL shaders.
//
// Revising:     12.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"
#include "shaderclass.h"

#include <fstream>
#include <DirectXMath.h>

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
		        ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 textColor);

	// memory allocation
	void* operator new(size_t i)
	{
		void* ptr = _aligned_malloc(i, 16);
		if (!ptr)
		{
			Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
			return nullptr;
		}

		return ptr;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader(void);
	
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		                     DirectX::XMMATRIX world, 
		                     DirectX::XMMATRIX view, 
		                     DirectX::XMMATRIX ortho,
		                     ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 textColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	HRESULT compileShaderFromFile(WCHAR* filename, LPCSTR functionName, 
		                          LPCSTR shaderModel, ID3DBlob** shaderOutput);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11Buffer*       m_pMatrixBuffer;
	ID3D11Buffer*       m_pPixelBuffer;
	ID3D11SamplerState* m_pSampleState;

private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX ortho;
	};

	struct PixelBufferType //contains just the pixel colour of the text that will be rendered
	{
		DirectX::XMFLOAT4 pixelColor;
	};
};
