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
#include "ConstantBufferTypes.h"
#include "SamplerState.h"

#include "../Render/LightStore.h"

#include <map>


//////////////////////////////////
// Class name: BumpMapShaderClass
//////////////////////////////////
class BumpMapShaderClass final
{
public:
	// a light constant buffer structur for the bump map pixel shader
	struct ConstantLightBuffer_BumpMapPS
	{
		DirectX::XMFLOAT3 diffuseColor;
		float padding_1;
		DirectX::XMFLOAT3 lightDirection;
		float padding_2;
		DirectX::XMFLOAT3 ambientColor;
		float padding_3;
	};

public:
	BumpMapShaderClass();
	~BumpMapShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
		const LightStore & lightStore);

	const std::string & GetShaderName() const;

private:  // restrict a copying of this class instance
	BumpMapShaderClass(const BumpMapShaderClass & obj);
	BumpMapShaderClass & operator=(const BumpMapShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
		const LightStore & lightStore);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS>       matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_BumpMapPS> lightBuffer_;

	const std::string className_{ "bump_map_shader" };
};