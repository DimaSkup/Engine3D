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
#include <d3dx11effect.h>
#include <d3d11.h>


//#include <fstream>		
//#include <memory>          // for using std::unique_ptr and std::make_unique()
//#include <DirectXMath.h>




//////////////////////////////////
// Class name: ShaderClass
//////////////////////////////////
class ShaderClass final
{
public:
	static HRESULT CompileEffectOrShaderFromFile(
		WCHAR* filename, 
		LPCSTR functionName,
		LPCSTR shaderProfile,
		ID3D10Blob** shaderOutput); 

	static HRESULT CompileAndCreateEffect(
		WCHAR* srcFile,                                   // the name of the .fx file that contains the effect souce code we want to compile
		ID3DX11Effect* pEffect,                           // a pointer to the created effect
		ID3D11Device* pDevice);                           // pointer to the Direct3D 11 device
};
