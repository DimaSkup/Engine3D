////////////////////////////////////////////////////////////////////
// Filename: PixelShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "PixelShader.h"

#include "shaderclass.h"
#include <fstream>	
#include "../Engine/Log.h"
#include "../Engine/macros.h"


bool PixelShader::Initialize(
	ID3D11Device* pDevice,
	const std::wstring& shaderPath,
	const std::string& funcName)
{
	// initializing a pixel shader interface object

	try
	{
		HRESULT hr = S_OK;
		std::string errorMgr;

		// loading of the shader code
		hr = ShaderClass::CompileShaderFromFile(
			shaderPath.c_str(),
			funcName.c_str(),
			"ps_5_0",
			&pShaderBuffer_,
			errorMgr);
		ASSERT_NOT_FAILED(hr, errorMgr);

		
		hr = pDevice->CreatePixelShader(
			pShaderBuffer_->GetBufferPointer(),
			pShaderBuffer_->GetBufferSize(),
			nullptr,
			&pShader_);

		if (FAILED(hr))
		{
			std::string errMgr;

			errMgr += "Failed to create a pixel shader obj: ";
			errMgr += StringHelper::ToString(shaderPath);
			errMgr += "::" + funcName + "()";

			THROW_ERROR(errMgr);
		}
	}
	catch (EngineException& e)
	{
		Shutdown();

		Log::Error(e, true);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void PixelShader::Shutdown()
{
	_RELEASE(pShader_);
	_RELEASE(pShaderBuffer_);
}