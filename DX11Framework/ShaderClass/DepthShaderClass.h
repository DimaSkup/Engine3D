/////////////////////////////////////////////////////////////////////
// Filename:       DepthShaderClass.h
// Description:    coloring object according to its depth position
//
// Revising:       10.06.23
/////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>


#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"


//////////////////////////////////
// Class name: DepthShaderClass
//////////////////////////////////
class DepthShaderClass : public ShaderClass
{
public:
	DepthShaderClass();
	~DepthShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		HWND hwnd) override;

	bool Render(ID3D11DeviceContext* pDeviceContext,
		const UINT indexCount,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection);


	virtual const std::string & GetShaderName() const _NOEXCEPT override;

private:  // restrict a copying of this class instance
	DepthShaderClass(const DepthShaderClass & obj);
	DepthShaderClass & operator=(const DepthShaderClass & obj);


private:
	// initialization and setting up of shaders
	void InitializeShaders(ID3D11Device* device,
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & world,
		const DirectX::XMMATRIX & view,
		const DirectX::XMMATRIX & projection);	// here we setup the constant shader buffer

	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader   vertexShader_;
	PixelShader    pixelShader_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;
};
