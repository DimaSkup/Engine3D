////////////////////////////////////////////////////////////////////
// Filename:     SpecularLightShaderClass.h
// Description:  this class is needed for rendering 3D models, 
//               its texture, SPECULAR light on it using HLSL shaders.
// Revising:     10.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "../Render/lightclass.h"

//#include <d3dcompiler.h>

//////////////////////////////////
// Class name: SpecularLightShaderClass
//////////////////////////////////
class SpecularLightShaderClass : public ShaderClass
{
public:
	SpecularLightShaderClass(void);
	SpecularLightShaderClass(const SpecularLightShaderClass& anotherObj);
	~SpecularLightShaderClass(void);

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		ID3D11ShaderResourceView* const* textureArray,
		DataContainerForShadersClass* pDataForShader) override;

	virtual const std::string & GetShaderName() const _NOEXCEPT override;

private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		        const DirectX::XMMATRIX & world,
		        const DirectX::XMMATRIX & view,
		        const DirectX::XMMATRIX & projection,
		        ID3D11ShaderResourceView* texture,
				const DirectX::XMFLOAT3 & cameraPosition,
		        const DirectX::XMFLOAT4 & diffuseColor, 
				const DirectX::XMFLOAT3 & lightDirection, 
				const DirectX::XMFLOAT4 & ambientColor,
				const DirectX::XMFLOAT4 & specularColor, 
				float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	

private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_LightPS>  lightBuffer_;
	ConstantBuffer<ConstantCameraBuffer_LightVS> cameraBuffer_;
};