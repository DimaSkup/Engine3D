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

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "shaderclass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: ReflectionShaderClass
//////////////////////////////////
class ReflectionShaderClass : public ShaderClass
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

	virtual bool Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataForShader) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,            
		const DirectX::XMMATRIX & projection,      
		const DirectX::XMMATRIX & reflectionMatrix,
		const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;


private:  // restrict a copying of this class instance
	ReflectionShaderClass(const ReflectionShaderClass & obj);
	ReflectionShaderClass & operator=(const ReflectionShaderClass & obj);

private:
	void InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
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
};
