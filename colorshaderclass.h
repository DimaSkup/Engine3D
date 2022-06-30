/////////////////////////////////////////////////////////////////////
// Filename:       colorshaderclass.h
// Description:    We use this class to invoke HLSL shaders 
//                 for drawing our 3D models which are on the GPU
//
// Revising:       06.04.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"

#include <DirectXMath.h>


//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass
{
public:
	ColorShaderClass(void);
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass(void);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext*, int, 
		        DirectX::XMMATRIX world, 
		        DirectX::XMMATRIX view, 
		        DirectX::XMMATRIX projection);

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
	bool InitializeShader(ID3D11Device* device, HWND hwnd,
		                  WCHAR* vertexShaderFilename, WCHAR* pixelShaderFilename);	// compilation and setting of shaders
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext*, 
		                     DirectX::XMMATRIX world, 
		                     DirectX::XMMATRIX view, 
		                     DirectX::XMMATRIX projection);	// here we setup the constant shader buffer
	void RenderShader(ID3D11DeviceContext*, int);

	HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, LPCSTR shaderModel, ID3DBlob** shaderOutput);

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
};

