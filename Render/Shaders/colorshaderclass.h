/////////////////////////////////////////////////////////////////////
// Filename:       colorshaderclass.h
// Description:    We use this class to invoke HLSL shaders 
//                 for drawing our 3D models which are on the GPU
//
// Revising:       06.04.22
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"

//#include "../Common/Types.h"


namespace Render
{

//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass final
{
private:
	struct ConstBuffPerFrame
	{
		DirectX::XMMATRIX viewProj;
	};

	struct InstancedData
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 color;
	};


public:
	ColorShaderClass();
	~ColorShaderClass();

	// restrict a copying of this class instance
	ColorShaderClass(const ColorShaderClass& obj) = delete;
	ColorShaderClass& operator=(const ColorShaderClass& obj) = delete;

	// Public modification API
	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	void Prepare(
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX& viewProj,
		const float totalGameTime);

	// Public rendering API
	void Render(
		ID3D11DeviceContext* pDeviceContext,
		ID3D11Buffer* pMeshVB,
		ID3D11Buffer* pMeshIB,
		const uint32_t vertexSize,
		const uint32_t indexCount);

	// Public query API	
	inline const std::string& GetShaderName() const { return className_; }


private:
	void InitializeShaders(
		ID3D11Device* device,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

	void BuildInstancedBuffer(ID3D11Device* pDevice);

private:
	VertexShader   vs_;
	PixelShader    pixelShader_;

	ID3D11Buffer* pInstancedBuffer_ = nullptr;
	std::vector<InstancedData> instancedData_;

	ConstantBuffer<ConstBuffPerFrame> constBufferPerFrame_;

	const std::string className_{ "color_shader_class" };
};

} // namespace Render