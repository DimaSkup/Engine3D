////////////////////////////////////////////////////////////////////
// Filename: PixelShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "PixelShader.h"

#include "shaderclass.h"
#include "../Common/Log.h"
#include "../Common/MemHelpers.h"
#include "../Common/Assert.h"

#include <fstream>	


namespace Render
{

PixelShader::PixelShader()
{

}

PixelShader::~PixelShader()
{
	if (pShader_)
	{
		pShader_->Release();
		pShader_ = nullptr;
	}

	if (pShaderBuffer_)
	{
		pShaderBuffer_->Release();
		pShaderBuffer_ = nullptr;
	}
}

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
		Assert::NotFailed(hr, errorMgr);

		
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

			throw LIB_Exception(errMgr);
		}
	}
	catch (LIB_Exception& e)
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
	SafeRelease(&pShader_);
	SafeRelease(&pShaderBuffer_);
}


} // namespace Render