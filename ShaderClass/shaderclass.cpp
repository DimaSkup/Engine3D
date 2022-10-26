////////////////////////////////////////////////////////////////////
// Filename: shaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "shaderclass.h"

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

bool VertexShader::Initialize(ID3D11Device* pDevice,
								std::wstring shaderPath,
								//LPCSTR functionName,  // is always equal to "main"
								//LPCSTR shaderModel,   // is always equal to vs_5_0
								ID3DBlob** shaderOutput)
{
	WCHAR* wpShaderPath = &shaderPath[0];
	HRESULT hr = ShaderClass::compileShaderFromFile(wpShaderPath, "main", "vs_5_0", shaderOutput);
	return true;
}