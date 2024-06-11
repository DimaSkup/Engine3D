////////////////////////////////////////////////////////////////////
// Filename:     SkyDomeShaderClass.h
// Description:  a class for initialization of sky dome shaders
//               and work with it;
// Created:      16.04.23
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
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "SamplerState.h"

#include "../Engine/log.h"


/////////////////////////////////////////
// Class name: SkyDomeShaderClass
/////////////////////////////////////////
class SkyDomeShaderClass
{
private:
	struct ConstantSkyDomeColorBufferType_PS
	{
		DirectX::XMFLOAT4 apexColor{};
		DirectX::XMFLOAT4 centerColor{};
	};

public:
	SkyDomeShaderClass();
	~SkyDomeShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & worldMatrix,
		const DirectX::XMMATRIX & viewMatrix,
		const DirectX::XMMATRIX & projectionMatrix,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,  // contains pairs ['texture_type' => 'texture_resource_view'] for the sky dome
		const DirectX::XMFLOAT4 & apexColor,            // the color of the sky dome's top
		const DirectX::XMFLOAT4 & centerColor);         // the color of the sky dome's horizon

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	SkyDomeShaderClass(const SkyDomeShaderClass & obj);
	SkyDomeShaderClass & operator=(const SkyDomeShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext, 
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view, 
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,  // contains pairs ['texture_type' => 'texture_resource_view'] for the sky dome
		const DirectX::XMFLOAT4 & apexColor,
		const DirectX::XMFLOAT4 & centerColor);

	void RenderShaders(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS> matrixConstBuffer_;
	ConstantBuffer<ConstantSkyDomeColorBufferType_PS> colorConstBuffer_;

	const std::string className_{ "sky_dome_shader" };
};
