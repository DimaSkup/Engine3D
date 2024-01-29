////////////////////////////////////////////////////////////////////
// Filename:     textureshaderclass.h
// Description:  this class will be used to draw the 3D models
//               using the texture vertex and pixel shaders.
//               It renders only a texture;
//
// Revising:     09.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TextureShaderClass : public ShaderClass
{
public:
	struct ConstantMatrixBuffer_VS
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProj;
	};

	// params for controlling the rendering process
	// in the pixel shader
	struct ConstantBufferPerFrame_PS
	{
		float fogEnabled;
		float useAlphaClip;
		float fogStart;              // how far from us the fog starts
		float fogRange;          // (1 / range) inversed distance from the fog start position where the fog completely hides the surface point

		DirectX::XMFLOAT3 fogColor;  // the colour of the fog (usually it's a degree of grey)
		
	};

public:
	TextureShaderClass();
	~TextureShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd) override;

	// a Render function for virtual/polymorph using
	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader) override;

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	TextureShaderClass(const TextureShaderClass & obj);
	TextureShaderClass & operator=(const TextureShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader);
#if 0
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection, 
		const DirectX::XMFLOAT3 & cameraPosition,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
		const DirectX::XMFLOAT4 & fogColor,
		const float fogStart,
		const float fogRange,
		const bool  fogEnabled,
		const bool  useAlphaClip);
#endif

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;
	
	ConstantBuffer<ConstantMatrixBuffer_VS>       matrixConstBuffer_;
	ConstantBuffer<ConstantCameraBufferType>      cameraBuffer_;
	ConstantBuffer<ConstantBufferPerFrame_PS>     bufferPerFrame_;
};
