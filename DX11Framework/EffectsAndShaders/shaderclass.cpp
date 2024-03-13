////////////////////////////////////////////////////////////////////
// Filename: shaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "shaderclass.h"


#include <d3dx11async.h>   // is neccessary for the D3DX11CompileFromFile() function
#include "../Engine/Log.h"
#include "../Engine/StringHelper.h"
#include "../Engine/macros.h"


// Compiles effect's or shader's bytecode from a .fx or .hlsl files respectively
HRESULT ShaderClass::CompileEffectOrShaderFromFile(
	WCHAR* filename, 
	LPCSTR functionName,
	LPCSTR shaderProfile, 
	ID3D10Blob** shaderOutput)
{

	//Log::Get()->Debug("%s() (%d): %S:%s()   %s", 
	//	__FUNCTION__, __LINE__, filename, functionName, "start");

	
	ID3DBlob* pErrorMsgs = nullptr;
	DWORD compileFlags = D3D10_SHADER_WARNINGS_ARE_ERRORS | D3D10_SHADER_ENABLE_STRICTNESS;

#if defined(DEBUG) | defined(_DEBUG)
	compileFlags |= D3D10_SHADER_DEBUG;
	compileFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	const HRESULT hr = D3DX11CompileFromFile(
		filename,                   // pSrcFile:      the name of the .hlsl/.fx file that contains the effect souce code we want to compile
		nullptr,                    // pDefines:      advanced option we do not use; see the SDK documentation;
		0,                          // pInclude:      advanced option we do not use; see the SDK documentation;
		functionName,               // pFunctionName: the shader function name entry point. This is only used when compiling shader programs individually. When using the effects framework, specify null, as the technique passes defined inside the effect file specify the shader entry points.
		shaderProfile,              // pProfile:      a string specifying the shader version we are using. For Direct3D 11 effects, we use shader version 5.0 ("fx_5_0")
		compileFlags,               // Flags1:        flags to specify how the shader code should be compiled
		0,                          // Flags2:        advanced effect compilation options we do not use; see the SDK documentation
		nullptr,                    // pPump:         advanced option we do not use to compile the shader asynchronously; see the SDK documentation
		shaderOutput,               // ppShader:      returns a point to a ID3D10Blob data structure that stores the compiled shader
		&pErrorMsgs,                // ppErrorMsgs:   returns a pointer to a ID3D11Blob data structure that stores a string containing the compilation errors, if any.
		nullptr);                   // pHResult:      used to obtain the returned error code if compiling asynchronously. Specify null if you specified null for pPump

	// If the shader failed to compile it should write something about the error
	if (pErrorMsgs != nullptr)
	{
		MessageBoxA(0, (char*)pErrorMsgs->GetBufferPointer(), 0, 0);
		Log::Error(LOG_MACRO, static_cast<char*>(pErrorMsgs->GetBufferPointer()));
		_RELEASE(pErrorMsgs);
	}

	// even if there are no errorMsgs, check to make sure there were no other errors
	COM_ERROR_IF_FAILED(hr, "can't compile the effect or shader from file: " + StringHelper::ToString(filename));

	return hr;
}

///////////////////////////////////////////////////////////

HRESULT ShaderClass::CompileAndCreateEffect(
	WCHAR* srcFile,                                   // the name of the .fx file that contains the effect souce code we want to compile
	ID3DX11Effect** ppFX,                             // a pointer to point to the created effect
	ID3D11Device* pDevice)                            // pointer to the Direct3D 11 device
	
{
	HRESULT hr = S_OK;

	ID3D10Blob* compiledShader = nullptr;
	const std::string shaderVersion{ "fx_5_0" };

	try
	{
		// compile an effect file
		hr = ShaderClass::CompileEffectOrShaderFromFile(
			srcFile,
			nullptr,                                      // since we want to create an effect we set the function name to null
			shaderVersion.c_str(),
			&compiledShader);
		COM_ERROR_IF_FAILED(hr, "can't compile the effect from file: " + StringHelper::ToString(srcFile));

		// create a new effect from the compiled bytecode
		hr = D3DX11CreateEffectFromMemory(
			compiledShader->GetBufferPointer(),           // pointer to the compiled effect data
			compiledShader->GetBufferSize(),              // the byte size of the compile effect data
			0,                                            // effect flags should match the flags specified for Flags2 in the D3DX11CompileFromFile function
			pDevice,                                      // pointer to the Direct3D 11 device
			ppFX);                                        // returns a pointer to the created effect
		COM_ERROR_IF_FAILED(hr, "can't create an effect from memory for the file: " + StringHelper::ToString(srcFile));
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		_RELEASE(compiledShader);
	}

	// done with compiled shader
	_RELEASE(compiledShader);

	return hr;
}