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
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"





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

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd);

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
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		        DirectX::XMMATRIX world,
		        DirectX::XMMATRIX view,
		        DirectX::XMMATRIX projection,
		        ID3D11ShaderResourceView* texture,
		        DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 ambientColor,
		        DirectX::XMFLOAT3 cameraPosition, DirectX::XMFLOAT4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	

private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_LightPS>  lightBuffer_;
	ConstantBuffer<ConstantCameraBuffer_LightVS> cameraBuffer_;
};