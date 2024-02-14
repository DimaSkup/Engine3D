////////////////////////////////////////////////////////////////////
// Filename:     AlphaMapShaderClass.h
// Description:  a shader class for initialization of alpha map texture shaders
//               and work with it;
//
// Created:      15.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"


//////////////////////////////////
// Class name: AlphaMapShaderClass
//////////////////////////////////
class AlphaMapShaderClass final
{
public:
	AlphaMapShaderClass();
	~AlphaMapShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	bool Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	AlphaMapShaderClass(const AlphaMapShaderClass & obj);
	AlphaMapShaderClass & operator=(const AlphaMapShaderClass & obj);


private:
	// initialize a vertex/pixel shader, sampler state and constant buffers
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;

	const std::string className_{ "alpha_map_shader" };
};