////////////////////////////////////////////////////////////////////
// Filename:     SkyPlaneShaderClass.h
// Description:  this is the shader used for rendering the clouds
//               on the sky plane
// Created:      27.06.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <DirectXMath.h>
#include <map>

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"     // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"

//////////////////////////////////
// Class name: SkyPlaneShaderClass
//////////////////////////////////
class SkyPlaneShaderClass final
{
public:
	SkyPlaneShaderClass();
	~SkyPlaneShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
		const float firstTranslationX,
		const float firstTranslationZ,
		const float secondTranslationX,
		const float secondTranslationZ,
		const float brightness);

	const std::string & GetShaderName() const;

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
		float firstTranslationX,
		float firstTranslationZ,
		float secondTranslationX,
		float secondTranslationZ,
		float brightness);

	void RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantSkyPlaneBuffer_PS>    skyBuffer_;

	const std::string className_{ "sky_plane_shader_class" };
};