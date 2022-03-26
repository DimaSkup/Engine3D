/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

using namespace std;

//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass
{
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);	// handle initializing of the shader
	void Shutdown(void);					// handle shutting down of the shader
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX); // sets the shader parameters and then draws the prepared model vertices using the shader

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader(void);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11Buffer*       m_matrixBuffer;
};