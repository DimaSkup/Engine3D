////////////////////////////////////////////////////////////////////
// Filename:     VertexShader.h
// Description:  this is a class for handling all the vertex shader stuff
//
// Revising:     05.11.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <string>


// a vertex shader container and the related stuff
class VertexShader
{
public:
	~VertexShader();

	bool Initialize(ID3D11Device* pDevice,
					const std::wstring & shaderPath,
					const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
					const UINT layoutElemNum);
	void Shutdown();

	// public query API
	ID3D11VertexShader* GetShader();
	ID3DBlob*           GetBuffer();
	ID3D11InputLayout*  GetInputLayout();

private:
	ID3D11VertexShader* pShader_ = nullptr;
	ID3DBlob*           pShaderBuffer_ = nullptr;
	ID3D11InputLayout*  pInputLayout_ = nullptr;
};

