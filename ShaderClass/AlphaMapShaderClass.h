////////////////////////////////////////////////////////////////////
// Filename:     AlphaMapShaderClass.h
// Description:  a class for initialization of alpha map texture shaders
//               and work with it;
//
// Created:      15.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "shaderclass.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"


//////////////////////////////////
// Class name: AlphaMapShaderClass
//////////////////////////////////
class AlphaMapShaderClass : public ShaderClass
{
public:
	AlphaMapShaderClass();
	~AlphaMapShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	AlphaMapShaderClass(const AlphaMapShaderClass & obj);
	AlphaMapShaderClass & operator=(const AlphaMapShaderClass & obj);


private:
	// initialize a vertex/pixel shader, sampler state and constant buffers
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd, 
		WCHAR* vsFilename, 
		WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& projection,
		ID3D11ShaderResourceView* const textureArray);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;
};