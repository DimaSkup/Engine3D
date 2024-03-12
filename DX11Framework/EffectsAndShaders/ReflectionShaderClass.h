////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     ReflectionShaderClass.h
// Description:  this class will be used for rendering a basic
//               planar reflections in DirectX 11 using HLSL.
//
// Revising:     12.01.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <map>

#include "../Engine/macros.h"
#include "../Engine/Log.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: ReflectionShaderClass
//////////////////////////////////
class ReflectionShaderClass final
{
private:
	// this is the structure for the reflection view matrix dynamic constant buffer
	struct ReflectionBufferType_VS
	{
		DirectX::XMMATRIX reflectionMatrix;
	};

public:
	ReflectionShaderClass();
	~ReflectionShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,            
		const DirectX::XMMATRIX & projection,      
		const DirectX::XMMATRIX & reflectionMatrix,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

	const std::string & GetShaderName() const;


private:  // restrict a copying of this class instance
	ReflectionShaderClass(const ReflectionShaderClass & obj);
	ReflectionShaderClass & operator=(const ReflectionShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const DirectX::XMMATRIX & reflectionMatrix,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader        vertexShader_;
	PixelShader         pixelShader_;
	SamplerState        samplerState_;

	ConstantBuffer<ConstantMatrixBuffer_VS>  matrixConstBuffer_;
	ConstantBuffer<ReflectionBufferType_VS>  reflectionConstBuffer_;

	const std::string className_{ "reflection_shader_class" };
};
