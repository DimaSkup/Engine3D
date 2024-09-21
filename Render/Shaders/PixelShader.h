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
#include <string>


namespace Render
{
	

class PixelShader
{
public:
	PixelShader();
	~PixelShader();

	bool Initialize(
		ID3D11Device* pDevice,
		const std::wstring & shaderPath,
		const std::string& funcName = "PS");

	void Shutdown();

	inline ID3D11PixelShader* GetShader() { return pShader_; };
	inline ID3DBlob* GetBuffer() { return pShaderBuffer_; };

private:
	ID3D11PixelShader* pShader_ = nullptr;
	ID3DBlob*          pShaderBuffer_ = nullptr;
};

} // namespace Render