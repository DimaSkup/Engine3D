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
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount, 
		        D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
		        ID3D11ShaderResourceView* texture, 
		        D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
		        D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		                     D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
		                     ID3D11ShaderResourceView* texture,
		                     D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
		                     D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext*, int);

	HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, LPCSTR shaderModel, ID3DBlob** outputBlob);
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	// camera buffer structure to matrch the camera constant in the vertex shader.
	// The padding is added to make the structure size a multiple of 16 to prevent
	// CreateBuffer failing when using sizeof with this structure
	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float       specularPower;
		D3DXVECTOR4 specularColor;
	};

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11SamplerState* m_pSampleState;

	ID3D11Buffer*       m_pMatrixBuffer;
	ID3D11Buffer*       m_pLightBuffer;
	ID3D11Buffer*       m_pCameraBuffer;
};
