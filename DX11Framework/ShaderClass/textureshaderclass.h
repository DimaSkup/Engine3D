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
	TextureShaderClass();
	~TextureShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,            // it also can be baseViewMatrix for UI rendering
		const DirectX::XMMATRIX & projection,      // it also can be orthographic matrix for UI rendering
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

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
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection, 
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;
	
	ConstantBuffer<ConstantMatrixBuffer_VS>       matrixConstBuffer_;
};
