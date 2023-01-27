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

#include "../Render/lightclass.h"
#include "../Model/modelclass.h"




//////////////////////////////////
// Class name: LightShaderClass
//////////////////////////////////
class CombinedShaderClass : public ShaderClass
{
public:
	CombinedShaderClass(void);
	CombinedShaderClass(const CombinedShaderClass& anotherObj);
	~CombinedShaderClass(void);

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd);

	bool Render(ID3D11DeviceContext* deviceContext,
		ModelClass* pModelToRender,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const DirectX::XMFLOAT3 & cameraPosition,
		const LightClass* pLight);


private:
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND, WCHAR* vsFilename, WCHAR* psFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & modelWorld,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		ID3D11ShaderResourceView** textureArray,
		const DirectX::XMFLOAT3 & cameraPosition,
		const LightClass* pLight);

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