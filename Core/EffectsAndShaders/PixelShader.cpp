////////////////////////////////////////////////////////////////////
// Filename: PixelShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "PixelShader.h"

// initializing a pixel shader interface object
bool PixelShader::Initialize(
	ID3D11Device* pDevice,
	const std::wstring& shaderPath,
	const std::string& funcName)
{
	//Log::Debug(LOG_MACRO);

	HRESULT hr = S_OK;

	// ---------------------------------------------------------------------------------- //
	//                     CREATION OF THE PIXEL SHADER OBJ                               //
	// ---------------------------------------------------------------------------------- //

	// loading of the shader code
	//WCHAR* wpShaderPath = &shaderPath[0];

	hr = ShaderClass::CompileShaderFromFile(
		shaderPath.c_str(),
		funcName.c_str(),
		"ps_5_0",
		&pShaderBuffer_);
	ASSERT_NOT_FAILED(hr, "Failed to load shader: " + StringHelper::ToString(shaderPath));

	// creation of the pixel shader
	hr = pDevice->CreatePixelShader(pShaderBuffer_->GetBufferPointer(),
									pShaderBuffer_->GetBufferSize(),
									nullptr,
									&pShader_);
	ASSERT_NOT_FAILED(hr, "Failed to create a pixel shader: " + StringHelper::ToString(shaderPath));

	return true;
}


ID3D11PixelShader* PixelShader::GetShader()
{
	return this->pShader_;
}

ID3DBlob* PixelShader::GetBuffer()
{
	return this->pShaderBuffer_;
}