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
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);	// compilation and setting of shaders
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);	// here we setup the constant shader buffer
	void RenderShader(ID3D11DeviceContext*, int);

	HRESULT CompileShaderFromFile(WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11Buffer*       m_pMatrixBuffer;
};

