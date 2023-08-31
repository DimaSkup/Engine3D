////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.cpp
// Description:  this class is needed for rendering 3D models, 
//               its texture, simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#include "LightShaderClass.h"

LightShaderClass::LightShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

LightShaderClass::~LightShaderClass(void)
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool LightShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	WCHAR* vsFilename = L"shaders/lightVertex.hlsl";
	WCHAR* psFilename = L"shaders/lightPixel.hlsl";

	// try to initialize the vertex and pixel HLSL shaders
	result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize shaders");

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shadersbool Render(ID3D11DeviceContext* deviceContext,
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* texture,
	const DirectX::XMFLOAT3 & cameraPosition,
	const DirectX::XMFLOAT4 & diffuseColor,
	const DirectX::XMFLOAT3 & lightDirection,
	const DirectX::XMFLOAT4 & ambientColor)
{
	bool result = false;

	// set the shader parameters
	result = SetShaderParameters(deviceContext,
		world,
		pDataForShader->GetViewMatrix(),
		pDataForShader->GetProjectionMatrix(),
		textureArray[0],
		pDataForShader->GetCameraPosition(),
		pDataForShader->GetDiffuseLight()->GetDiffuseColor(),
		pDataForShader->GetDiffuseLight()->GetDirection(),
		pDataForShader->GetDiffuseLight()->GetAmbientColor());
	COM_ERROR_IF_FALSE(result, "can't set the shader parameters");


	// render the model using this shader
	RenderShader(deviceContext, indexCount);

	return true;
}


const std::string & LightShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
bool LightShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	WCHAR* vsFilename,
	WCHAR* psFilename)
{
	//Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	const UINT layoutElemNum = 3;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout


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

	layoutDesc[2].SemanticName = "NORMAL";
	layoutDesc[2].SemanticIndex = 0;
	layoutDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[2].InputSlot = 0;
	layoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[2].InstanceDataStepRate = 0;

	// initialize the vertex shader
	if (!this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum))
		return false;


	// initialize the pixel shader
	if (!this->pixelShader_.Initialize(pDevice, psFilename))
		return false;


	// initialize the sampler state
	if (!this->samplerState_.Initialize(pDevice))
		return false;


	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;

	// initialize the constnat light buffer
	hr = this->lightBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;

	return true;
} // InitializeShaders()



  // sets parameters for the HLSL shaders
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* texture,  // a texture resource for the model
	const DirectX::XMFLOAT3 & cameraPosition,
	const DirectX::XMFLOAT4 & diffuseColor,
	const DirectX::XMFLOAT3 & lightDirection,
	const DirectX::XMFLOAT4 & ambientColor)
{
	HRESULT hr = S_OK;
	UINT bufferPosition = 0;


	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT MATRIX BUFFER                              //
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
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, matrixBuffer_.GetAddressOf());

	// set the shader resource for the vertex shader
	deviceContext->PSSetShaderResources(0, 1, &texture);


	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT CAMERA BUFFER                              //
	// ---------------------------------------------------------------------------------- //

	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;
	cameraBuffer_.data.padding = 0.0f;

	// update the constant camera buffer
	if (!cameraBuffer_.ApplyChanges())
		return false;

	// set the buffer position in the vertex shader
	bufferPosition = 1;  // because the matrix buffer in zero position

						 // set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, cameraBuffer_.GetAddressOf());

	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT LIGHT BUFFER                               //
	// ---------------------------------------------------------------------------------- //

	// write data into the buffer
	lightBuffer_.data.diffuseColor = diffuseColor;
	lightBuffer_.data.lightDirection = lightDirection;
	lightBuffer_.data.ambientColor = ambientColor;

	// update the constant camera buffer
	if (!lightBuffer_.ApplyChanges())
		return false;

	// set the buffer position in the pixel shader
	bufferPosition = 0;

	// set the constant light buffer for the HLSL pixel shader
	deviceContext->PSSetConstantBuffers(bufferPosition, 1, lightBuffer_.GetAddressOf());

	return true;
} // SetShaderParameters


  // sets stuff which we will use: layout, vertex and pixel shader, sampler state
  // and also renders our 3D model
void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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