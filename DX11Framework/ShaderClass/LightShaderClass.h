////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
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
#include "../Render/lightclass.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"   // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: LightShaderClass
//////////////////////////////////
class LightShaderClass final : public ShaderClass
{
public:
	struct ConstantMatrixBuffer_LightVS
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProj;
	};

	// a constant light buffer structure for the light pixel shader
	struct ConstantLightBuffer_LightPS
	{
		DirectX::XMFLOAT3 ambientColor;         // a common light of the scene
		float             ambientLightStrength; // the power of ambient light
		DirectX::XMFLOAT3 diffuseColor;         // color of the main directed light
		float             padding_1;
		DirectX::XMFLOAT3 lightDirection;       // a direction of the diffuse light
		float             padding_2;
	};

	struct ConstantBufferPerFrame_LightPS
	{
		DirectX::XMFLOAT3 fogColor;  // the colour of the fog (usually it's a degree of grey)
		float fogStart;              // how far from us the fog starts
		float fogRange_inv;          // (1 / range) inversed distance from the fog start position where the fog completely hides the surface point
		bool  fogEnabled;
		bool debugNormals;
	};


public:
	LightShaderClass();
	~LightShaderClass();

	// initialize the shader class object
	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	// we call this rendering function from the model_to_shader mediator
	virtual bool Render(ID3D11DeviceContext* pDeviceContext, 
		DataContainerForShaders* pDataForShader) override;

	virtual const std::string & GetShaderName() const _NOEXCEPT override;

	int kek() { return 10; }

private:  // restrict a copying of this class instance
	LightShaderClass(const LightShaderClass & obj);
	LightShaderClass & operator=(const LightShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	// setup shader parameters before rendering
	void SetShaderParameters(ID3D11DeviceContext* deviceContext,
		DataContainerForShaders* pDataForShaders);
#if 0
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
		const DirectX::XMFLOAT3 & cameraPosition,
		const std::vector<LightClass*> & diffuseLightsArr);
#endif

	// render a model using HLSL shaders
	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);


private:
	// classes for work with the vertex, pixel shaders and the sampler state
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	// constant buffers
	ConstantBuffer<ConstantMatrixBuffer_LightVS>   matrixBuffer_;
	ConstantBuffer<ConstantLightBuffer_LightPS>    lightBuffer_;
	ConstantBuffer<ConstantCameraBufferType>       cameraBuffer_;
	ConstantBuffer<ConstantBufferPerFrame_LightPS> bufferPerFrame_;
};