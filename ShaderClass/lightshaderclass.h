////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Render/lightclass.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"


//////////////////////////////////
// Class name: LightShaderClass
//////////////////////////////////
class LightShaderClass final : public ShaderClass
{
public:
	LightShaderClass();
	~LightShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* deviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const pTextureArray,
		const DirectX::XMFLOAT3 & cameraPosition,
		LightClass* pLightSources);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	LightShaderClass(const LightShaderClass & obj);
	LightShaderClass & operator=(const LightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const texture,
		const DirectX::XMFLOAT3 & cameraPosition,
		LightClass* pLightSources);

	void RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_LightPS>  lightBuffer_;
	ConstantBuffer<ConstantCameraBuffer_LightVS> cameraBuffer_;
};