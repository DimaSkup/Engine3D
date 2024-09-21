////////////////////////////////////////////////////////////////////
// Filename: VertexShader.cpp
// Revising: 05.11.22
////////////////////////////////////////////////////////////////////
#include "VertexShader.h"

#include "shaderclass.h"
#include "../Common/Log.h"
#include "../Common/MemHelpers.h"
#include "../Common/Assert.h"

#include <fstream>	
#include <string>


namespace Render
{


VertexShader::~VertexShader()
{
	Shutdown();
}

///////////////////////////////////////////////////////////

bool VertexShader::Initialize(
	ID3D11Device* pDevice,
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
		std::string errorMgr;
		std::string funcName = "VS";


		// compile a vertex shader into the buffer
		hr = ShaderClass::CompileShaderFromFile(
			shaderPath.c_str(), 
			funcName.c_str(),
			"vs_5_0",
			&pShaderBuffer_,
			errorMgr);
		Assert::NotFailed(hr, errorMgr);


		hr = pDevice->CreateVertexShader(
			pShaderBuffer_->GetBufferPointer(),
			pShaderBuffer_->GetBufferSize(),
			nullptr,
			&pShader_);

		if (FAILED(hr))
		{
			std::string errMgr;

			errMgr += "Failed to create a vertex shader obj: ";
			errMgr += StringHelper::ToString(shaderPath);
			errMgr += "::" + funcName + "()";

			throw LIB_Exception(errMgr);
		}

		hr = pDevice->CreateInputLayout(
			layoutDesc, 
			layoutElemNum,
			pShaderBuffer_->GetBufferPointer(),
			pShaderBuffer_->GetBufferSize(),
			&pInputLayout_);
		Assert::NotFailed(hr, "can't create the input layout for vertex shader: " + StringHelper::ToString(shaderPath));
	}
	catch (LIB_Exception & e)
	{
		Log::Error(e, true);
		Shutdown();

		return false;
	}

	return true;  
}

///////////////////////////////////////////////////////////

void VertexShader::Shutdown()
{
	// Shutting down of the class object, releasing of the memory, etc.

	Log::Debug("Shutdown");

	SafeRelease(&pInputLayout_);
	SafeRelease(&pShaderBuffer_);
	SafeRelease(&pShader_);
}

}; // namespace Render