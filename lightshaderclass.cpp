////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"

LightShaderClass::LightShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pSampleState = nullptr;

	m_pMatrixBuffer = nullptr;
	m_pCameraBuffer = nullptr;
	m_pLightBuffer = nullptr;
}

LightShaderClass::LightShaderClass(const LightShaderClass& anotherObj)
{
}

LightShaderClass::~LightShaderClass(void)
{
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //





// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //











































/*
////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"

LightShaderClass::LightShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pSampleState = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pLightBuffer = nullptr;
	m_pCameraBuffer = nullptr;
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass(void)
{
}

// ---------------------------------------------------------------------------------- //
//
//                           PUBLIC FUNCTIONS
//
// ---------------------------------------------------------------------------------- //

// Initializes the vertex and pixel shaders
bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// Initialize the vertex and pixel shaders
	if (!InitializeShader(device, hwnd, L"light.vs", L"light.ps"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize shaders");
	}

	return true;
} // Initialize

// Shuts down the vertex and pixel shaders, releases the memory from the related objects
void LightShaderClass::Shutdown(void)
{
	// Shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
} // Shutdown

// Sets the shader parameters for rendering. Renders the model
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	                          D3DXMATRIX worldMatrix,
	                          D3DXMATRIX viewMatrix,
	                          D3DXMATRIX projectionMatrix,
	                          ID3D11ShaderResourceView* texture,
	                          D3DXVECTOR4 diffuseColor,
	                          D3DXVECTOR3 lightDirection,
	                          D3DXVECTOR4 ambientColor,
	                          D3DXVECTOR3 cameraPosition,
	                          D3DXVECTOR4 specularColor,
	                          float specularPower)
{
	bool result;

	// Set the shader parameters which are necessary for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix,
		                         texture, diffuseColor, lightDirection, ambientColor,
		                         cameraPosition, specularColor, specularPower);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set shader parameters");
		return false;
	}

	// Render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
} // Render

// ---------------------------------------------------------------------------------- //
//
//                           PRIVATE FUNCTIONS
//
// ---------------------------------------------------------------------------------- //

// Compiles a shader from file
HRESULT LightShaderClass::CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, 
	                          LPCSTR shaderModel, ID3DBlob** shaderBlob)
{
	Log::Get()->Debug("%s::%d: compilation of %S:%s shader",
		              __FUNCTION__, __LINE__, filename, functionName);

	HRESULT hr = S_OK;
	ID3DBlob* errorMessage = nullptr;
	UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	hr = D3DX11CompileFromFile(filename, nullptr, NULL,
		                       functionName, shaderModel,
		                       shaderFlags, NULL, nullptr,
		                       shaderBlob, &errorMessage, nullptr);
	if (errorMessage != nullptr)
	{
		Log::Get()->Error(THIS_FUNC, static_cast<char*>(errorMessage->GetBufferPointer()));
		_RELEASE(errorMessage);
	}

	return hr;
} // CompileShaderFromFile

// Initializes the vertex and pixel shaders, vertex input layout, sampler state, 
// constant matrix buffer, and constant light buffer
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
	                                    WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* VSBuffer = nullptr;   // here we place the vertex shader bytecode
	ID3DBlob* PSBuffer = nullptr;   // here we place the pixel shader bytecode
	D3D11_INPUT_ELEMENT_DESC layoutDesc[3]; // setup of the vertex input layout
	UINT numElements = 0;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;     // description for the constant matrix buffer
	D3D11_BUFFER_DESC lightBufferDesc;      // description for the constant light buffer
	D3D11_BUFFER_DESC cameraBufferDesc;     // description for the constant camera buffer

	// ------------------ CREATION OF THE VERTEX AND PIXEL SHADERS ---------------------- //
	hr = CompileShaderFromFile(vsFilename, "LightVertexShader", "vs_5_0", &VSBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader");
		return false;
	}

	hr = device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(),
		                            nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		_RELEASE(VSBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create the vertex shader object");
		return false;
	}

	hr = CompileShaderFromFile(psFilename, "LightPixelShader", "ps_5_0", &PSBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader");
		return false;
	}

	hr = device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(),
		                           nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		_RELEASE(PSBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create the pixel object");
		return false;
	}

	// --------------------- CREATION OF THE VERTEX INPUT LAYOUT ---------------------- //
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

	numElements = ARRAYSIZE(layoutDesc);

	hr = device->CreateInputLayout(layoutDesc, numElements, 
		                           VSBuffer->GetBufferPointer(),
		                           VSBuffer->GetBufferSize(),
		                           &m_pLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex input layout");
		return false;
	}

	// Releasing of the vertex and pixel buffers since they are no longer needed
	_RELEASE(VSBuffer);
	_RELEASE(PSBuffer);


	// --------------- CREATION OF THE TEXTURE SAMPLER STATE ------------------------ //
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MinLOD = 0;
	samplerDesc.MipLODBias = 0.0f;

	hr = device->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the sampler state");
		return false;
	}

	// ----------------- CREATION OF THE CONSTANT MATRIX BUFFER --------------------- //
	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// create the matrix dynamic constant buffer
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant matrix buffer");
		return false;
	}


	// ----------------- CREATION OF THE CONSTANT CAMERA BUFFER --------------------- //
	// Setup the description of the camera dynamic constant buffer that is in the vertex shader
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant
	// buffer from within this class
	hr = device->CreateBuffer(&cameraBufferDesc, nullptr, &m_pCameraBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the camera buffer");
		return false;
	}


	// ----------------- CREATION OF THE CONSTANT LIGHT BUFFER --------------------- //
	// Setup the description of the light dynamic constant buffer that is in the pixel shader
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// create the light dynamic constant buffer
	hr = device->CreateBuffer(&lightBufferDesc, nullptr, &m_pLightBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant light buffer");
		return false;
	}

	return true;
} // InitializeShader

// Releases the memory from shaders, input layout, matrix buffer and light buffer
void LightShaderClass::ShutdownShader(void)
{
	_RELEASE(m_pLightBuffer);
	_RELEASE(m_pCameraBuffer);
	_RELEASE(m_pMatrixBuffer);
	_RELEASE(m_pSampleState);
	_RELEASE(m_pLayout);
	_RELEASE(m_pPixelShader);
	_RELEASE(m_pVertexShader);

	return;
}

// Sets parameters for shader buffers
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	                                       D3DXMATRIX worldMatrix,
	                                       D3DXMATRIX viewMatrix,
	                                       D3DXMATRIX projectionMatrix,
	                                       ID3D11ShaderResourceView* texture,
	                                       D3DXVECTOR4 diffuseColor,
	                                       D3DXVECTOR3 lightDirection,
	                                       D3DXVECTOR4 ambientColor,
	                                       D3DXVECTOR3 cameraPosition,
	                                       D3DXVECTOR4 specularColor,
	                                       float specularPower)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixDataPtr = nullptr;
	LightBufferType*  lightDataPtr = nullptr;
	CameraBufferType* cameraDataPtr = nullptr;
	UINT bufferNumber = 0;

	// ------------------- SETUP DATA IN THE CONSTANT MATRIX BUFFER --------------------- //
	// Prepare world, view, and projection matrices for shaders
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant matrix buffer
	hr = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant matrix buffer");
		return false;
	}

	// Get a pointer to the constant matrix buffer data
	matrixDataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

	// Setup data in the buffer
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;

	// Unlock the constant matrix buffer
	deviceContext->Unmap(m_pMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	// Set shader texture resource in the pixel buffer
	deviceContext->PSSetShaderResources(0, 1, &texture);


	// ------------------- SETUP DATA IN THE CONSTANT CAMERA BUFFER --------------------- //
	// Lock the camera constant buffer so it can be written to
	hr = deviceContext->Map(m_pCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map the camera buffer");
		return false;
	}

	// Get a pointer to the data in the constant camera buffer
	cameraDataPtr = static_cast<CameraBufferType*>(mappedResource.pData);

	// Copy the camera position into the constant buffer 
	cameraDataPtr->cameraPosition = cameraPosition;
	cameraDataPtr->padding = 0.0f;

	// Unlock the camera constant buffer
	deviceContext->Unmap(m_pCameraBuffer, 0);

	// Set the position of the camera constant buffer in the vertex shader
	// (it is equal to 1, because the matrix buffer has index 0)
	bufferNumber = 1;

	// Now set the camera constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pCameraBuffer);



	// ------------------- SETUP DATA IN THE CONSTANT LIGHT BUFFER --------------------- //
	// Lock the constant light buffer
	hr = deviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant light buffer");
		return false;
	}

	// Get a pointer to the data in the buffer
	lightDataPtr = static_cast<LightBufferType*>(mappedResource.pData);

	// Setup data in the buffer
	lightDataPtr->ambientColor = ambientColor;
	lightDataPtr->diffuseColor = diffuseColor;
	lightDataPtr->lightDirection = lightDirection;
	lightDataPtr->specularColor = specularColor;
	lightDataPtr->specularPower = specularPower;

	// Unlock the constant light buffer
	deviceContext->Unmap(m_pLightBuffer, 0);

	// Set the position of the constant buffer in the pixel shader
	bufferNumber = 0;

	// Set the constant buffer in the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pLightBuffer);

	return true;
} // SetShaderParameters


// Setup the parts of rendering pipeline.
// Renders the 3D model using HLSL shaders
void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(m_pLayout);

	// Set the vertex and pixel shaders that will be used to render this model
	deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// Set the sampler in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	// Render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader
*/