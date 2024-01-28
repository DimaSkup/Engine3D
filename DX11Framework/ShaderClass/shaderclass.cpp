////////////////////////////////////////////////////////////////////
// Filename: shaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "shaderclass.h"
#include <iostream>


ShaderClass::~ShaderClass()
{
}



// Compiles shader's bytecode from a HLSL file
HRESULT ShaderClass::CompileShaderFromFile(WCHAR* filename, LPCSTR functionName,
	                                       LPCSTR shaderModel, ID3DBlob** shaderOutput)
{

	//Log::Get()->Debug("%s() (%d): %S:%s()   %s", 
	//	__FUNCTION__, __LINE__, filename, functionName, "start");

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

	// If the shader failed to compile it should write something about the error
	if (errorMsg != nullptr)
	{
		Log::Error(LOG_MACRO, static_cast<char*>(errorMsg->GetBufferPointer()));
		_RELEASE(errorMsg);
	}

	//Log::Get()->Debug("%s() (%d): %S:%s()   %s", __FUNCTION__, __LINE__, filename, functionName, "is compiled");

	return hr;
}


///////////////////////////////////////////////////////////

bool ShaderClass::Render(ID3D11DeviceContext* pDeviceContext, DataContainerForShaders* pDataForShader)
{
	Log::Error(LOG_MACRO, "ERROR: YOU HAVE TO OVERRIDE THIS FUNCTION");
	COM_ERROR_IF_FALSE(false, "ERROR: YOU HAVE TO OVERRIDE THIS FUNCTION");
	return false;
} // end Render

