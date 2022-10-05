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
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../ShaderClass/shaderclass.h"

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
	bool InitializeShader(ID3D11Device* device, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader(void);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		        DirectX::XMMATRIX world,
		        DirectX::XMMATRIX view,
		        DirectX::XMMATRIX projection,
		        ID3D11ShaderResourceView* texture,
		        DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 ambientColor,
		        DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, LPCSTR shaderModel, ID3DBlob** shaderBlob);

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

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*  m_pLayout;
	ID3D11SamplerState* m_pSampleState;

	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pCameraBuffer;
	ID3D11Buffer* m_pLightBuffer;
};