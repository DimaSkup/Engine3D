////////////////////////////////////////////////////////////////////
// Filename: VertexShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "VertexShader.h"


VertexShader::~VertexShader()
{
	this->Shutdown();
}


// initializing a vertex shader interface object
bool VertexShader::Initialize(ID3D11Device* pDevice,
								std::wstring shaderPath,
								D3D11_INPUT_ELEMENT_DESC* layoutDesc,
								UINT layoutElemNum)
{
	//Log::Debug(THIS_FUNC_EMPTY);
	HRESULT hr = S_OK;

	// ---------------------------------------------------------------------------------- //
	//                     CREATION OF THE VERTEX SHADER OBJ                              //
	// ---------------------------------------------------------------------------------- //

	// loading of the shader code
	WCHAR* wpShaderPath = &shaderPath[0];

	hr = ShaderClass::CompileShaderFromFile(wpShaderPath, "main", "vs_5_0", &this->pShaderBuffer);
	if (FAILED(hr))
	{
		std::string errorMsg{ "Failed to load shader: " };
		errorMsg += StringConverter::ToString(shaderPath);

		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
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

		COM_ERROR_IF_FALSE(false, errorMsg.c_str());
	}


	// ---------------------------------------------------------------------------------- //
	//                       CREATION OF THE INPUT LAYOUT                                 //
	// ---------------------------------------------------------------------------------- //


	// create the input layout
	hr = pDevice->CreateInputLayout(layoutDesc, layoutElemNum,
		pShaderBuffer->GetBufferPointer(),
		pShaderBuffer->GetBufferSize(),
		&pInputLayout);
	COM_ERROR_IF_FAILED(hr, "can't create the input layout for vertex shader");

	return true;  // we successfully created a vertex shader object
} /* Initialize() */


// Shutting down of the class object, releasing of the memory, etc.
void VertexShader::Shutdown()
{
	Log::Debug(THIS_FUNC_EMPTY);
	_RELEASE(pShader);
	_RELEASE(pShaderBuffer);
	_RELEASE(pInputLayout);
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