////////////////////////////////////////////////////////////////////
// Filename:     SpriteShaderClass.h
// Description:  a shader class for initialization of HLSL shaders 
//               which will be used for rendering 2D sprites onto the screen;
//
// Created:      28.12.23
////////////////////////////////////////////////////////////////////
#pragma once



//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"

//////////////////////////////////
// Class name: SpriteShaderClass
//////////////////////////////////
class SpriteShaderClass final
{
public:
	SpriteShaderClass();
	~SpriteShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	// a rendering function which takes parameters for HLSL shaders
	// from the common data_for_shaders_container;
	bool Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	SpriteShaderClass(const SpriteShaderClass & obj);
	SpriteShaderClass & operator=(const SpriteShaderClass & obj);


private:
	// initialize a vertex/pixel shader, sampler state and constant buffers
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader vertexShader_;
	PixelShader  pixelShader_;
	SamplerState samplerState_;
	ConstantBuffer<ConstantMatrixBuffer_VS_WVP> matrixBuffer_;

	const std::string className_{ "sprite_shader_class" };
};