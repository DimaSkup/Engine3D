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
		        D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
		        ID3D11ShaderResourceView* texture, D3DXVECTOR4 textColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader(void);
	
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		                     D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX ortho,
		                     ID3D11ShaderResourceView* texture, D3DXVECTOR4 textColor);
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
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX ortho;
	};

	struct PixelBufferType //contains just the pixel colour of the text that will be rendered
	{
		D3DXVECTOR4 pixelColor;
	};
};
