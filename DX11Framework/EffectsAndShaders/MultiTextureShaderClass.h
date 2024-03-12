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
#include <map>

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "SamplerState.h"

#include "../Engine/log.h"


/////////////////////////////////////////
// Class name: MultiTextureShaderClass
/////////////////////////////////////////
class MultiTextureShaderClass final
{
public:
	MultiTextureShaderClass();
	~MultiTextureShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	MultiTextureShaderClass(const MultiTextureShaderClass & obj);
	MultiTextureShaderClass & operator=(const MultiTextureShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext);

	void RenderShaders(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS> matrixConstBuffer_;
	
	const std::string className_{ "multi_texture_shader" };
};
