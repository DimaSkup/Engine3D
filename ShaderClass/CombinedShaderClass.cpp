////////////////////////////////////////////////////////////////////
// Filename: CombinedShaderClass.cpp
////////////////////////////////////////////////////////////////////
#include "CombinedShaderClass.h"

CombinedShaderClass::CombinedShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

CombinedShaderClass::~CombinedShaderClass(void) 
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool CombinedShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	try
	{
		WCHAR* vsFilename = L"shaders/combinedVS.hlsl";
		WCHAR* psFilename = L"shaders/combinedPS.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, 
			pDeviceContext,
			hwnd, 
			vsFilename, 
			psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize the combined shader class");
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool CombinedShaderClass::Render(ID3D11DeviceContext* deviceContext, 
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const textureArray,
	const DirectX::XMFLOAT3 & cameraPosition,
	const LightClass* pLightSource)
	//DirectX::XMFLOAT4 diffuseColor,       // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	//DirectX::XMFLOAT3 lightDirection,     // a direction of the diffuse colour
	//DirectX::XMFLOAT4 ambientColor,       // a common colour for the scene
	//DirectX::XMFLOAT3 cameraPosition,     // the current position of the camera
	//DirectX::XMFLOAT4 specularColor,      // the specular colour is the reflected colour of the object's highlights
	//float specularPower)                  // specular intensity
{
	try
	{
		// set the shader parameters
		SetShaderParameters(deviceContext,
			world,
			view, projection,
			textureArray,
			cameraPosition,
			pLightSource);

		// render the model using this shader
		RenderShader(deviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render the model");
	}

	return true;
}


const std::string & CombinedShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
void CombinedShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	const UINT layoutElemNum = 3;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	HRESULT hr = S_OK;
	bool result = false;

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
	COM_ERROR_IF_FALSE(false, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(false, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(false, "can't initialize the sampler state");


	// ------------------------------- CONSTANT BUFFERS --------------------------------- //

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
} // InitializeShaders()



  // sets parameters for the HLSL shaders
void CombinedShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const textureArray,  // texture resources for the model
	const DirectX::XMFLOAT3 & cameraPosition,
	const LightClass* pLightSource)
{
	HRESULT hr = S_OK;
	bool result = false;
	UINT bufferPosition = 0;
	UINT texturesNumber = 3;

	// ---------------------------------------------------------------------------------- //
	//                         UPDATE THE VERTEX SHADER                                   //
	// ---------------------------------------------------------------------------------- //

	// update the matrix buffer
	matrixBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer position
	bufferPosition = 0;

	// set the matrix const buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, matrixBuffer_.GetAddressOf());



	// update the camera buffer
	cameraBuffer_.data.cameraPosition = cameraPosition;

	result = cameraBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

	// set the buffer position in the vertex shader
	bufferPosition = 1;  

	// set the camera const buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, cameraBuffer_.GetAddressOf());





	// ---------------------------------------------------------------------------------- //
	//                          UPDATE THE PIXEL SHADER                                   //
	// ---------------------------------------------------------------------------------- //

	// update the light const buffer
	lightBuffer_.data.diffuseColor = pLightSource->GetDiffuseColor();
	lightBuffer_.data.lightDirection = pLightSource->GetDirection();
	lightBuffer_.data.ambientColor = pLightSource->GetAmbientColor();
	lightBuffer_.data.specularColor = pLightSource->GetSpecularColor();
	lightBuffer_.data.specularPower = pLightSource->GetSpecularPower();

	result = lightBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

	// set the buffer position in the pixel shader
	bufferPosition = 0;

	// set the light const buffer for the HLSL pixel shader
	deviceContext->PSSetConstantBuffers(bufferPosition, 1, lightBuffer_.GetAddressOf());

	// set the shader resource (textures) for the pixel shader
	deviceContext->PSSetShaderResources(0, texturesNumber, &textureArray);


	return;
} // SetShaderParameters


  // sets stuff which we will use: layout, vertex and pixel shader, sampler state
  // and also renders our 3D model
void CombinedShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount)
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