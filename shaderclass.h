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
#include "includes.h"
#include "log.h"

#include <d3dcompiler.h>

//////////////////////////////////
// Class name: ShaderClass
//////////////////////////////////
class ShaderClass
{
public:
	static HRESULT compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
		                                 LPCSTR shaderModel, ID3DBlob** shaderOutput);
};
