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


//////////////////////////////////
// Class name: ShaderClass
//////////////////////////////////
class ShaderClass final
{
public:
	static HRESULT CompileShaderFromFile(
		const WCHAR* filename, 
		LPCSTR functionName,
		LPCSTR shaderProfile,
		ID3D10Blob** shaderOutput); 
};
