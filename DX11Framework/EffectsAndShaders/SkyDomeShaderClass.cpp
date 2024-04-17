////////////////////////////////////////////////////////////////////
// Filename:    SkyDomeShaderClass.cpp
// Description: an implementation of the SkyDomeShaderClass
// Created:     16.04.23
////////////////////////////////////////////////////////////////////
#include "SkyDomeShaderClass.h"


SkyDomeShaderClass::SkyDomeShaderClass()
	: className_{ __func__ }
{
	Log::Debug(LOG_MACRO);
}

SkyDomeShaderClass::~SkyDomeShaderClass()
{
}



////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// initialize the sky dome HLSL shaders
bool SkyDomeShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		WCHAR* vsFilename = L"shaders/SkyDomeVertex.hlsl";
		WCHAR* psFilename = L"shaders/SkyDomePixel.hlsl";

		// initialize the vertex and pixel shaders
		this->InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the sky dome shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

// the Render() function takes as input a pointer to the texture array.
// This will give the shader access to the two textures for blending operations.
bool SkyDomeShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & worldMatrix,
	const DirectX::XMMATRIX & viewMatrix,
	const DirectX::XMMATRIX & projectionMatrix,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,  // contains pairs ['texture_type' => 'texture_resource_view'] for the sky dome
	const DirectX::XMFLOAT4 & apexColor,            // the color of the sky dome's top
	const DirectX::XMFLOAT4 & centerColor)          // the color of the sky dome's horizon
{
	
	try
	{
		// set the shaders parameters that will be used for rendering
		this->SetShadersParameters(pDeviceContext,
			worldMatrix,
			viewMatrix,
			projectionMatrix,
			texturesMap,
			apexColor,
			centerColor);

		// now render the prepared buffers with the shader
		this->RenderShaders(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render");
		return false;
	}
	
	return true;
}


const std::string & SkyDomeShaderClass::GetShaderName() const
{
	return className_;
}


////////////////////////////////////////////////////////////////////
//
//                     PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// the InitializeShaders() loads the vertex and pixel shaders as well as 
// setting up the layout, matrix buffer, sample state, and other buffers
// for rendering the sky dome
void SkyDomeShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
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



	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the sky dome vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the sky dome pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant matrix buffer");

	// initialize the constant colour buffer
	hr = this->colorConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant colour buffer");

	return;
} // InitializeShaders()

///////////////////////////////////////////////////////////

void SkyDomeShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & worldMatrix,
	const DirectX::XMMATRIX & viewMatrix,
	const DirectX::XMMATRIX & projectionMatrix,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,  // contains pairs ['texture_type' => 'texture_resource_view'] for the sky dome
	const DirectX::XMFLOAT4 & apexColor,
	const DirectX::XMFLOAT4 & centerColor)
{
	// SetShadersParameters() sets the matrices and texture array 
	// in the shaders before rendering;

	// ------------------------- UPDATE THE VERTEX SHADER -------------------------- //

	// transpose matrices and update the matrix constant buffer
	matrixConstBuffer_.data.world = DirectX::XMMatrixTranspose(worldMatrix);
	matrixConstBuffer_.data.view = DirectX::XMMatrixTranspose(viewMatrix);
	matrixConstBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	// load matrices data into GPU
	matrixConstBuffer_.ApplyChanges(pDeviceContext);

	// set the matrix constant buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixConstBuffer_.GetAddressOf());


	// ------------------------- UPDATE THE PIXEL SHADER --------------------------- //

	// copy the color data into the color constant buffer
	colorConstBuffer_.data.apexColor = apexColor;
	colorConstBuffer_.data.centerColor = centerColor;

	// load color data into GPU
	colorConstBuffer_.ApplyChanges(pDeviceContext);

	// set the color constant buffer in the pixel shader with the updated values
	pDeviceContext->PSSetConstantBuffers(0, 1, colorConstBuffer_.GetAddressOf());

	// set shader texture array resource in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());
	


	// --------------------- SET TEXTURES FOR THE PIXEL SHADER ------------------------- //

	try
	{
		pDeviceContext->PSSetShaderResources(0, 1, texturesMap.at("diffuse"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}


	return;
}

///////////////////////////////////////////////////////////

void SkyDomeShaderClass::RenderShaders(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount)
{
	// The RenderShaders() sets the layout, shaders, and sampler.
	// It then draws the model using the HLSL shaders

	// set the vertex input layout
	pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// set the vertex and pixel shaders that will be used for rendering
	pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0U);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0U);

	// set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
