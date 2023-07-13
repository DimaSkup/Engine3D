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

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) override;

	virtual bool Render(ID3D11DeviceContext* pDevCon,
		const int indexCount,
		const DirectX::XMMATRIX & world,
		ID3D11ShaderResourceView* const* textureArray,
		DataContainerForShadersClass* pDataForShader) override;


	virtual const std::string & GetShaderName() const _NOEXCEPT override;


	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:  // restrict a copying of this class instance
	ColorShaderClass(const ColorShaderClass & obj);
	ColorShaderClass & operator=(const ColorShaderClass & obj);


private:
	// compilation and setting up of shaders
	bool InitializeShaders(ID3D11Device* device, 
		                   ID3D11DeviceContext* pDeviceContext,
		                   HWND hwnd,
		                   WCHAR* vsFilename, 
		                   WCHAR* psFilename);	

	bool SetShaderParameters(ID3D11DeviceContext*, 
		                     DirectX::XMMATRIX world, 
		                     DirectX::XMMATRIX view, 
		                     DirectX::XMMATRIX projection);	// here we setup the constant shader buffer
	void RenderShader(ID3D11DeviceContext*, int);

private:
	VertexShader   vertexShader_;
	PixelShader    pixelShader_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;
};

