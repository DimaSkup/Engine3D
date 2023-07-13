////////////////////////////////////////////////////////////////////
// Filename:     LightMapShaderClass.cpp
// Description:  an implementation of the LightmapShaderClass class;
//
// Created:      13.01.23
////////////////////////////////////////////////////////////////////
#include "LightMapShaderClass.h"



// class constructor
LightMapShaderClass::LightMapShaderClass() 
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
};

// class copy constructor
LightMapShaderClass::LightMapShaderClass(const LightMapShaderClass& copy) {};

// class destructor
LightMapShaderClass::~LightMapShaderClass() {};



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialize the light map HLSL shaders
bool LightMapShaderClass::Initialize(ID3D11Device* pDevice,
									 ID3D11DeviceContext* pDeviceContext,
									 HWND hwnd)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	WCHAR* vsFilename = L"shaders/lightMapVertex.hlsl";
	WCHAR* psFilename = L"shaders/lightMapPixel.hlsl";

	// initialize the vertex and pixel shaders
	result = this->InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can'n initialize shaders");

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// render light mapped textures using HLSL shaders
bool LightMapShaderClass::Render(ID3D11DeviceContext* pDeviceContext, 
	const int indexCount,
	const DirectX::XMMATRIX & world,
	ID3D11ShaderResourceView* const* textureArray,
	DataContainerForShadersClass* pDataForShader)
{
	bool result = false;
	
	// set the shaders parameters that it will use for rendering
	result = this->SetShadersParameters(pDeviceContext,
		world, 
		pDataForShader->GetViewMatrix(), 
		pDataForShader->GetProjectionMatrix(), 
		textureArray);
	COM_ERROR_IF_FALSE(result, "can't set shaders parameters");

	// render prepared buffers with the shaders
	this->RenderShader(pDeviceContext, indexCount);

	return true;
}


const std::string & LightMapShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


bool LightMapShaderClass::InitializeShaders(ID3D11Device* pDevice,
											ID3D11DeviceContext* pDeviceContext,
											HWND hwnd,
											WCHAR* vsFilename,
											WCHAR* psFilename)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	bool result = false;
	constexpr UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];


	// set up the vertex input layout description;
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
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the matrix const buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");

	return true;
} // InitializeShaders()



  // SetShadersParameters() sets the matrices and texture array 
  // in the shaders before rendering;
bool LightMapShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
											   const DirectX::XMMATRIX & worldMatrix,
											   const DirectX::XMMATRIX & viewMatrix,
											   const DirectX::XMMATRIX & projectionMatrix,
											   ID3D11ShaderResourceView* const* textureArray)
{
	bool result = false;
	UINT bufferNumber = 0; // set the position of the matrix constant buffer in the vertex shader

	// ----------------------- UPDATE THE VERTEX SHADER --------------------------------- //

	// update matrix buffer data
	this->matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(worldMatrix);
	this->matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(viewMatrix);
	this->matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = this->matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, this->matrixBuffer_.GetAddressOf());

	// ------------------------ UPDATE THE PIXEL SHADER --------------------------------- //

	// set shader texture array resource in the pixel shader
	pDeviceContext->PSSetShaderResources(0, 2, textureArray);

	return true;
} // SetShadersParameters()


// render the light mapped textures and model using the HLSL shaders
void LightMapShaderClass::RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	// set the vertex input layout
	pDeviceContext->IASetInputLayout(this->vertexShader_.GetInputLayout());

	// set the vertex and pixel shader that will be used to render the model
	pDeviceContext->VSSetShader(this->vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(this->pixelShader_.GetShader(),  nullptr, 0);

	// set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, this->samplerState_.GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
