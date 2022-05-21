////////////////////////////////////////////////////////////////////
// Filename:     lightshaderclass.h
// Description:  this class is needed for rendering 3D models, 
//               its texture, light on it using HLSL shaders.
// Revising:     21.05.22
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
	LightShaderClass(const LightShaderClass& anotherObj);
	~LightShaderClass(void);

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		        D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
		        ID3D11ShaderResourceView* texture,
		        D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
		        D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);

private:
	bool InitializeShader(ID3D11Device* device, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader(void);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		                     D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
		                     ID3D11ShaderResourceView* texture,
		                     D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
		                     D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, LPCSTR shaderModel, ID3DBlob** shaderBlob);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;  // we need the padding because the size of this struct must be a multiple of 16
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;    // a main directed light
		D3DXVECTOR4 ambientColor;    // a common light of the scene
		D3DXVECTOR3 lightDirection;  // a direction of the diffuse light
		float       specularPower;   // the intensity of specular light
		D3DXVECTOR4 specularColor;   // the color of specular light
	};

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11SamplerState* m_pSampleState;

	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pCameraBuffer;
	ID3D11Buffer* m_pLightBuffer;

};




























