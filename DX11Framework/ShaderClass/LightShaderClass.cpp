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
	try
	{
		const WCHAR* vsFilename = L"shaders/lightVertex.hlsl";
		const WCHAR* psFilename = L"shaders/lightPixel.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the light shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
} // end Initialize

///////////////////////////////////////////////////////////

bool LightShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	// we call this function from the model_to_shader mediator

	try
	{
		// set the shader parameters
		SetShaderParameters(pDeviceContext,
			pDataForShader->world,
			pDataForShader->view,
			pDataForShader->orthoOrProj,
			pDataForShader->texturesMap,
			pDataForShader->cameraPos,
			*pDataForShader->ptrToDiffuseLightsArr);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

	return true;

} // end Render

///////////////////////////////////////////////////////////


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shadersbool Render(ID3D11DeviceContext* deviceContext,
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
	const DirectX::XMFLOAT3 & cameraPosition,
	const std::vector<LightClass*> & diffuseLightsArr)
{

	COM_ERROR_IF_ZERO(texturesMap.size(), "there is no data in the textures map");
	COM_ERROR_IF_ZERO(diffuseLightsArr.size(), "there is no data in the diffuse light sources array");

	try
	{
		// set the shader parameters
		SetShaderParameters(deviceContext,
			world,
			view,
			projection,
			texturesMap,
			cameraPosition,
			diffuseLightsArr);

		// render the model using this shader
		RenderShader(deviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

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
void LightShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	bool result = false;
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


	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");


	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	// initialize the constant light buffer
	hr = this->lightBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the light buffer");

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	return;
}



// sets parameters for the HLSL shaders
void LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap,
	const DirectX::XMFLOAT3 & cameraPosition,
	const std::vector<LightClass*> & diffuseLightsArr)
{
	bool result = false;
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
	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");


	// set the buffer position
	bufferPosition = 0;

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, matrixBuffer_.GetAddressOf());

	// set the shader resource for the vertex shader
	deviceContext->PSSetShaderResources(0, 1, texturesMap.at("diffuse"));


	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT CAMERA BUFFER                              //
	// ---------------------------------------------------------------------------------- //

	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;
	cameraBuffer_.data.padding = 0.0f;

	// update the constant camera buffer
	result = cameraBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer position in the vertex shader
	bufferPosition = 1;  

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, cameraBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT LIGHT BUFFER                               //
	// ---------------------------------------------------------------------------------- //

	// write data into the buffer
	lightBuffer_.data.diffuseColor   = diffuseLightsArr[0]->GetDiffuseColor();
	lightBuffer_.data.lightDirection = diffuseLightsArr[0]->GetDirection();
	lightBuffer_.data.ambientColor   = diffuseLightsArr[0]->GetAmbientColor();

	// update the constant camera buffer
	result = lightBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer position in the pixel shader
	bufferPosition = 0;

	// set the constant light buffer for the HLSL pixel shader
	deviceContext->PSSetConstantBuffers(bufferPosition, 1, lightBuffer_.GetAddressOf());

	return;
}


// sets stuff which we will use: layout, vertex and pixel shader, sampler state
// and also renders our 3D model
void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount)
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
}