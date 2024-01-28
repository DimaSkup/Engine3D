////////////////////////////////////////////////////////////////////
// Filename:     CombinedShaderClass.h
// Description:  this class is needed for rendering 3D models, 
//               its textures, light sources on it using HLSL shaders.
// Revising:     27.01.22
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




//////////////////////////////////
// Class name: SpecularLightShaderClass
//////////////////////////////////
class CombinedShaderClass : public ShaderClass
{
public:
	// a constant light buffer structure for the light data which
	// is passed into the pixel shader
	struct ConstantLightBuffer_LightPS
	{
		DirectX::XMFLOAT3 lightDirection;     // a direction of the diffuse light
		float             specularPower;      // the intensity of specular light
		DirectX::XMFLOAT3 specularColor;      // the color of specular light
		DirectX::XMFLOAT3 ambientColor;       // a common light of the scene
		DirectX::XMFLOAT3 diffuseColor;       // color of the main directed light
	};

public:
	CombinedShaderClass(void);

	~CombinedShaderClass(void);

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* deviceContext, 
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray,
		const DirectX::XMFLOAT3 & cameraPosition,
		const LightClass* pLightSource);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	CombinedShaderClass(const CombinedShaderClass & obj);
	CombinedShaderClass & operator=(const CombinedShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd, 
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & modelWorld,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView* const textureArray,
		const DirectX::XMFLOAT3 & cameraPosition,
		const LightClass* pLightSource);

	void RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_VS>      matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_LightPS>  lightBuffer_;
	ConstantBuffer<ConstantCameraBufferType> cameraBuffer_;

	//std::unique_ptr<ConstantBuffer<ConstantMatrixBuffer_VS>>       pMatrixBuffer_;
	//std::unique_ptr<ConstantBuffer<ConstantLightBuffer_LightPS>>   pLightBuffer_;
	//std::unique_ptr<ConstantBuffer<ConstantCameraBufferType>>  pCameraBuffer_;
};