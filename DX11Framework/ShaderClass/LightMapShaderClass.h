////////////////////////////////////////////////////////////////////
// Filename:     LightMapShaderClass.h
// Description:  a class for initialization of light map texture shaders
//               and work with it;
//
// Created:      13.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "SamplerState.h"


//////////////////////////////////
// Class name: LightMapShaderClass
//////////////////////////////////
class LightMapShaderClass final
{
public:
	LightMapShaderClass();
	~LightMapShaderClass();


	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	bool Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);
	
	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	LightMapShaderClass(const LightMapShaderClass & obj);
	LightMapShaderClass & operator=(const LightMapShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;

	const std::string className_{ "light_map_shader" };
};