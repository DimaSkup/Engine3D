////////////////////////////////////////////////////////////////////
// Filename: shaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "shaderclass.h"
#include <iostream>

// Compiles a shader bytecode from a HLSL file
HRESULT ShaderClass::compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
	                                       LPCSTR shaderModel, ID3DBlob** shaderOutput)
{

	Log::Get()->Debug("%s() (%d): %S:%s()", __FUNCTION__, __LINE__, filename, functionName);

	HRESULT hr = S_OK;
	ID3DBlob* errorMsg = nullptr;
	UINT compileFlags = D3D10_SHADER_WARNINGS_ARE_ERRORS | D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
	compileFlags |= D3D10_SHADER_DEBUG;
#endif

	hr = D3DX11CompileFromFile(filename, nullptr, 0,
		                       functionName, shaderModel,
		                       compileFlags, 0, nullptr,
		                       shaderOutput, &errorMsg, nullptr);

	// If the shader failed to compile it should have writen something to the error message
	if (errorMsg != nullptr)
	{
		Log::Get()->Error(THIS_FUNC, static_cast<char*>(errorMsg->GetBufferPointer()));
		_RELEASE(errorMsg);
	}

	return hr;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                               VERTEX SHADER
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialized a vertex shader interface object
bool VertexShader::Initialize(ID3D11Device* pDevice,
								std::wstring shaderPath)
								//LPCSTR functionName,  // is always equal to "main"
								//LPCSTR shaderModel,   // is always equal to vs_5_0
								//ID3DBlob** shaderOutput
{
	HRESULT hr = S_OK;

	// loading of the shader code
	WCHAR* wpShaderPath = &shaderPath[0];
	hr = ShaderClass::compileShaderFromFile(wpShaderPath, "main", "vs_5_0", &this->pShaderBuffer);
	if (FAILED(hr))
	{
		std::string errorMsg { "Failed to load shader: " };
		errorMsg += StringConverter::ToString(shaderPath);
		
		Log::Error(THIS_FUNC, errorMsg.c_str());
		return false;
	}


	// creation of the vertex shader
	hr = pDevice->CreateVertexShader(this->pShaderBuffer->GetBufferPointer(),
		                             this->pShaderBuffer->GetBufferSize(),
		                             nullptr,
		                             &this->pShader);
	if (FAILED(hr))
	{
		std::string errorMsg{ "Failed to create a vertex shader: " };
		errorMsg += StringConverter::ToString(shaderPath);

		Log::Error(THIS_FUNC, errorMsg.c_str());
		return false;
	}

	return true;  // we successfully created a vertex shader object
}


ID3D11VertexShader* VertexShader::GetShader()
{
	return this->pShader;
}

ID3DBlob* VertexShader::GetBuffer()
{
	return this->pShaderBuffer;
}