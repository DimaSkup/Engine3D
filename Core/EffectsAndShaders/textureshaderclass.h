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
#include <vector>
#include <map>
#include <assimp/material.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: TextureShaderClass
//////////////////////////////////
class TextureShaderClass final
{
private:
	struct AddressesOfMembers
	{
		ID3D11SamplerState* const* ppSamplerState = nullptr;
		ID3D11VertexShader*  pVertexShader = nullptr;
		ID3D11PixelShader*   pPixelShader = nullptr;
		ID3D11InputLayout*   pVertexShaderInputLayout = nullptr;

		ID3D11Buffer* const* constBuffPerObjAddr = nullptr;
		ID3D11Buffer* const* constBuffPerFrameAddr = nullptr;
		ID3D11Buffer* const* constBuffRareChangedAddr = nullptr;
	};

public:

	struct Basic32
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tex;
	};

	struct InstancedData
	{
		DirectX::XMFLOAT4X4 world;
	};


	struct ConstBufferPerObj
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProj;
		DirectX::XMMATRIX texTransform;
	};

	struct ConstBufferPerFrame
	{
		DirectX::XMFLOAT3 cameraPos;
		float padding = 0.0f;              // we need the padding because the size of this struct must be a multiple of 16
	};

	// params for controlling the rendering process
	// in the pixel shader
	struct ConstBufferRareChanged
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

	TextureShaderClass(const TextureShaderClass& obj) = delete;
	TextureShaderClass& operator=(const TextureShaderClass& obj) = delete;


	// Public modification API
	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	// Public rendering API
	void PrepareShaderForRendering(
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMFLOAT3& cameraPosition,
		ID3D11Buffer* pMeshVB,
		ID3D11Buffer* pMeshIB,
		D3D11_PRIMITIVE_TOPOLOGY topologyType);

	void Render(
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,
		const DirectX::XMMATRIX & viewProj,
		const std::vector<DirectX::XMMATRIX>& texTransforms,
		const std::vector<ID3D11ShaderResourceView* const*>& textures,
		const UINT indexCount);


	// Public API for controlling of shader rendering state
	void SwitchFog(ID3D11DeviceContext* pDeviceContext);
	void SwitchAlphaClipping(ID3D11DeviceContext* pDeviceContext);
	void SetForParams(ID3D11DeviceContext* pDeviceContext, const DirectX::XMFLOAT3 & fogColor, const float fogStart, const float fogRange);
	
	// Public query API
	inline const std::string& GetShaderName() const { return className_; }	

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void BuildInstancedBuffer(ID3D11Device* pDevice);

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	ID3D11Buffer* pInstancedBuffer_ = nullptr;
	std::vector<InstancedData> instancedData_;
	
	ConstantBuffer<ConstBufferPerObj>       constBuffPerObj_;
	ConstantBuffer<ConstBufferPerFrame>     constBuffPerFrame_;
	ConstantBuffer<ConstBufferRareChanged>  constBuffRareChanged_;

	AddressesOfMembers addresses_;

	const std::string className_{ "texture_shader" };
};
