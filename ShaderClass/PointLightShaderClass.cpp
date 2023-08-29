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
	WCHAR* vsFilename = L"shaders/pointLightVertex.hlsl";
	WCHAR* psFilename = L"shaders/pointLightPixel.hlsl";

	try
	{
		// try to initialize the vertex and pixel HLSL shaders
		bool result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
		COM_ERROR_IF_FALSE(result, "can't initialize the vertex/pixel shader");
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
bool PointLightShaderClass::Render(ID3D11DeviceContext* deviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,               // model's world
	ID3D11ShaderResourceView* const* textureArray,
	DataContainerForShadersClass* pDataForShader)  // contains different data is needed for rendering (for instance: matrices, camera data, light sources data, etc.)
{
	return false;
};


bool PointLightShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const pTextureArray,
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
bool PointLightShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	WCHAR* vsFilename,
	WCHAR* psFilename)
{
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

	try
	{
		// initialize the vertex shader
		if (!this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum))
			COM_ERROR_IF_FALSE(false, "can't initialize the vertex shader");

		// initialize the pixel shader
		if (!this->pixelShader_.Initialize(pDevice, psFilename))
			COM_ERROR_IF_FALSE(false, "can't initialize the pixel shader");

		// initialize the sampler state
		if (!this->samplerState_.Initialize(pDevice))
			COM_ERROR_IF_FALSE(false, "can't initialize the sampler state");

		// initialize the constant matrix buffer
		if (!this->matrixBuffer_.Initialize(pDevice, pDeviceContext))
			COM_ERROR_IF_FAILED(false, "can't initialize the matrix buffer");

		// initialize the constant buffer for lights colours 
		if (!this->lightColorBuffer_.Initialize(pDevice, pDeviceContext))
			COM_ERROR_IF_FAILED(false, "can't initialize the constant buffer for light colours");

		// initialize the constant buffer for lights positions
		if (!this->lightPositionBuffer_.Initialize(pDevice, pDeviceContext))
			COM_ERROR_IF_FAILED(false, "can't initialize the constant buffer for light positions");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;
} // InitializeShaders()



// sets parameters for the HLSL shaders: updates constant buffers, 
// sets shader texture resources, etc.
void PointLightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const pTextureArray,
	const DirectX::XMFLOAT4* pPointLightColor,
	const DirectX::XMFLOAT4* pPointLightPosition)
{
	HRESULT hr = S_OK;
	UINT bufferNumber = 0;   // the buffer position in the shader


	// ---------------------------------------------------------------------------------- //
	//                UPDATE THE MATRICES BUFFER IN THE VERTEX SHADER                     //
	// ---------------------------------------------------------------------------------- //

	// copy the matrices into the constant buffer
	matrixBuffer_.data.world = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	// update the matrix buffer
	if (!matrixBuffer_.ApplyChanges())
		COM_ERROR_IF_FALSE(false, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, matrixBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//            UPDATE THE LIGHT POSITIONS BUFFER IN THE VERTEX SHADER                  //
	// ---------------------------------------------------------------------------------- //

	// copy the light position variables into the constant buffer
	for (UINT i = 0; i < NUM_LIGHTS; i++)
	{
		lightPositionBuffer_.data.lightPosition[i].x = pPointLightPosition[i].x;
		lightPositionBuffer_.data.lightPosition[i].y = pPointLightPosition[i].y;
		lightPositionBuffer_.data.lightPosition[i].z = pPointLightPosition[i].z;
		lightPositionBuffer_.data.lightPosition[i].w = pPointLightPosition[i].w;
	}

	// update the light positions buffer
	if (!lightPositionBuffer_.ApplyChanges())
		COM_ERROR_IF_FALSE(false, "can't update the light position buffer");

	// update the buffer number in the vertex shader
	bufferNumber = 1;

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, lightPositionBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//              UPDATE THE LIGHT COLORS BUFFER IN THE PIXEL SHADER                    //
	// ---------------------------------------------------------------------------------- //

	// copy the light colors variables into the constant buffer
	for (UINT i = 0; i < NUM_LIGHTS; i++)
	{
		lightColorBuffer_.data.diffuseColor[i].x = pPointLightColor[i].x;
		lightColorBuffer_.data.diffuseColor[i].y = pPointLightColor[i].y;
		lightColorBuffer_.data.diffuseColor[i].z = pPointLightColor[i].z;
		lightColorBuffer_.data.diffuseColor[i].w = pPointLightColor[i].w;
	}

	// update the light positions buffer
	if (!lightColorBuffer_.ApplyChanges())
		COM_ERROR_IF_FALSE(false, "can't update the light color buffer");

	// update the buffer number in the pixel shader
	bufferNumber = 0;

	// set the buffer for the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, lightColorBuffer_.GetAddressOf());



	// set the shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &pTextureArray);

	return;
} // SetShaderParameters


  // sets stuff which we will use: layout, vertex and pixel shader, sampler state
  // and also renders our 3D model
void PointLightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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