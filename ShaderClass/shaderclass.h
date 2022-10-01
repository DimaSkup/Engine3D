////////////////////////////////////////////////////////////////////
// Filename:     shaderclass.h
// Description:  this is a base shader class which contains common
//               utils for shader classes to work with shaders
//
// Revising:     12.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>   // is neccessary for the D3DX11CompileFromFile() function
#include <fstream>		

#include "../LogSystem/Log.h"
#include "../Engine/macros.h"

//////////////////////////////////
// Class name: ShaderClass
//////////////////////////////////
class ShaderClass
{
public:
	static HRESULT compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
		                                 LPCSTR shaderModel, ID3DBlob** shaderOutput);
};
