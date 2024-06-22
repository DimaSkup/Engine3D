////////////////////////////////////////////////////////////////////
// Filename: VertexShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "VertexShader.h"
#include "shaderclass.h"
#include "../Engine/Log.h"
#include "../Engine/macros.h"

#include <fstream>		


VertexShader::~VertexShader()
{
	this->Shutdown();
}


// initializing a vertex shader interface object
bool VertexShader::Initialize(ID3D11Device* pDevice,
	const std::wstring & shaderPath,
	const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
	const UINT layoutElemNum)
{
	// THIS FUNC compiles an HLSL shader by shaderPath;
	// compiles this shader into buffer, and then creates
	// a vertex shader object and an input layout using this buffer;

	try
	{
		HRESULT hr = S_OK;

		// loading of the shader code
		//WCHAR* wpShaderPath = &shaderPath[0];

		// compile a vertex shader into the buffer
		hr = ShaderClass::CompileShaderFromFile(
			shaderPath.c_str(), 
			"VS",
			"vs_5_0",
			&pShaderBuffer_);
		ASSERT_NOT_FAILED(hr, "Failed to compile a shader from file: " + StringHelper::ToString(shaderPath));

		hr = pDevice->CreateVertexShader(
			pShaderBuffer_->GetBufferPointer(),
			pShaderBuffer_->GetBufferSize(),
			nullptr,
			&pShader_);
		ASSERT_NOT_FAILED(hr, "Failed to create a vertex shader: " + StringHelper::ToString(shaderPath));

		hr = pDevice->CreateInputLayout(
			layoutDesc, 
			layoutElemNum,
			pShaderBuffer_->GetBufferPointer(),
			pShaderBuffer_->GetBufferSize(),
			&pInputLayout_);
		ASSERT_NOT_FAILED(hr, "can't create the input layout for vertex shader: " + StringHelper::ToString(shaderPath));
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't compile the vertex shader: " + StringHelper::ToString(shaderPath));

		this->Shutdown();

		return false;
	}

	// we successfully created a vertex shader object and the input layout for it
	return true;  
}

///////////////////////////////////////////////////////////

void VertexShader::Shutdown()
{
	// Shutting down of the class object, releasing of the memory, etc.

	Log::Debug(LOG_MACRO);

	_RELEASE(pInputLayout_);
	_RELEASE(pShaderBuffer_);
	_RELEASE(pShader_);
}



///////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC QUERY API
///////////////////////////////////////////////////////////////////////////////////////////

ID3D11VertexShader* VertexShader::GetShader()
{
	return pShader_;
}

ID3DBlob* VertexShader::GetBuffer()
{
	return pShaderBuffer_;
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return pInputLayout_;
}