////////////////////////////////////////////////////////////////////
// Filename:     VertexShader.h
// Description:  this is a class for handling all the vertex shader stuff
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


class VertexShader
{
public:
	~VertexShader();

	bool Initialize(
		ID3D11Device* pDevice,
		const std::wstring & shaderPath,
		const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
		const UINT layoutElemNum);

	void Shutdown();

	// public query API
	inline ID3D11VertexShader* GetShader()      { return pShader_; };
	inline ID3DBlob*           GetBuffer()      { return pShaderBuffer_; };
	inline ID3D11InputLayout*  GetInputLayout() { return pInputLayout_; };

private:
	ID3D11VertexShader* pShader_ = nullptr;
	ID3DBlob*           pShaderBuffer_ = nullptr;
	ID3D11InputLayout*  pInputLayout_ = nullptr;
};

};  // namespace Render