////////////////////////////////////////////////////////////////////
// Filename: shaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "shaderclass.h"

HRESULT ShaderClass::compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
	                                       LPCSTR shaderModel, ID3DBlob** shaderOutput)
{

	Log::Get()->Debug("%s (%d): %s:%s", __FUNCTION__, __LINE__, filename, functionName);

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
	if (errorMsg != nullptr)
	{
		Log::Get()->Error(THIS_FUNC, static_cast<char*>(errorMsg->GetBufferPointer()));
		_RELEASE(errorMsg);
	}

	return hr;
}