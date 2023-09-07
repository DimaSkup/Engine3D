////////////////////////////////////////////////////////////////////
// Filename:     PointLightShaderClass.cpp
// Description:  this class is needed for rendering textured models 
//               with multiple POINT LIGHTS on it using HLSL shaders.
//
// Created:      28.08.23
////////////////////////////////////////////////////////////////////
#include "PointLightShaderClass.h"

PointLightShaderClass::PointLightShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
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
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	// try to initialize the vertex/pixel shader, sampler state, and constant buffers
	try
	{
		const WCHAR* vsFilename = L"shaders/pointLightVertex.hlsl";
		const WCHAR* psFilename = L"shaders/pointLightPixel.hlsl";
	
		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize the PointLightShaderClass instance");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool PointLightShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const* pTextureArray,
	const DirectX::XMFLOAT4* pPointLightColor,
	const DirectX::XMFLOAT4* pPointLightPosition)
{
	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			world, view, projection,
			pTextureArray,
			pPointLightColor,
			pPointLightPosition);

		// render the model using this shader
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render the model");
		return false;
	}

	return true;
}


const std::string & PointLightShaderClass::GetShaderName() const _NOEXCEPT
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
	HWND hwnd,
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
} // InitializeShaders()



// sets parameters for the HLSL shaders: updates constant buffers, 
// sets shader texture resources, etc.
void PointLightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const* pTextureArray,
	const DirectX::XMFLOAT4* pPointLightColor,
	const DirectX::XMFLOAT4* pPointLightPosition)
{
	bool result = false;

	// ---------------- SET PARAMS FOR THE VERTEX SHADER ------------------- //

	// copy the matrices into the constant buffer
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	// update the matrix buffer
	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());


	// copy the light position variables into the constant buffer
	for (UINT i = 0; i < NUM_LIGHTS; i++)
	{
		lightPositionBuffer_.data.lightPosition[i].x = pPointLightPosition[i].x;
		lightPositionBuffer_.data.lightPosition[i].y = pPointLightPosition[i].y;
		lightPositionBuffer_.data.lightPosition[i].z = pPointLightPosition[i].z;
		lightPositionBuffer_.data.lightPosition[i].w = pPointLightPosition[i].w;
	}

	// update the light positions buffer
	result = lightPositionBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the light position buffer");

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(1, 1, lightPositionBuffer_.GetAddressOf());



	// ---------------- SET PARAMS FOR THE PIXEL SHADER -------------------- //

	// copy the light colors variables into the constant buffer
	for (UINT i = 0; i < NUM_LIGHTS; i++)
	{
		lightColorBuffer_.data.diffuseColor[i].x = pPointLightColor[i].x;
		lightColorBuffer_.data.diffuseColor[i].y = pPointLightColor[i].y;
		lightColorBuffer_.data.diffuseColor[i].z = pPointLightColor[i].z;
		lightColorBuffer_.data.diffuseColor[i].w = pPointLightColor[i].w;
	}

	// update the light positions buffer
	result = lightColorBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the light color buffer");

	// set the buffer for the pixel shader
	deviceContext->PSSetConstantBuffers(0, 1, lightColorBuffer_.GetAddressOf());



	// set the shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, pTextureArray);

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