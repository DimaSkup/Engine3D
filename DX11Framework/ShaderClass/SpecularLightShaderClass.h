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
#include "ConstantBufferTypes.h"
#include "../Render/lightclass.h"

//#include <d3dcompiler.h>

//////////////////////////////////
// Class name: SpecularLightShaderClass
//////////////////////////////////
class SpecularLightShaderClass : public ShaderClass
{
public:
	// a constant light buffer structure for the light pixel shader
	struct ConstantLightBuffer_SpecularLightPS
	{
		DirectX::XMFLOAT3 specularColor;      // the color of specular light
		float             specularPower;      // the intensity of specular light
		DirectX::XMFLOAT3 ambientColor;       // a common light of the scene
		float             ambientStrength;    // intensity of the ambient light
		DirectX::XMFLOAT3 diffuseColor;       // color of the main directed light
		DirectX::XMFLOAT3 lightDirection;     // a direction of the diffuse light
	};


public:
	SpecularLightShaderClass();
	~SpecularLightShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		                DataContainerForShaders* pDataForShader) override;

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	SpecularLightShaderClass(const SpecularLightShaderClass & obj);
	SpecularLightShaderClass & operator=(const SpecularLightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, 
		                     const DataContainerForShaders* pDataForShader);

	void RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount);
	

private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>              matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_SpecularLightPS>  lightBuffer_;
	ConstantBuffer<ConstantCameraBufferType>             cameraBuffer_;
};