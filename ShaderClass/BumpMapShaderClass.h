////////////////////////////////////////////////////////////////////
// Filename:     BumpMapShaderClass.h
// Description:  a class for initialization of bump map texture shaders
//               and work with it;
//
// Created:      18.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"

#include "../Render/lightclass.h"


//////////////////////////////////
// Class name: BumpMapShaderClass
//////////////////////////////////
class BumpMapShaderClass final : public ShaderClass
{
public:
	BumpMapShaderClass();
	~BumpMapShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray,
		LightClass* pLightSources);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;

private:  // restrict a copying of this class instance
	BumpMapShaderClass(const BumpMapShaderClass & obj);
	BumpMapShaderClass & operator=(const BumpMapShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray,
		LightClass* pLightSources);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS>       matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_BumpMapPS> lightBuffer_;
};