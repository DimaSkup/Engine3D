////////////////////////////////////////////////////////////////////
// Filename:     PointLightShaderClass.cpp
// Description:  this class is needed for rendering textured models 
//               with multiple POINT LIGHTS on it using HLSL shaders.
//
// Created:      28.08.23
////////////////////////////////////////////////////////////////////
#include "PointLightShaderClass.h"

PointLightShaderClass::PointLightShaderClass()
	: className_(__func__)
{
	Log::Debug(LOG_MACRO);
	
}

PointLightShaderClass::~PointLightShaderClass(void)
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool PointLightShaderClass::Initialize(ID3D11Device* pDevice, 
	                                   ID3D11DeviceContext* pDeviceContext)
{
	// try to initialize the vertex/pixel shader, sampler state, and constant buffers
	try
	{
		const WCHAR* vsFilename = L"shaders/pointLightVertex.hlsl";
		const WCHAR* psFilename = L"shaders/pointLightPixel.hlsl";
	
		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize the PointLightShaderClass instance");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
void PointLightShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & viewProj,
	const LightSourcePointStore & pointLights,
	ID3D11ShaderResourceView* const* ppDiffuseTexture)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext, 
			world,
			viewProj,
			pointLights,
			ppDiffuseTexture);

		// render the model using this shader
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't render the model");
	}
	return;
}


const std::string & PointLightShaderClass::GetShaderName() const
{
	return className_;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
void PointLightShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	bool result = false;
	const UINT layoutElemNum = 3;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
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

	layoutDesc[2].SemanticName = "NORMAL";
	layoutDesc[2].SemanticIndex = 0;
	layoutDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[2].InputSlot = 0;
	layoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[2].InstanceDataStepRate = 0;



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



	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the constant buffer for lights colours 
	hr = this->lightColorBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant buffer for light colours");

	// initialize the constant buffer for lights positions
	hr = this->lightPositionBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant buffer for light positions");

	return;
}

///////////////////////////////////////////////////////////

void PointLightShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & viewProj,
	const LightSourcePointStore & pointLights,
	ID3D11ShaderResourceView* const* ppDiffuseTexture)
{
	// sets parameters for the HLSL shaders: updates constant buffers, 
	// sets shader texture resources, etc.

	bool result = false;

	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// copy the matrices into the constant buffer
	matrixBuffer_.data.world         = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(world * viewProj);


	// update the matrix buffer
	result = matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                VERTEX SHADER: UPDATE THE POINT LIGHTS POSITIONS                    //
	// ---------------------------------------------------------------------------------- //


	// copy the light position variables into the constant buffer
	for (UINT i = 0; i < NUM_POINT_LIGHTS; i++)
		lightPositionBuffer_.data.lightPosition[i] = pointLights.positions_[i];

	// update the light positions buffer
	result = lightPositionBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the light position buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(1, 1, lightPositionBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//                VERTEX SHADER: UPDATE THE POINT LIGHTS COLORS                       //
	// ---------------------------------------------------------------------------------- //

	// copy the light colors variables into the constant buffer
	for (UINT i = 0; i < NUM_POINT_LIGHTS; i++)
		lightColorBuffer_.data.diffuseColor[i] = pointLights.colors_[i];

	// update the light positions buffer
	result = lightColorBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the light color buffer");

	// set the buffer for the pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, lightColorBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//                            PIXEL SHADER: SET TEXTURES                              //
	// ---------------------------------------------------------------------------------- //

	try
	{
		pDeviceContext->PSSetShaderResources(0, 1, ppDiffuseTexture);
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}

	return;
} // SetShaderParameters


  // sets stuff which we will use: layout, vertex and pixel shader, sampler state
  // and also renders our 3D model
void PointLightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount)
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