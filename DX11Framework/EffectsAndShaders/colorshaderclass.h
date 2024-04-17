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

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "shaderclass.h"

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
	struct ConstBuffPerObj
	{
		// a structure for data which is changed for each object
		DirectX::XMMATRIX worldViewProj;
		DirectX::XMFLOAT3 rgbColor;

		// a flag which defined either we use a color of vertex for painting
		// or we use RGB color from the const buffer (rgbColor variable)
		float             isUseVertexColor;   
	};


public:
	ColorShaderClass();
	~ColorShaderClass();

	// Public modification API
	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);


	// Public rendering API
	void RenderGeometry(ID3D11DeviceContext* pDeviceContext,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,
		const DirectX::XMMATRIX & viewProj,
		const UINT indexCount,
		const float totalGameTime);                            // time passed since the start of the application

	void RenderGeometry(ID3D11DeviceContext* pDeviceContext,
		ID3D11Buffer* vertexBufferPtr,
		ID3D11Buffer* indexBufferPtr,
		const std::vector<DirectX::XMFLOAT4> & colorsArr,
		const std::vector<DirectX::XMMATRIX> & worldMatrices,  // unique colour for each geometry obj
		const DirectX::XMMATRIX & viewProj,
		const float totalGameTime,                             // time passed since the start of the application
		const UINT vertexBufferStride,
		const UINT indexCount);

	// Public query API	
	const std::string & GetShaderName() const;

private:  // restrict a copying of this class instance
	ColorShaderClass(const ColorShaderClass & obj);
	ColorShaderClass & operator=(const ColorShaderClass & obj);

private:
	// compilation and setting up of shaders
	void InitializeShaders(ID3D11Device* device,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename, 
		const WCHAR* psFilename);

private:
	VertexShader   vertexShader_;
	PixelShader    pixelShader_;

	ConstantBuffer<ConstBuffPerObj> constBuffPerObj_;     // a constant buffer for data which is changed for each object

	const std::string className_{ "color_shader_class" };
};

