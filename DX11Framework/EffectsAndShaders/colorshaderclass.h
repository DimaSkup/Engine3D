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

//#include <d3dx10math.h>
//#include <d3dx11async.h>



#include "../Engine/macros.h"
#include "../Engine/Log.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass final
{
private:
	struct ConstantMatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProj;
	};

	struct ConstantColorBuffer_VS
	{
		DirectX::XMFLOAT4 rgbaColor;
	};


public:
	ColorShaderClass();
	~ColorShaderClass();

	bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext,
		ID3D11Buffer* vertexBufferPtr,
		ID3D11Buffer* indexBufferPtr,
		const UINT vertexBufferStride,
		const UINT indexCount,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMFLOAT4 & color);

	const std::string & GetShaderName() const;

private:  // restrict a copying of this class instance
	ColorShaderClass(const ColorShaderClass & obj);
	ColorShaderClass & operator=(const ColorShaderClass & obj);


private:
	// compilation and setting up of shaders
	void InitializeShaders(ID3D11Device* device,
		ID3D11DeviceContext* pDeviceContext,
		WCHAR* fxFilename);
		//const WCHAR* vsFilename, 
		//const WCHAR* psFilename);	
private:
	ID3DX11Effect* pFX_ = nullptr;
	ID3DX11EffectTechnique* pTech_ = nullptr;
	ID3D11InputLayout* pInputLayout_ = nullptr;
	ID3DX11EffectMatrixVariable* pfxWorldViewProj_ = nullptr;

	//VertexShader   vertexShader_;
	//PixelShader    pixelShader_;

	//ConstantBuffer<ConstantMatrixBufferType> matrixBuffer_;
	//ConstantBuffer<ConstantColorBuffer_VS>  colorBuffer_;

	const std::string className_{ "color_shader_class" };
};

