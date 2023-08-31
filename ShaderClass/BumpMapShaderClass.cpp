////////////////////////////////////////////////////////////////////
// Filename:     BumpMapShaderClass.cpp
// Description:  an implementation of the BumpMapShaderClass class;
//
// Created:      18.01.23
////////////////////////////////////////////////////////////////////
#include "BumpMapShaderClass.h"



// class constructor
BumpMapShaderClass::BumpMapShaderClass()
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
};


// class destructor
BumpMapShaderClass::~BumpMapShaderClass() 
{
};



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialize the alpha map HLSL shaders
bool BumpMapShaderClass::Initialize(ID3D11Device* pDevice,
									ID3D11DeviceContext* pDeviceContext,
									HWND hwnd)
{
	try
	{
		WCHAR* vsFilename = L"shaders/bumpMapVertex.hlsl";
		WCHAR* psFilename = L"shaders/bumpMapPixel.hlsl";

		// initialize the vertex and pixel shaders
		bool result = this->InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
		COM_ERROR_IF_FALSE(result, "can'n initialize shaders");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the bump map shader");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// render bump mapped textures using HLSL shaders
bool BumpMapShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const textureArray,
	const DirectX::XMFLOAT3 & lightDirection,
	const DirectX::XMFLOAT4 & diffuseColor)
{
	bool result = false;

	// set the shaders parameters that it will use for rendering
	result = this->SetShadersParameters(pDeviceContext, 
		world,
		view,
		projection,
		textureArray,
		lightDirection,
		diffuseColor);
	COM_ERROR_IF_FALSE(result, "can't set shaders parameters");

	// render prepared buffers with the shaders
	this->RenderShader(pDeviceContext, indexCount);

	return true;
}


const std::string & BumpMapShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}





/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


bool BumpMapShaderClass::InitializeShaders(ID3D11Device* pDevice,
											ID3D11DeviceContext* pDeviceContext,
											HWND hwnd,
											WCHAR* vsFilename,
											WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	constexpr UINT layoutElemNum = 5;
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

	layoutDesc[2].SemanticName = "NORMAL";
	layoutDesc[2].SemanticIndex = 0;
	layoutDesc[2].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[2].InputSlot = 0;
	layoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[2].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[2].InstanceDataStepRate = 0;

	// the tangent and binormal element are setup the same as the normal element with the
	// exception of the semantic name
	layoutDesc[3].SemanticName = "TANGENT";
	layoutDesc[3].SemanticIndex = 0;
	layoutDesc[3].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[3].InputSlot = 0;
	layoutDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[3].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[3].InstanceDataStepRate = 0;

	layoutDesc[4].SemanticName = "BINORMAL";
	layoutDesc[4].SemanticIndex = 0;
	layoutDesc[4].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[4].InputSlot = 0;
	layoutDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[4].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[4].InstanceDataStepRate = 0;



	// ---------------------------------- SHADERS --------------------------------------- //

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");


	// -------------------------------- SAMPLER STATE ----------------------------------- //

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");


	// ------------------------------- CONSTANT BUFFERS --------------------------------- //

	// initialize the matrix const buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	// initialize the light const buffer
	hr = this->lightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light const buffer");


	return true;
} /* InitializeShaders() */



// SetShadersParameters() sets the matrices and texture array 
// in the shaders before rendering;
bool BumpMapShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
											  const DirectX::XMMATRIX & worldMatrix,
											  const DirectX::XMMATRIX & viewMatrix,
											  const DirectX::XMMATRIX & projectionMatrix,
											  ID3D11ShaderResourceView* const textureArray,
											  const DirectX::XMFLOAT3 & lightDirection,
											  const DirectX::XMFLOAT4 & diffuseColor)
{
	UINT bufferNumber = 0; // set the position of the matrix constant buffer in the vertex shader
	bool result = false;

	// ----------------------- UPDATE THE VERTEX SHADER --------------------------------- //

	// update the matrix buffer data
	this->matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(worldMatrix);
	this->matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(viewMatrix);
	this->matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = this->matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, this->matrixBuffer_.GetAddressOf());



	// ------------------------ UPDATE THE PIXEL SHADER --------------------------------- //

	// set shader texture array resource in the pixel shader
	pDeviceContext->PSSetShaderResources(0, 2, &textureArray);

	// update the light buffer data
	this->lightBuffer_.data.diffuseColor = diffuseColor;
	this->lightBuffer_.data.lightDirection = lightDirection;

	result = this->lightBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the light const buffer");

	// set the position of the light const buffer in the pixel shader
	bufferNumber = 0;

	// set the light constant buffer in the pixel shader with the updated values
	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, this->lightBuffer_.GetAddressOf());


	return true;
} /* SetShadersParameters() */


// render the bump mapped textures and model using the HLSL shaders
void BumpMapShaderClass::RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
{
	// set the vertex input layout
	pDeviceContext->IASetInputLayout(this->vertexShader_.GetInputLayout());

	// set the vertex and pixel shader that will be used to render the model
	pDeviceContext->VSSetShader(this->vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(this->pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, this->samplerState_.GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
