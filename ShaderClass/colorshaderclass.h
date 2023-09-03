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
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
//#include <d3dcompiler.h>
#include <DirectXMath.h>


#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"


//////////////////////////////////
// Class name: ColorShaderClass
//////////////////////////////////
class ColorShaderClass : public ShaderClass
{
public:
	ColorShaderClass();
	~ColorShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const DirectX::XMFLOAT4 & color);

	virtual const std::string & GetShaderName() const _NOEXCEPT override;

private:  // restrict a copying of this class instance
	ColorShaderClass(const ColorShaderClass & obj);
	ColorShaderClass & operator=(const ColorShaderClass & obj);


private:
	// compilation and setting up of shaders
	void InitializeShaders(ID3D11Device* device, 
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);	

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection,
		const DirectX::XMFLOAT4 & color);

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader   vertexShader_;
	PixelShader    pixelShader_;

	std::unique_ptr<ConstantBuffer<ConstantMatrixBuffer_VS>>  pMatrixBuffer_;
	std::unique_ptr<ConstantBuffer<ConstantColorBuffer_VS>>   pColorBuffer_;
};

