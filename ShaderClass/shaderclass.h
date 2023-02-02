////////////////////////////////////////////////////////////////////
// Filename:     shaderclass.h
// Description:  this is a base shader class which contains common
//               utils for shader classes to work with shaders
//
// Revising:     12.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>   // is neccessary for the D3DX11CompileFromFile() function
#include <fstream>		
#include <DirectXMath.h>

#include "../Engine/Log.h"
#include "../Engine/StringConverter.h"
#include "../Engine/macros.h"

//////////////////////////////////
// Class name: ShaderClass
//////////////////////////////////
class ShaderClass
{
public:
	virtual bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		DirectX::XMMATRIX worldMatrix,
		DirectX::XMMATRIX viewMatrix,
		DirectX::XMMATRIX projectionMatrix) 
	{
		return true;
	};

	virtual bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
		DirectX::XMMATRIX world,
		DirectX::XMMATRIX view,
		DirectX::XMMATRIX projection,
		ID3D11ShaderResourceView** texturesArray,
		float alpha = 1.0f)
	{
		return true;
	}

	static HRESULT compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
		                                 LPCSTR shaderModel, ID3DBlob** shaderOutput);
};
