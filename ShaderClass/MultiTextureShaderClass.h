////////////////////////////////////////////////////////////////////
// Filename:     MultiTextureShaderClass.h
// Description:  a class for initialization of multi texture shaders
//               and work with it;
// Created:      09.01.23
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
#include "ConstantBufferTypes.h"
#include "SamplerState.h"
#include "../Engine/log.h"

using namespace std;


/////////////////////////////////////////
// Class name: MultiTextureShaderClass
/////////////////////////////////////////
class MultiTextureShaderClass final : public ShaderClass
{
public:
	MultiTextureShaderClass();
	~MultiTextureShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	MultiTextureShaderClass(const MultiTextureShaderClass & obj);
	MultiTextureShaderClass & operator=(const MultiTextureShaderClass & obj);

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
		ID3D11ShaderResourceView* const textureArray);

	void RenderShaders(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixConstBuffer_;
	SamplerState samplerState_;
};
