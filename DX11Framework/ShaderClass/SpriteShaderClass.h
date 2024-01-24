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
#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"

//////////////////////////////////
// Class name: SpriteShaderClass
//////////////////////////////////
class SpriteShaderClass : public ShaderClass
{
public:
	SpriteShaderClass();
	~SpriteShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd) override;

	// a rendering function which takes parameters for HLSL shaders
	// from the common data_for_shaders_container;
	bool SpriteShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader);

	// a rendering function for direct passing params for HLSL shaders
	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & WVO,   // world * base_view * ortho matrix
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	SpriteShaderClass(const SpriteShaderClass & obj);
	SpriteShaderClass & operator=(const SpriteShaderClass & obj);


private:
	// initialize a vertex/pixel shader, sampler state and constant buffers
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	// when we setup the data for the shaders we check if we have textures 
	// with such keys (types)
	const std::vector<std::string> textureKeys_{ "diffuse" };

	std::unique_ptr<VertexShader> pVertexShader_;
	std::unique_ptr<PixelShader>  pPixelShader_;
	std::unique_ptr<SamplerState> pSamplerState_;
	std::unique_ptr<ConstantBuffer<ConstantMatrixBuffer_VS_WVP>> pMatrixBuffer_;
};