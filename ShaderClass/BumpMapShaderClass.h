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


//////////////////////////////////
// Class name: BumpMapShaderClass
//////////////////////////////////
class BumpMapShaderClass final : public ShaderClass
{
public:
	BumpMapShaderClass();
	~BumpMapShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext, 
						const int indexCount,
				        const DirectX::XMMATRIX & world,               // model world matrix
				        ID3D11ShaderResourceView* const* textureArray,
				        DataContainerForShadersClass* pDataForShader) override;

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	BumpMapShaderClass(const BumpMapShaderClass & obj);
	BumpMapShaderClass & operator=(const BumpMapShaderClass & obj);

private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
							HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
							  const DirectX::XMMATRIX& world,
							  const DirectX::XMMATRIX& view,
							  const DirectX::XMMATRIX& projection,
							  ID3D11ShaderResourceView* const* textureArray,
							  const DirectX::XMFLOAT3 & lightDirection,
							  const DirectX::XMFLOAT4 & diffuseColor);
	void RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS>       matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_BumpMapPS> lightBuffer_;
};