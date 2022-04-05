/////////////////////////////////////////////////////////////////////
// Filename:       colorshaderclass.h
// Description:    We use this class to invoke HLSL shaders to 
//                 draw our 3D models which are on the GPU
// 
// Revising:       05.04.22
/////////////////////////////////////////////////////////////////////


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
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DMATRIX, D3DXMATRIX);	// here we initialize a shader constant buffer
	void RenderShaders(ID3D11DeviceContext*, int);

	HRESULT CompileShaderFromFile(WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);
	HRESULT CompileShaderFromFile(WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);

};


































/*
/////////////////////////////////////////////////////////////////////
// Filename:    colorshaderclass.h
// Description: this class we use to invoke our HLSL shaders for drawing
//              the 3D models that are on the GPU
// 
// Revising:    29.03.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"

using namespace std;


//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass
{
public:
	ColorShaderClass(void);
	ColorShaderClass(const ColorShaderClass& another);
	~ColorShaderClass(void);

	bool Initialize(ID3D11Device*, HWND);	// calls initialization function for the shaders
	void Shutdown(void);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
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

*/