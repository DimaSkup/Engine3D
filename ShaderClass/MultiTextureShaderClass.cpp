////////////////////////////////////////////////////////////////////
// Filename:    MultiTextureShaderClass.cpp
// Description: an implementation of the MultiTextureShaderClass
// Created:     09.01.23
////////////////////////////////////////////////////////////////////
#include "MultiTextureShaderClass.h"




////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// initialize the multitexture HLSL shaders
bool MultiTextureShaderClass::Initialize(ID3D11Device* pDevice, 
										 ID3D11DeviceContext* pDeviceContext, 
										 HWND hwnd)
{
	bool result = false;
	WCHAR* vsFilename = L"shaders/MultiTextureVertex.hlsl";
	WCHAR* psFilename = L"shaders/MultiTexturePixel.hlsl";

	// initialize the vertex and pixel shaders
	result = this->InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the multitexture shaders");

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// the Render() function takes as input a pointer to the texture array.
// This will give the shader access to the two textures for blending operations.
bool MultiTextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,
	ID3D11ShaderResourceView* const* textureArray,
	DataContainerForShadersClass* pDataForShader)
{
	bool result = false;

	// set the shaders parameters that will be used for rendering
	result = this->SetShadersParameters(pDeviceContext, 
		world, 
		pDataForShader->GetViewMatrix(), 
		pDataForShader->GetProjectionMatrix(), 
		textureArray);
	COM_ERROR_IF_FALSE(result, "can't set shaders parameters for rendering");

	// now render the prepared buffers with the shader
	this->RenderShaders(pDeviceContext, indexCount);

	return true;
}


////////////////////////////////////////////////////////////////////
//
//                     PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// the InitializeShaders() loads the vertex and pixel shaders as well as 
// setting up the layout, matrix buffer, and sample state
bool MultiTextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
												ID3D11DeviceContext* pDeviceContext,
												HWND hwnd,
												WCHAR* vsFilename,
												WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	constexpr UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];


	// set up the input layout description
	// this setup needs to match the VERTEX structure in the ModelClass and in the shader
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	layoutDesc[1].SemanticName = "TEXCOORD";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[1].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the multi texture vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the multi texture pixel shader");

	// initialize the constant matrix buffer
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant matrix buffer");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");

	return true;
} // InitializeShaders()


// SetShadersParameters() sets the matrices and texture array 
// in the shaders before rendering;
bool MultiTextureShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
												   DirectX::XMMATRIX worldMatrix,
												   DirectX::XMMATRIX viewMatrix,
												   DirectX::XMMATRIX projectionMatrix,
												   ID3D11ShaderResourceView* const* textureArray)
{
	//HRESULT hr = S_OK;
	bool result = false;
	UINT bufferNumber = 0;  // set the position of the matrix constant buffer in the vertex shader


	// ------------------------- UPDATE THE VERTEX SHADER -------------------------- //

	// transpose matrices and update the matrix constant buffer
	this->matrixConstBuffer_.data.world      = DirectX::XMMatrixTranspose(worldMatrix);
	this->matrixConstBuffer_.data.view       = DirectX::XMMatrixTranspose(viewMatrix);
	this->matrixConstBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = this->matrixConstBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the constant matrix buffer");

	// set the matrix constant buffer in the vertex shader with the updated valuved
	pDeviceContext->VSSetConstantBuffers(0, 1, this->matrixConstBuffer_.GetAddressOf());


	// ------------------------- UPDATE THE PIXEL SHADER --------------------------- //

	// set shader texture array resource in the pixel shader
	pDeviceContext->PSSetShaderResources(0, 2, textureArray);


	return true;
} // SetShadersParameters()


// The RenderShaders() sets the layout, shaders, and sampler.
// It then draws the model using the HLSL shaders
void MultiTextureShaderClass::RenderShaders(ID3D11DeviceContext* pDeviceContext,
											int indexCount)
{
	// set the vertex input layout
	pDeviceContext->IASetInputLayout(this->vertexShader_.GetInputLayout());

	// set the vertex and pixel shaders that will be used for rendering
	pDeviceContext->VSSetShader(this->vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(this->pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
	

	return;
}
