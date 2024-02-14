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
class DepthShaderClass
{
public:
	DepthShaderClass();
	~DepthShaderClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

	const std::string & GetShaderName() const;

private:  // restrict a copying of this class instance
	DepthShaderClass(const DepthShaderClass & obj);
	DepthShaderClass & operator=(const DepthShaderClass & obj);


private:
	// initialization and setting up of shaders
	void InitializeShaders(ID3D11Device* device,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void SetShaderParameters(ID3D11DeviceContext* pDeviceContext);	
	void RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount);

private:
	VertexShader   vertexShader_;
	PixelShader    pixelShader_;
	ConstantBuffer<ConstantMatrixBuffer_VS> matrixBuffer_;

	const std::string className_{ "depth_shader_class" };
};

