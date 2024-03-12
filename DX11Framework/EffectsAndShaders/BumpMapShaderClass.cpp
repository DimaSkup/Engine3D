////////////////////////////////////////////////////////////////////
// Filename:     BumpMapShaderClass.cpp
// Description:  an implementation of the BumpMapShaderClass class;
//
// Created:      18.01.23
////////////////////////////////////////////////////////////////////
#include "BumpMapShaderClass.h"



// class constructor
BumpMapShaderClass::BumpMapShaderClass()
	: className_{__func__}
{
	Log::Debug(LOG_MACRO);
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
bool BumpMapShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/bumpMapVertex.hlsl";
		const WCHAR* psFilename = L"shaders/bumpMapPixel.hlsl";

		// initialize the vertex and pixel shaders
		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the bump map shader");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool BumpMapShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
	const LightStore & lightStore)
{
	// render bump mapped textures using HLSL shaders

	try
	{
		// set the shaders parameters that it will use for rendering
		SetShadersParameters(pDeviceContext,
			world,
			view,
			projection,
			texturesMap,
			lightStore);

		// render prepared buffers with the shaders
		this->RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't render");
	}

	return true;
}

///////////////////////////////////////////////////////////

const std::string & BumpMapShaderClass::GetShaderName() const
{
	return className_;
}





/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


void BumpMapShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
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



	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");


	// ------------------------------- CONSTANT BUFFERS --------------------------------- //

	// initialize the matrix const buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	// initialize the light const buffer
	hr = lightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light const buffer");


	return;
} /* InitializeShaders() */



// SetShadersParameters() sets the matrices and texture array 
// in the shaders before rendering;
void BumpMapShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & worldMatrix,
	const DirectX::XMMATRIX & viewMatrix,
	const DirectX::XMMATRIX & projectionMatrix,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
	const LightStore & lightStore)
{
	bool result = false;

	// ----------------------- UPDATE THE VERTEX SHADER --------------------------------- //

	// update the matrix buffer data
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(worldMatrix);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(viewMatrix);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());



	// ------------------------ UPDATE THE PIXEL SHADER --------------------------------- //

	try
	{
		// set shader texture array resource in the pixel shader
		pDeviceContext->PSSetShaderResources(0, 1, texturesMap.at("diffuse"));
		pDeviceContext->PSSetShaderResources(1, 1, texturesMap.at("normals"));
	}
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}

	
	// update the light buffer data
	DirectX::XMFLOAT3 lightDir;
	DirectX::XMStoreFloat3(&lightDir, lightStore.diffuseLightsStore_.directions_[0]);

	lightBuffer_.data.lightDirection = lightDir;
	lightBuffer_.data.diffuseColor   = lightStore.diffuseLightsStore_.diffuseColors_[0];
	lightBuffer_.data.ambientColor   = lightStore.diffuseLightsStore_.ambientColors_[0];

	result = lightBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the light const buffer");

	// set the light constant buffer in the pixel shader with the updated values
	pDeviceContext->PSSetConstantBuffers(0, 1, lightBuffer_.GetAddressOf());


	return;
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
