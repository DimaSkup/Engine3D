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
#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBufferInterface.h"
#include "ConstantBufferTypes.h"
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

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		                DataContainerForShaders* pDataForShader) override;

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
		                      const DataContainerForShaders* pDataForShader);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader* pVertexShader_ = nullptr;
	PixelShader*  pPixelShader_ = nullptr;
	SamplerState* pSamplerState_ = nullptr;

	//ConstantBufferInterface<ConstantMatrixBuffer_VS>* pMatrixBuffer_ = nullptr;
	ConstantBuffer<ConstantMatrixBuffer_VS>* pMatrixBuffer_ = nullptr;
};