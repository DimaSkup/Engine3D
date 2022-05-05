////////////////////////////////////////////////////////////////////
// Filename:     lightshaderclass.h
// Description:  this class is needed for rendering our 3D model,
//               its texture and light on it using HLSL shaders.
// 
// Revising:     15.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"

//////////////////////////////////
// Class name: LightShaderClass
//////////////////////////////////
class LightShaderClass
{
public:
	LightShaderClass(void);
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass(void);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext*, int, 
		        D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
		        ID3D11ShaderResourceView*, 
		        D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext*, 
		                     D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		                     ID3D11ShaderResourceView*, 
		                     D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

	HRESULT CompileShaderFromFile(WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float       padding;	// added extra padding so structure is a multiple of 16 for CreateBuffer function requirements
	};

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11SamplerState* m_pSampleState;
	ID3D11Buffer*       m_pMatrixBuffer;
	ID3D11Buffer*       m_pLightBuffer;

};
