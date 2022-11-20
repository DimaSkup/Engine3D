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
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
//#include <d3dcompiler.h>
#include <DirectXMath.h>


#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "VertexShader.h"
#include "PixelShader.h"


//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass
{
public:
	ColorShaderClass(void);
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass(void);

	bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount, 
		        DirectX::XMMATRIX worldMatrix, 
		        DirectX::XMMATRIX viewMatrix, 
		        DirectX::XMMATRIX projectionMatrix);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:
	bool InitializeShaders(ID3D11Device* device, HWND hwnd,
		                  WCHAR* vertexShaderFilename, WCHAR* pixelShaderFilename);	// compilation and setting of shaders
	void ShutdownShader(void);

	bool SetShaderParameters(ID3D11DeviceContext*, 
		                     DirectX::XMMATRIX world, 
		                     DirectX::XMMATRIX view, 
		                     DirectX::XMMATRIX projection);	// here we setup the constant shader buffer
	void RenderShader(ID3D11DeviceContext*, int);

	//HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, LPCSTR shaderModel, ID3DBlob** shaderOutput);

private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	VertexShader   vertexShader;
	PixelShader    pixelShader;
	ID3D11Buffer*  pMatrixBuffer_ = nullptr;
};

