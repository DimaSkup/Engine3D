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
	const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
	const UINT layoutElemNum)
{
	try
	{
		//Log::Debug(LOG_MACRO);
		HRESULT hr = S_OK;

		// ---------------------------------------------------------------------------------- //
		//                     CREATION OF THE VERTEX SHADER OBJ                              //
		// ---------------------------------------------------------------------------------- //

		// loading of the shader code
		WCHAR* wpShaderPath = &shaderPath[0];

		hr = ShaderClass::CompileEffectOrShaderFromFile(wpShaderPath, "main", "vs_5_0", &this->pShaderBuffer_);
		COM_ERROR_IF_FAILED(hr, "Failed to load a shader : " + StringHelper::ToString(shaderPath));

		// creation of the vertex shader
		hr = pDevice->CreateVertexShader(this->pShaderBuffer_->GetBufferPointer(),
			this->pShaderBuffer_->GetBufferSize(),
			nullptr,
			&this->pShader_);
		COM_ERROR_IF_FAILED(hr, "Failed to create a vertex shader: " + StringHelper::ToString(shaderPath));


		// ---------------------------------------------------------------------------------- //
		//                       CREATION OF THE INPUT LAYOUT                                 //
		// ---------------------------------------------------------------------------------- //


		// create the input layout
		hr = pDevice->CreateInputLayout(layoutDesc, layoutElemNum,
			pShaderBuffer_->GetBufferPointer(),
			pShaderBuffer_->GetBufferSize(),
			&pInputLayout_);
		COM_ERROR_IF_FAILED(hr, "can't create the input layout for vertex shader: " + StringHelper::ToString(shaderPath));
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't compile the vertex shader: " + StringHelper::ToString(shaderPath));

		this->Shutdown();

		return false;
	}

	return true;  // we successfully created a vertex shader object
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
	return this->pShader_;
}

ID3DBlob* VertexShader::GetBuffer()
{
	return this->pShaderBuffer_;
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->pInputLayout_;
}