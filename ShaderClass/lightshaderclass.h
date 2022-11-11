////////////////////////////////////////////////////////////////////
// Filename:     lightshaderclass.h
// Description:  this class is needed for rendering 3D models, 
//               its texture, light on it using HLSL shaders.
// Revising:     10.11.22
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
#include "VertexShader.h"
#include "PixelShader.h"

//#include <d3dcompiler.h>

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
		        DirectX::XMMATRIX world,
		        DirectX::XMMATRIX view, 
		        DirectX::XMMATRIX projection,
		        ID3D11ShaderResourceView* texture,
		        DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 ambientColor,
		        DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 specularColor, float specularPower);


	// memory allocation (we need it because of using DirectX::XM-objects)
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	bool InitializeShaders(ID3D11Device* device, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader(void);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		        DirectX::XMMATRIX world,
		        DirectX::XMMATRIX view,
		        DirectX::XMMATRIX projection,
		        ID3D11ShaderResourceView* texture,
		        DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 ambientColor,
		        DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	

private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct CameraBufferType
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding;  // we need the padding because the size of this struct must be a multiple of 16
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 diffuseColor;    // a main directed light
		DirectX::XMFLOAT4 ambientColor;    // a common light of the scene
		DirectX::XMFLOAT3 lightDirection;  // a direction of the diffuse light
		float             specularPower;   // the intensity of specular light
		DirectX::XMFLOAT4 specularColor;   // the color of specular light
	};

	// classes for work with the vertex and pixel shaders
	VertexShader        vertexShader;
	PixelShader         pixelShader;

	ID3D11SamplerState* pSampleState_ = nullptr;
	ID3D11Buffer*       pMatrixBuffer_ = nullptr;
	ID3D11Buffer*       pCameraBuffer_ = nullptr;
	ID3D11Buffer*       pLightBuffer_ = nullptr;
};