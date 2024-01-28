////////////////////////////////////////////////////////////////////
// Filename:     PixelShader.h
// Description:  this is a class for handling all the pixel shader stuff
//
// Revising:     05.11.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <fstream>		

#include "shaderclass.h"
#include "../Engine/Log.h"
#include "../Engine/macros.h"

// a pixel shader container and the related stuff
class PixelShader
{
public:
	~PixelShader() {
		Log::Debug(LOG_MACRO);
		_RELEASE(pShader);
		_RELEASE(pShaderBuffer);
	}

	bool Initialize(ID3D11Device* pDevice,
					std::wstring shaderPath);

	ID3D11PixelShader* GetShader();
	ID3DBlob* GetBuffer();

private:
	ID3D11PixelShader* pShader = nullptr;
	ID3DBlob* pShaderBuffer = nullptr;
};