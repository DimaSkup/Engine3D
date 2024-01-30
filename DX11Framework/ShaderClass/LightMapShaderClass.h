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
class LightMapShaderClass final : public ShaderClass
{
public:
	LightMapShaderClass();
	~LightMapShaderClass();


	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader) override;
	

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	LightMapShaderClass(const LightMapShaderClass & obj);
	LightMapShaderClass & operator=(const LightMapShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DataContainerForShaders* pDataForShader);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;
};