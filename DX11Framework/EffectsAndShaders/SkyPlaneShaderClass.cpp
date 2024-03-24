////////////////////////////////////////////////////////////////////
// Filename:     SkyPlaneShaderClass.cpp
// Description:  this is the shader used for rendering the clouds
//               on the sky plane
// Created:      27.06.23
////////////////////////////////////////////////////////////////////
#include "SkyPlaneShaderClass.h"

SkyPlaneShaderClass::SkyPlaneShaderClass()
	: className_ {__func__}
{
	Log::Debug(LOG_MACRO);
}

SkyPlaneShaderClass::~SkyPlaneShaderClass()
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool SkyPlaneShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/skyPlaneVertex.hlsl";
		const WCHAR* psFilename = L"shaders/skyPlanePixel.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, 
			pDeviceContext, 
			vsFilename, 
			psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the sky plane shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;

} // end Initialize

///////////////////////////////////////////////////////////


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool SkyPlaneShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
	float firstTranslationX,
	float firstTranslationZ,
	float secondTranslationX,
	float secondTranslationZ,
	float brightness)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			world,                                   // world matrix
			view,                                    // view matrix
			projection,                              // projection matrix
			texturesMap,                             // cloud textures
			firstTranslationX,                       // first cloud translation by X-axis
			firstTranslationZ,                       // first cloud translation by Z-axis
			secondTranslationX,                      // second cloud translation by X-axis
			secondTranslationZ,                      // second cloud translation by Z-axis
			brightness);                             // brightness of the clouds

													 
		RenderShader(pDeviceContext, indexCount);    // render the model using HLSL shaders
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render");
		return false;
	}

	return true;
}


const std::string & SkyPlaneShaderClass::GetShaderName() const
{
	return className_;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
void SkyPlaneShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	const UINT layoutElemNum = 2;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);     // description for the sampler state
	bool result = false;
	HRESULT hr = S_OK;


	// set the description for the input layout
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	layoutDesc[1].SemanticName = "TEXCOORD";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;


	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");


	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");


	// setup some params of a texture sampler state description;
	// (the other params are set to default)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice, &samplerDesc);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initializer the constant matrix buffer");

	// initialize the constant sky buffer
	hr = skyBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initializer the constant sky buffer");

	return;
} // InitializeShaders()



 
void SkyPlaneShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap, // cloud textures
	float firstTranslationX,                    // first cloud translation by X-axis
	float firstTranslationZ,                    // first cloud translation by Z-axis
	float secondTranslationX,                   // second cloud translation by X-axis
	float secondTranslationZ,                   // second cloud translation by Z-axis
	float brightness)                           // brightness of the clouds
{
	// this function sets parameters for the HLSL shaders

	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer
	matrixBuffer_.data.world = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	// update the constant matrix buffer
	bool result = matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                  PIXEL SHADER: UPDATE THE CONSTANT BUFFERS                         //
	// ---------------------------------------------------------------------------------- //

	// write data into the buffer
	skyBuffer_.data.firstTranslationX = firstTranslationX;
	skyBuffer_.data.firstTranslationZ = firstTranslationZ;
	skyBuffer_.data.secondTranslationX = secondTranslationX;
	skyBuffer_.data.secondTranslationZ = secondTranslationZ;
	skyBuffer_.data.brightness = brightness;

	// update the constant buffer
	result = skyBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the constant light buffer for the HLSL pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, skyBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                  PIXEL SHADER: UPDATE SHADER TEXTURE RESOURCES                     //
	// ---------------------------------------------------------------------------------- //

	try
	{
		// go through each texture and set it for the pixel shader
		assert("FIX SETTING OF TEXTURES FOR THE SKY PLANE" && 0);
		//pDeviceContext->PSSetShaderResources(i, 1, texturesMap.at(textureKeys_[i]));

	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}
	
	//pDeviceContext->PSSetShaderResources(0, 1, &(pTextureArray[0]));  // first cloud
	//pDeviceContext->PSSetShaderResources(1, 1, &(pTextureArray[1]));  // second cloud

	return;

} // end SetShaderParameters

  ///////////////////////////////////////////////////////////

void SkyPlaneShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, 
	const UINT indexCount)
{
	// this function sets stuff which we will use: layout, vertex and pixel shader,
	// sampler state, and also renders our 3D model

	// set the input layout for the vertex shader
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// set shader which we will use for rendering
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state for the pixel shader
	deviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader