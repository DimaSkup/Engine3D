////////////////////////////////////////////////////////////////////
// Filename:     textureshaderclass.h
// Description:  this class will be used to draw the 3D models
//               using the texture vertex and pixel shaders.
//
// Revising:     09.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "shaderclass.h"


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TextureShaderClass
{
public:
	TextureShaderClass(void);
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass(void);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext*, int, 
		        DirectX::XMMATRIX world, 
		        DirectX::XMMATRIX view,
		        DirectX::XMMATRIX projection, ID3D11ShaderResourceView* texture);

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
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader(void);
	
	bool SetShaderParameters(ID3D11DeviceContext*, 
		                     DirectX::XMMATRIX world,
		                     DirectX::XMMATRIX view,
		                     DirectX::XMMATRIX projection, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext*, int);

	HRESULT CompileShaderFromFile(WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);

private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11Buffer*       m_pMatrixBuffer;
	ID3D11SamplerState* m_pSampleState;
};
