////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     ReflectionShaderClass.cpp
// Description:  implementation of the ReflectionShaderClass functional;
//
// Revising:     12.01.24
////////////////////////////////////////////////////////////////////////////////////////////
#include "ReflectionShaderClass.h"

ReflectionShaderClass::ReflectionShaderClass()
	: className_ {__func__}
{
	Log::Debug(LOG_MACRO);
}


ReflectionShaderClass::~ReflectionShaderClass()
{
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
////////////////////////////////////////////////////////////////////////////////////////////

// Loads the texture HLSL files for this shader
bool ReflectionShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/reflectionVS.hlsl";
		const WCHAR* psFilename = L"shaders/reflectionPS.hlsl";

		InitializeShaders(pDevice,
			pDeviceContext,
			vsFilename,
			psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the reflection shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

// Sets variables are used inside the shaders and renders the model using these shaders. 
// Also this function takes a parameters called texture
// which is the pointer to the texture resource.
bool ReflectionShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,            
	const DirectX::XMMATRIX & projection,      
	const DirectX::XMMATRIX & reflectionMatrix,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap)
{
	try
	{
		// Set the shaders parameters that will be used for rendering
		SetShadersParameters(pDeviceContext,
			world,
			view,
			projection,
			reflectionMatrix,
			texturesMap);

		// Now render the prepared buffers with the shaders
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render");
		return false;
	}

	return true;
}


const std::string & ReflectionShaderClass::GetShaderName() const
{
	return className_;
}





////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE METHODS
//
////////////////////////////////////////////////////////////////////////////////////////////

// initialized the vertex shader, pixel shader, input layout, and sampler;
void ReflectionShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[layoutElemNum];
	D3D11_SAMPLER_DESC samplerDesc;


	// ------------------------------- INPUT LAYOUT DESC -------------------------------- //

	// Create the vertex input layout description
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, polygonLayout, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// create a texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice, &samplerDesc);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the matrix const buffer (for the vertex shader)
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	// initialize the reflection constant buffer (for the pixel shader)
	hr = this->reflectionConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the reflection constant buffer");

	return;

} // end InitializeShader

///////////////////////////////////////////////////////////

void ReflectionShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const DirectX::XMMATRIX & reflectionMatrix,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap)
{
	// Sets the variables which are used within the vertex shader.
	// This function takes in a pointer to a texture resource and then assigns it to 
	// the shader using a texture resource pointer. Note that the texture has to be set 
	// before rendering of the buffer occurs.

	bool result = false;


	// ---------------- SET MATRIX CONST BUFFER FOR THE VERTEX SHADER -------------------- //

	// update data of the matrix const buffer
	matrixConstBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixConstBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixConstBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	result = matrixConstBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "failed to update the matrix constant buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixConstBuffer_.GetAddressOf());



	// ---------------- SET REFLECTION CONST BUFFER FOR THE VERTEX SHADER ---------------- //

	// update data of the reflection constant buffer
	reflectionConstBuffer_.data.reflectionMatrix = DirectX::XMMatrixTranspose(reflectionMatrix);
	
	result = matrixConstBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "failed to update the reflection constant buffer");

	// set the reflection constant buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(1, 1, reflectionConstBuffer_.GetAddressOf());



	// ---------------------- SET TEXTURES FOR THE PIXEL SHADER ------------------------- //

	// Set shader texture resources for the pixel shader
	try
	{	
		pDeviceContext->PSSetShaderResources(0, 1, texturesMap.at("diffuse"));
		pDeviceContext->PSSetShaderResources(1, 1, texturesMap.at("reflection_texture"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}


	return;

} // end SetShadersParameters

///////////////////////////////////////////////////////////
  
void ReflectionShaderClass::RenderShader(ID3D11DeviceContext* deviceContext,
	const UINT indexCount)
{
	// this function calls the shader technique to render the polygons

	// Set the vertex input layout
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// Set the vertex and pixels shaders that will be used to render the model
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// Render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}