////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"

LightShaderClass::LightShaderClass(void)
{
}

// we don't use the copy constructor and destructor in this class
LightShaderClass::LightShaderClass(const LightShaderClass& anotherObj) {}
LightShaderClass::~LightShaderClass(void) {}

// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result = false;

	// try to initialize the vertex and pixel HLSL shaders
	result = InitializeShader(device, hwnd, 
		                      L"shaders/lightVertex.hlsl", L"shaders/lightPixel.hlsl");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize shaders");
		return false;
	}

	return true;
}

// Cleans up the memory after the shaders, layout, sampler state and buffers
void LightShaderClass::Shutdown(void)
{
	ShutdownShader();
	return;
}


// 1. Sets the parameters for HLSL shaders which are used for rendering
// 2. Renders the model using the HLSL shaders
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	                          DirectX::XMMATRIX world,
	                          DirectX::XMMATRIX view,
	                          DirectX::XMMATRIX projection,
	                          ID3D11ShaderResourceView* texture,    // a texture resource for the model
	                          DirectX::XMFLOAT4 diffuseColor,       // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	                          DirectX::XMFLOAT3 lightDirection,     // a direction of the diffuse colour
	                          DirectX::XMFLOAT4 ambientColor,       // a common colour for the scene
	                          DirectX::XMFLOAT3 cameraPosition,     // the current position of the camera
	                          DirectX::XMFLOAT4 specularColor,      // the specular colour is the reflected colour of the object's highlights
	                          float specularPower)                  // specular intensity
{
	bool result = false;
	
	// set the shader parameters
	result = SetShaderParameters(deviceContext,
		                         world, view, projection,
		                         texture,
		                         diffuseColor, lightDirection, ambientColor,
		                         cameraPosition, specularColor, specularPower);
	
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the shader parameters");
		return false;
	}


	// render the model
	RenderShader(deviceContext, indexCount);

	return true;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// compiles shader from shader file
HRESULT LightShaderClass::CompileShaderFromFile(WCHAR* filename, LPCSTR functionName,
	                                            LPCSTR shaderModel, ID3DBlob** shaderBlob)
{
	return ShaderClass::compileShaderFromFile(filename, functionName, shaderModel, shaderBlob);
}

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, 
	                                    WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	D3D11_INPUT_ELEMENT_DESC layoutDesc[3];
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


	// ---------------------------------------------------------------------------------- //
	//                    CREATION OF THE VERTEX AND PIXEL SHADERS                        //
	// ---------------------------------------------------------------------------------- //

	// compile and create the vertex shader
	hr = CompileShaderFromFile(vsFilename, "LightVertexShader", "vs_5_0", &vsBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader code");
		return false;
	}

	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
		                            vsBlob->GetBufferSize(),
		                            nullptr,
		                            &m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex shader");
		_RELEASE(vsBlob);
		return false;
	}


	// compile and create the pixel shader
	hr = CompileShaderFromFile(psFilename, "LightPixelShader", "ps_5_0", &psBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader code");
		return false;
	}

	hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
		                           psBlob->GetBufferSize(),
		                           nullptr,
		                           &m_pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the pixel shader");
		_RELEASE(psBlob);
		return false;
	}

	// ---------------------------------------------------------------------------------- //
	//                       CREATION OF THE INPUT LAYOUT                                 //
	// ---------------------------------------------------------------------------------- //

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

	UINT layoutElemNum = ARRAYSIZE(layoutDesc);

	// create the input layout
	hr = device->CreateInputLayout(layoutDesc, layoutElemNum, 
		                           vsBlob->GetBufferPointer(),
		                           vsBlob->GetBufferSize(),
		                           &m_pLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the input layout");
		return false;
	}

	if (vsBlob == nullptr || psBlob == nullptr)
		Log::Get()->Error("VS AND PS IS NOT NULL");

	// Releasing of the vertex and pixel buffers since they are no longer needed
	_RELEASE(vsBlob);
	_RELEASE(psBlob);

	// ---------------------------------------------------------------------------------- //
	//                        CREATION OF THE SAMPLER STATE                               //
	// ---------------------------------------------------------------------------------- //

	// create description for the sampler state which is used in the pixel HLSH shader
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MipLODBias = 0.0f;
	
	// create a sampler state using the description
	hr = device->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the sampler state");
		return false;
	}


	// ---------------------------------------------------------------------------------- //
	//                        CREATION OF CONSTANT BUFFERS                                //
	// ---------------------------------------------------------------------------------- //

	// ----------------- CREATION OF THE CONSTANT MATRIX BUFFER --------------------- //
	// create description for the constant matrix buffer which is used in the vertex HLSL shader
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.MiscFlags = 0;

	// create a constant matrix buffer using the description
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant matrix buffer");
		return false;
	}


	// ----------------- CREATION OF THE CONSTANT CAMERA BUFFER --------------------- //
	// create description for the constant camera buffer which is used in vertex HLSL shader
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.StructureByteStride = 0;
	cameraBufferDesc.MiscFlags = 0;

	// create a constant camera buffer using the description
	hr = device->CreateBuffer(&cameraBufferDesc, nullptr, &m_pCameraBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant camera buffer");
		return false;
	}


	// ----------------- CREATION OF THE CONSTANT LIGHT BUFFER --------------------- //
	// create description for the constant light buffer which is used in pixel HLSL shader
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.StructureByteStride = 0;
	lightBufferDesc.MiscFlags = 0;

	// create a constant light buffer using the description
	hr = device->CreateBuffer(&lightBufferDesc, nullptr, &m_pLightBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the  constant light buffer");
		return false;
	}

	return true;
} // InitializeShader

// helps to release the memory
void LightShaderClass::ShutdownShader(void)
{
	_RELEASE(m_pLightBuffer);
	_RELEASE(m_pCameraBuffer);
	_RELEASE(m_pMatrixBuffer);
	_RELEASE(m_pSampleState);
	_RELEASE(m_pLayout);
	_RELEASE(m_pPixelShader);
	_RELEASE(m_pVertexShader);
}


// sets parameters for the HLSL shaders
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	                                       DirectX::XMMATRIX worldMatrix,
	                                       DirectX::XMMATRIX viewMatrix,
	                                       DirectX::XMMATRIX projectionMatrix,
	                                       ID3D11ShaderResourceView* texture,
	                                       DirectX::XMFLOAT4 diffuseColor,
	                                       DirectX::XMFLOAT3 lightDirection,
	                                       DirectX::XMFLOAT4 ambientColor,
	                                       DirectX::XMFLOAT3 cameraPosition,
	                                       DirectX::XMFLOAT4 specularColor, float specularPower)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	MatrixBufferType* matrixDataPtr = nullptr;
	CameraBufferType* cameraDataPtr = nullptr;
	LightBufferType* lightDataPtr = nullptr;
	UINT bufferPosition = 0;

	// ---------------------------------------------------------------------------------- //
	//                     SETUP THE CONSTANT MATRIX BUFFER                               //
	// ---------------------------------------------------------------------------------- //
	// prepare matrices for using in the HLSL vertex buffer
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	// lock the constant matrix buffer
	hr = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant matrix buffer");
		return false;
	}

	// get a pointer to the data in the buffer
	matrixDataPtr = static_cast<MatrixBufferType*>(mappedData.pData);

	// write data into the matrix buffer
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;

	// unlock the constant matrix buffer
	deviceContext->Unmap(m_pMatrixBuffer, 0);

	// set the buffer position
	bufferPosition = 0;

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, &m_pMatrixBuffer);

	// set the shader resource for the vertex shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// clean the memory 
	//_DELETE(matrixDataPtr);


	// ---------------------------------------------------------------------------------- //
	//                     SETUP THE CONSTANT CAMERA BUFFER                               //
	// ---------------------------------------------------------------------------------- //
	// lock the constant camera buffer
	hr = deviceContext->Map(m_pCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant camera buffer");
		return false;
	}

	// get a pointer to the data in the buffer
	cameraDataPtr = static_cast<CameraBufferType*>(mappedData.pData);

	// write data into the camera buffer
	cameraDataPtr->cameraPosition = cameraPosition;
	cameraDataPtr->padding = 0.0f;

	// unlock the buffer
	deviceContext->Unmap(m_pCameraBuffer, 0);

	// set the buffer position in the vertex shader
	bufferPosition = 1;  // because the matrix buffer in zero position

	// set the buffer for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferPosition, 1, &m_pCameraBuffer);

	// clean the memory 
	//_DELETE(cameraDataPtr);

	// ---------------------------------------------------------------------------------- //
	//                     SETUP THE CONSTANT LIGHT BUFFER                                //
	// ---------------------------------------------------------------------------------- //

	// lock the constant light buffer
	hr = deviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant light buffer");
		return false;
	}

	// get a pointer to the data in the buffer
	lightDataPtr = static_cast<LightBufferType*>(mappedData.pData);

	// write data into the buffer
	lightDataPtr->diffuseColor = diffuseColor;
	lightDataPtr->lightDirection = lightDirection;
	lightDataPtr->ambientColor = ambientColor;
	lightDataPtr->specularColor = specularColor;
	lightDataPtr->specularPower = specularPower;

	// unlock the buffer
	deviceContext->Unmap(m_pLightBuffer, 0);

	// set the buffer position in the pixel shader
	bufferPosition = 0;

	// set the constant light buffer for the HLSL pixel shader
	deviceContext->PSSetConstantBuffers(bufferPosition, 1, &m_pLightBuffer);

	// clean the memory 
	//_DELETE(lightDataPtr);

	return true;
} // SetShaderParameters


// sets stuff which we will use: layout, vertex and pixel shader, sampler state
// and also renders our 3D model
void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set the input layout for the vertex shader
	deviceContext->IASetInputLayout(m_pLayout);

	// set shader which we will use for rendering
	deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// set the sampler state for the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader