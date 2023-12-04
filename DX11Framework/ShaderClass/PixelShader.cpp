////////////////////////////////////////////////////////////////////
// Filename: PixelShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "PixelShader.h"

// initializing a pixel shader interface object
bool PixelShader::Initialize(ID3D11Device* pDevice, std::wstring shaderPath)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;

	// ---------------------------------------------------------------------------------- //
	//                     CREATION OF THE PIXEL SHADER OBJ                               //
	// ---------------------------------------------------------------------------------- //

	// loading of the shader code
	WCHAR* wpShaderPath = &shaderPath[0];

	hr = ShaderClass::CompileShaderFromFile(wpShaderPath, "main", "ps_5_0", &this->pShaderBuffer);
	if (FAILED(hr))
	{
		std::string errorMsg{ "Failed to load shader: " };
		errorMsg += StringHelper::ToString(shaderPath);

		Log::Error(THIS_FUNC, errorMsg.c_str());
		return false;
	}

	// creation of the pixel shader
	hr = pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(),
									pShaderBuffer->GetBufferSize(),
									nullptr,
									&this->pShader);
	if (FAILED(hr))
	{
		std::string errorMsg{ "Failed to create a pixel shader: " };
		errorMsg += StringHelper::ToString(shaderPath);

		Log::Error(THIS_FUNC, errorMsg.c_str());
		return false;
	}

	return true;
}


ID3D11PixelShader* PixelShader::GetShader()
{
	return this->pShader;
}

ID3DBlob* PixelShader::GetBuffer()
{
	return this->pShaderBuffer;
}