////////////////////////////////////////////////////////////////////
// Filename: VertexShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "VertexShader.h"

// initializing a vertex shader interface object
bool VertexShader::Initialize(ID3D11Device* pDevice,
								std::wstring shaderPath,
								D3D11_INPUT_ELEMENT_DESC* layoutDesc,
								UINT layoutElemNum)
{
	HRESULT hr = S_OK;

	// ---------------------------------------------------------------------------------- //
	//                     CREATION OF THE VERTEX SHADER OBJ                              //
	// ---------------------------------------------------------------------------------- //

	// loading of the shader code
	WCHAR* wpShaderPath = &shaderPath[0];

	hr = ShaderClass::compileShaderFromFile(wpShaderPath, "main", "vs_5_0", &this->pShaderBuffer);
	if (FAILED(hr))
	{
		std::string errorMsg{ "Failed to load shader: " };
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


	// ---------------------------------------------------------------------------------- //
	//                       CREATION OF THE INPUT LAYOUT                                 //
	// ---------------------------------------------------------------------------------- //


	// create the input layout
	hr = pDevice->CreateInputLayout(layoutDesc, layoutElemNum,
		pShaderBuffer->GetBufferPointer(),
		pShaderBuffer->GetBufferSize(),
		&pInputLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the input layout for vertex shader");
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

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->pInputLayout;
}