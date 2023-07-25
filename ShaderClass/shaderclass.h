////////////////////////////////////////////////////////////////////
// Filename:     shaderclass.h
// Description:  this is a base shader class which contains common
//               utils for shader classes to work with shaders
//
// Revising:     12.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11async.h>   // is neccessary for the D3DX11CompileFromFile() function
#include <fstream>		
#include <DirectXMath.h>

#include "../Engine/Log.h"
#include "../Engine/StringConverter.h"
#include "../Engine/macros.h"

#include "../Model/ModelMediator.h"
#include "../ShaderClass/DataContainerForShadersClass.h"

//////////////////////////////////
// Class name: ShaderClass
//////////////////////////////////
class ShaderClass : public GraphicsComponent
{
public:
	virtual ~ShaderClass();

	virtual bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd) = 0;
	virtual bool Render(ID3D11DeviceContext* pDeviceContext,
		const int indexCount,
		const DirectX::XMMATRIX & worldMatrix,
		ID3D11ShaderResourceView* const* textureArray,
		DataContainerForShadersClass* pDataForShader) = 0;

	virtual const std::string & GetShaderName() const _NOEXCEPT = 0;


	static HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName,
		                                 LPCSTR shaderModel, ID3DBlob** shaderOutput); 

protected:
	std::string className_{ "shader_name" };
};
