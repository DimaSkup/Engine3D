////////////////////////////////////////////////////////////////////
// Filename:     LightMapShaderClass.cpp
// Description:  an implementation of the LightmapShaderClass class;
//
// Created:      13.01.23
////////////////////////////////////////////////////////////////////
#include "LightMapShaderClass.h"



// class constructor
LightMapShaderClass::LightMapShaderClass() 
	: className_{__func__}
{
	Log::Debug(LOG_MACRO);
};

// class destructor
LightMapShaderClass::~LightMapShaderClass() {};



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialize the light map HLSL shaders
bool LightMapShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/lightMapVertex.hlsl";
		const WCHAR* psFilename = L"shaders/lightMapPixel.hlsl";

		// initialize the vertex and pixel shaders
		this->InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the light map shader class");
		return false;
	}
	

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}


// render light mapped textures using HLSL shaders
bool LightMapShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount)
{
	try
	{
		// set the shader parameters which will be used for rendering
		SetShaderParameters(pDeviceContext);

		// render prepared buffers with the shaders
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can' initialize");
		return false;
	}

	return true;
}


const std::string & LightMapShaderClass::GetShaderName() const
{
	return className_;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


void LightMapShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
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

	return;
} // InitializeShaders()



  // SetShadersParameters() sets the matrices and texture array 
  // in the shaders before rendering;
void LightMapShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext)
{
	bool result = false;
#if 0

	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// update matrix buffer data
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(pDataForShader->world);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(pDataForShader->view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(pDataForShader->projection);

	result = this->matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//                            PIXEL SHADER: SET TEXTURES                              //
	// ---------------------------------------------------------------------------------- //

	try
	{
		assert("FIX SETTING OF THESE TEXTURES BECAUSE THERE ARE NO 2 DIFFUSE TEXTURES");
		pDeviceContext->PSSetShaderResources(0, 2, pDataForShader->texturesMap.at("diffuse"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}
#endif
	return;
} // SetShadersParameters()


// render the light mapped textures and model using the HLSL shaders
void LightMapShaderClass::RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
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
