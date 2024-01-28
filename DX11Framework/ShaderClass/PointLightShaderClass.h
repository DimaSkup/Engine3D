////////////////////////////////////////////////////////////////////
// Filename:     PointLightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with multiple POINT LIGHTS on it using HLSL shaders.
//
// Created:      28.08.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// GLOBALS
//////////////////////////////////
const int NUM_POINT_LIGHTS = 4;


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
#include "ConstantBufferTypes.h"

//////////////////////////////////
// Class name: PointLightShaderClass
//////////////////////////////////
class PointLightShaderClass final : public ShaderClass
{
private:
	// there are two structures for the diffuse colour and light position arrays
	// that are used in the vertex and pixel shader
	struct LightColorBufferType
	{
		DirectX::XMFLOAT4 diffuseColor[NUM_POINT_LIGHTS];
	};

	struct LightPositionBufferType
	{
		DirectX::XMFLOAT4 lightPosition[NUM_POINT_LIGHTS];
	};

public:
	PointLightShaderClass();
	~PointLightShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const* pTextureArray,
		const DirectX::XMFLOAT4* pPointLightsColors,
		const DirectX::XMFLOAT4* pPointLightsPositions);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	PointLightShaderClass(const PointLightShaderClass & obj);
	PointLightShaderClass & operator=(const PointLightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const* pTextureArray,
		const DirectX::XMFLOAT4* pPointLightColor,
		const DirectX::XMFLOAT4* pPointLightPosition);

	void RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>  matrixBuffer_;
	ConstantBuffer<LightColorBufferType>     lightColorBuffer_;
	ConstantBuffer<LightPositionBufferType>  lightPositionBuffer_;
	//ConstantBuffer<ConstantLightBuffer_LightPS>  lightBuffer_;
	//ConstantBuffer<ConstantCameraBufferType> cameraBuffer_;
};