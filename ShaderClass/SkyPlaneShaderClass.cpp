////////////////////////////////////////////////////////////////////
// Filename:     SkyPlaneShaderClass.cpp
// Description:  this is the shader used for rendering the clouds
//               on the sky plane
// Created:      27.06.23
////////////////////////////////////////////////////////////////////
#include "SkyPlaneShaderClass.h"

SkyPlaneShaderClass::SkyPlaneShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

SkyPlaneShaderClass::~SkyPlaneShaderClass(void)
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool SkyPlaneShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	WCHAR* vsFilename = L"shaders/skyPlaneVertex.hlsl";
	WCHAR* psFilename = L"shaders/skyPlanePixel.hlsl";

	// try to initialize the vertex and pixel HLSL shaders
	result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize shaders");

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool SkyPlaneShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,
	ID3D11ShaderResourceView* const* textureArray,
	DataContainerForShadersClass* pDataForShader)  // contains different data is needed for rendering (for instance: matrices, camera data, light sources data, etc.)
{
	assert(pDeviceContext != nullptr);

	bool result = false;

	// get some data from the shaders data container
	float* pSkyPlaneTranslation = static_cast<float*>(pDataForShader->GetDataByKey("SkyPlaneTranslation"));
	float* pSkyPlaneCloudBrigtness = static_cast<float*>(pDataForShader->GetDataByKey("SkyPlaneBrigtness"));

	// set the shader parameters
	result = SetShaderParameters(pDeviceContext,
		world,                                     // world matrix
		pDataForShader->GetViewMatrix(),           // view matrix
		pDataForShader->GetProjectionMatrix(),     // projection matrix
		textureArray[0],                           // first cloud texture
		textureArray[1],                           // second cloud texture
		pSkyPlaneTranslation[0],                   // first cloud translation by X-axis
		pSkyPlaneTranslation[1],                   // first cloud translation by Z-axis
		pSkyPlaneTranslation[2],                   // second cloud translation by X-axis
		pSkyPlaneTranslation[3],                   // second cloud translation by Z-axis
		*pSkyPlaneCloudBrigtness);                 // brightness of the clouds
	COM_ERROR_IF_FALSE(result, "can't set the shader parameters");


	// render the model using this shader
	RenderShader(pDeviceContext, indexCount);

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
bool SkyPlaneShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	WCHAR* vsFilename,
	WCHAR* psFilename)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	const UINT layoutElemNum = 2;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);     // description for the sampler state


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


	// initialize the vertex shader
	if (!this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum))
		return false;


	// initialize the pixel shader
	if (!this->pixelShader_.Initialize(pDevice, psFilename))
		return false;


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
	if (!this->samplerState_.Initialize(pDevice, &samplerDesc))
		return false;


	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initializer the constant matrix buffer");

	// initialize the constant sky buffer
	hr = this->skyBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initializer the constant sky buffer");

	return true;
} // InitializeShaders()



  // sets parameters for the HLSL shaders
bool SkyPlaneShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* texture,    // first cloud texture
	ID3D11ShaderResourceView* texture2,   // second cloud texture
	float firstTranslationX,              // first cloud translation by X-axis
	float firstTranslationZ,              // first cloud translation by Z-axis
	float secondTranslationX,             // second cloud translation by X-axis
	float secondTranslationZ,             // second cloud translation by Z-axis
	float brightness)                     // brightness of the clouds
{
	HRESULT hr = S_OK;
	UINT bufferPosition = 0;


	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer
	matrixBuffer_.data.world = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	// update the constant matrix buffer
	if (!matrixBuffer_.ApplyChanges())
		return false;


	// set the buffer position
	bufferPosition = 0;

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(bufferPosition, 1, matrixBuffer_.GetAddressOf());



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
	if (!skyBuffer_.ApplyChanges())
		return false;

	// set the buffer position in the pixel shader
	bufferPosition = 0;

	// set the constant light buffer for the HLSL pixel shader
	pDeviceContext->PSSetConstantBuffers(bufferPosition, 1, skyBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                  PIXEL SHADER: UPDATE SHADER TEXTURE RESOURCES                     //
	// ---------------------------------------------------------------------------------- //
	// set the shader resource for the vertex shader
	pDeviceContext->PSSetShaderResources(0, 1, &texture);   // first cloud
	pDeviceContext->PSSetShaderResources(1, 1, &texture2);  // second cloud

	return true;
} // SetShaderParameters


  // sets stuff which we will use: layout, vertex and pixel shader, sampler state
  // and also renders our 3D model
void SkyPlaneShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
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