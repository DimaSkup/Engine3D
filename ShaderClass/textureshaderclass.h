////////////////////////////////////////////////////////////////////
// Filename:     textureshaderclass.h
// Description:  this class will be used to draw the 3D models
//               using the texture vertex and pixel shaders.
//
// Revising:     09.04.22
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
#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TextureShaderClass
{
public:
	TextureShaderClass(void);
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass(void);

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext*, int, 
		        DirectX::XMMATRIX world, 
		        DirectX::XMMATRIX view,
		        DirectX::XMMATRIX projection,
				ID3D11ShaderResourceView* texture);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	bool InitializeShaders(ID3D11Device* pDevice, HWND hwnd,
		                   WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShaders(void);
	
	bool SetShadersParameters(ID3D11DeviceContext*, 
		                     DirectX::XMMATRIX world,
		                     DirectX::XMMATRIX view,
		                     DirectX::XMMATRIX projection, 
							ID3D11ShaderResourceView* texture);
	void RenderShaders(ID3D11DeviceContext*, int);

private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};


	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	ID3D11Buffer*       pMatrixBuffer_ = nullptr;
};
