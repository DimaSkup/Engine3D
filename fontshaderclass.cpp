////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"

FontShaderClass::FontShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pPixelBuffer = nullptr;
	m_pSampleState = nullptr;
}

FontShaderClass::FontShaderClass(const FontShaderClass& copy)
{
}

FontShaderClass::~FontShaderClass(void)
{
}


// --------------------------------------------------------------------------------- // 
//
//                            PUBLIC FUNCTIONS
//
// --------------------------------------------------------------------------------- // 

// Initialize() loads the new font vertex shader and pixel shader HLSL files
bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result = false;

	// Initialize the vertex and pixel shaders 
	result = InitializeShader(device, hwnd, L"shaders/font.vs", L"shaders/font.ps");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the shaders");
		return false;
	}

	Log::Get()->Debug(THIS_FUNC, "is initialized()");

	return true;
}

// Shutdown() calls ShutdownShader() which releases the font shader related pointers and data
void FontShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

// Render() will set the shader parameters and then draw the buffers using the font shader.
bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	                         DirectX::XMMATRIX worldMatrix, 
	                         DirectX::XMMATRIX viewMatrix, 
	                         DirectX::XMMATRIX orthoMatrix,
	                         ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 pixelColor)
{
	bool result = false;

	// set the shader paremeters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, orthoMatrix,
		                         texture, pixelColor);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set shader parameters");
		return false;
	}

	// now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
} // Render()

// --------------------------------------------------------------------------------- // 
//
//                            PRIVATE FUNCTIONS
//
// --------------------------------------------------------------------------------- //

// InitializeShader() loads the new HLSL font vertex and pixel shaders as well 
// as the pointers that interface with the shader
bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, 
	                                   WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* vsBuffer = nullptr;
	ID3DBlob* psBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements = 0;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// --------------------------- COMPILE SHADERS ------------------------------------ //


	// compile the vertex shader code
	hr = compileShaderFromFile(vsFilename, "FontVertexShader", "vs_5_0", &vsBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader");
		return false;
	}



	// compile the pixel shader code
	hr = compileShaderFromFile(psFilename, "FontPixelShader", "ps_5_0", &psBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader");
		return false;
	}


	// create the vertex shader from the buffer
	hr = device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),
		                            nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex shader");
		//_RELEASE(vsBuffer);
		//_RELEASE(psBuffer);
		return false;
	}

	// create the pixel shader from the buffer
	hr = device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(),
		                           nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the pixel shader");
		//_RELEASE(vsBuffer);
		//_RELEASE(psBuffer);
		return false;
	}


	// ---------------------------- VERTEX INPUT LAYOUT --------------------------------//

	// create the vertex input layout descripton;
	// this setup needs to match the VERTEX structure in the FontClass and in the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// get a count of the elements in the layout
	numElements = ARRAYSIZE(polygonLayout);

	// create the vertex input layout
	hr = device->CreateInputLayout(polygonLayout, numElements, vsBuffer->GetBufferPointer(),
		                           vsBuffer->GetBufferSize(), &m_pLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex input layout");
		//_RELEASE(vsBuffer);
		//_RELEASE(psBuffer);
		return false;
	}

	// release the vertex shader buffer and pixel shader buffer since they are no longer needed
	_RELEASE(vsBuffer);
	_RELEASE(psBuffer);



	// ------------------------------ MATRIX BUFFER  ----------------------------------//

	// setup the description of the dynamic constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// create the constant matrix buffer pointer so we can access the vertex shader 
	// constant buffer from within this class
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant matrix buffer");
		return false;
	}



	// ------------------------------- PIXEL BUFFER -------------------------------------//

	// setup the description of the dynamic pixel constant buffer that is in the pixel shader
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// create the pixel constant buffer pointer so we can access the pixel shader 
	// constant buffer from withing this class
	hr = device->CreateBuffer(&pixelBufferDesc, nullptr, &m_pPixelBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant pixel buffer");
		return false;
	}


	// ------------------------------- SAMPLER STATE -------------------------------------//

	// create a texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// create the texture sampler state
	hr = device->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the sampler state");
		return false;
	}

	return true;
} // InitializeShader()

// ShutdownShader() releases all the shader related data
void FontShaderClass::ShutdownShader(void)
{
	_RELEASE(m_pSampleState);   // release the sampler state
	_RELEASE(m_pPixelBuffer);   // release the constant buffer of pixel
	_RELEASE(m_pMatrixBuffer);  // release the constant buffer of matrices
	_RELEASE(m_pLayout);        // release the input layout
	_RELEASE(m_pPixelShader);   // release the pixel shader
	_RELEASE(m_pVertexShader);  // release the vertex shader
}


// The SetShaderParameters() sets all the shader variables before rendering
bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	                                      DirectX::XMMATRIX worldMatrix,
	                                      DirectX::XMMATRIX viewMatrix,
	                                      DirectX::XMMATRIX orthoMatrix,
	                                      ID3D11ShaderResourceView* texture,
	                                      DirectX::XMFLOAT4 pixelColor)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixDataPtr = nullptr;
	PixelBufferType* pixelDataPtr = nullptr;
	unsigned int bufferNumber = 0;

	// ------------------------- VERTEX SHADER PARAMS ------------------------------ //

	// lock the matrix constant buffer so it can be written to
	hr = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the matrix buffer");
		return false;
	}

	// get a pointer to the data in the matrix buffer
	matrixDataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

	// transpose the matrices to prepare them for the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix  = XMMatrixTranspose(viewMatrix);
	orthoMatrix = XMMatrixTranspose(orthoMatrix);

	// copy matrices into the matrix constant buffer
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->ortho = orthoMatrix;

	// unlock the matrix constant buffer
	deviceContext->Unmap(m_pMatrixBuffer, 0);

	// set the position of the matrix constant buffer in the vertex shader
	bufferNumber = 0;

	// now set the matrix constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	// set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);


	// ------------------------- PIXEL SHADER PARAMS ------------------------------ //

	// lock the pixel constant buffer so it can be written to
	hr = deviceContext->Map(m_pPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the pixel buffer");
		return false;
	}

	// get a pointer to the data in the pixel buffer
	pixelDataPtr = static_cast<PixelBufferType*>(mappedResource.pData);

	// copy the pixel colour into the pixel buffer
	pixelDataPtr->pixelColor = pixelColor;

	// unlock the pixel constant buffer
	deviceContext->Unmap(m_pPixelBuffer, 0);

	// set the position of the pixel constant buffer in the pixel shader
	bufferNumber = 0;

	// now set the pixel constant buffer in the pixel shader with the updated value
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pPixelBuffer);

	return true;
} // SetShaderParameters()


// RenderShader() draws the prepared font vertex/index buffers using the font shader
void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set the vertex input layout
	deviceContext->IASetInputLayout(m_pLayout);

	// set the vertex and pixel shader that will be used for rendering
	deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	// render the triangles
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

// compiles a shader code from file and returns its byte-code
HRESULT FontShaderClass::compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
	                                           LPCSTR shaderModel, ID3DBlob** shaderOutput)
{
	return ShaderClass::compileShaderFromFile(filename, functionName, shaderModel, shaderOutput);
}

