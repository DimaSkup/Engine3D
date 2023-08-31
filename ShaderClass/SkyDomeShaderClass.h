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
#include <fstream>

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"
#include "../Engine/log.h"

using namespace std;

/////////////////////////////////////////
// Class name: SkyDomeShaderClass
/////////////////////////////////////////
class SkyDomeShaderClass : public ShaderClass
{
private:
	struct ConstantColorBufferType_PS
	{
		DirectX::XMFLOAT4 apexColor{};
		DirectX::XMFLOAT4 centerColor{};
	};

public:
	SkyDomeShaderClass();
	~SkyDomeShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & worldMatrix,
		const DirectX::XMMATRIX & viewMatrix,
		const DirectX::XMMATRIX & projectionMatrix,
		ID3D11ShaderResourceView* const pTextureArray,
		const DirectX::XMFLOAT4 & apexColor,            // the color of the sky dome's top
		const DirectX::XMFLOAT4 & centerColor);         // the color of the sky dome's horizon

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	SkyDomeShaderClass(const SkyDomeShaderClass & obj);
	SkyDomeShaderClass & operator=(const SkyDomeShaderClass & obj);

private:
	bool InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd, 
		WCHAR* vsFilename, 
		WCHAR* psFilename);

	bool SetShadersParameters(ID3D11DeviceContext* pDeviceContext, 
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view, 
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray,
		const DirectX::XMFLOAT4 & apexColor,
		const DirectX::XMFLOAT4 & centerColor);

	void RenderShaders(ID3D11DeviceContext* pDeviceContext, const int indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixConstBuffer_;
	ConstantBuffer<ConstantColorBufferType_PS> colorConstBuffer_;
	SamplerState samplerState_;
};
