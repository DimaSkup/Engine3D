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
#include <fstream>		

#include "shaderclass.h"
#include "../Engine/Log.h"
#include "../Engine/macros.h"

// a vertex shader container and the related stuff
class VertexShader
{
public:
	~VertexShader() {
		Log::Debug(THIS_FUNC_EMPTY);
		_RELEASE(pShader);
		_RELEASE(pShaderBuffer);
		_RELEASE(pInputLayout);
	}

	bool Initialize(ID3D11Device* pDevice,
					std::wstring shaderPath,
					D3D11_INPUT_ELEMENT_DESC* layoutDesc,
					UINT layoutElemNum);

	ID3D11VertexShader* GetShader();
	ID3DBlob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();

private:
	ID3D11VertexShader* pShader = nullptr;
	ID3DBlob* pShaderBuffer = nullptr;
	ID3D11InputLayout*  pInputLayout = nullptr;
};

