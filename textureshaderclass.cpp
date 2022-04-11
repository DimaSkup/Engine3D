////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pSampleState = nullptr;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass(void)
{
}

// ------------------------------------------------------------------------- //
//
//                        PUBLIC METHODS
//
// ------------------------------------------------------------------------- //

// Loads the texture HLSL files for this shader
bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShader(device, hwnd, L"texture.vs", L"texture.ps"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture shaders");
		return false;
	}
}

// Releases the shader variables
void TextureShaderClass::Shutdown(void)
{
	ShutdownShader();

	return;
}

// Sets variables are used inside the shaders and renders the model using these shaders. 
// Also this function takes a parameters called texture
// which is the pointer to the texture resource.
bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                                D3DXMATRIX worldMatrix, 
                                D3DXMATRIX viewMatrix, 
                                D3DXMATRIX projectionMatrix, 
                                ID3D11ShaderResourceView* texture)
{
	bool result;

	// Set the shaders parameters that will be used for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix,
		                         projectionMatrix, texture);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set texture shader parameters");
		return false;
	}

	// Now render the prepared buffers with the shaders
	RenderShader(deviceContext, indexCount);

	return true;
}

// ------------------------------------------------------------------------- //
//
//                        PRIVATE METHODS
//
// ------------------------------------------------------------------------- //

// Compiles a shader from a HLSL file
HRESULT CompileShaderFromFile(WCHAR* filename, 
                              LPCSTR functionName, 
                              LPCSTR shaderModel, 
                              ID3DBlob** shaderBlob)
{
	Log::Get()->Debug("%s::%d: compilation of %s:%s shader",
	                  __FUNCTION__, __LINE__, filename, functionName);

	HRESULT hr = S_OK;
	UINT compileFlags = D3D10_SHADER_WARNINGS_ARE_ERRORS | D3D10_SHADER_ENABLE_STRICTNESS;
	ID3DBlob* errorMessage = nullptr;

	hr = D3DX11CompileFromFile(filename, nullptr, 0,
		                       functionName, shaderModel,
                               compileFlags, 0, nullptr,
                               shaderBlob, &errorMessage, nullptr);

	// If the shader failed to compile it should have writen something to the error message
	if (errorMessage != nullptr)
	{
		Log::Get()->Error(THIS_FUNC, static_cast<char*>(errorMessage->GetBufferPointer()));
		_RELEASE(errorMessage);
	}

	return hr;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
	                                      WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* VSBuffer = nullptr;
	ID3DBlob* PSBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;	// a description of the constant buffer for the vertex shader
	D3D11_SAMPLER_DESC samplerDesc;

	// ------------------- COMPILATION AND CREATION OF SHADERS -------------------------- //

	// Compile the vertex shader code
	hr = CompileShaderFromFile(vsFilename, "TextureVertexShader", "vs_5_0", &VSBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader code");
	}

	// Create the vertex shader from the buffer
	hr = device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(),
		                            nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		_RELEASE(VSBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create the vertex shader");
		return false;
	}


	// Compile the pixel shader code
	hr = CompileShaderFromFile(psFilename, "TexturePixelShader", "ps_5_0", &PSBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader");
		return false;
	}

	// Create the pixel shader from the buffer
	hr = device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(),
		                           nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		_RELEASE(PSBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create the pixel shader");
		return false;
	}

	// Release the pixel shader buffer since it is no longer needed
	_RELEASE(PSBuffer);


	// ------------------ CREATION OF THE VERTEX INPUT LAYOUT ------------------- //
	// Create the vertex input layout description
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

	// Get a count of the elements in the layout
	numElements = ARRAYSIZE(polygonLayout);

	// Create the vertex input layout
	hr = device->CreateInputLayout(polygonLayout, numElements, 
		                           VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(),
		                           &m_pLayout);
	if (FAILED(hr))
	{
		_RELEASE(VSBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create the vertex input layout");
		return false;
	}

	// Release the vertex shader buffer since it is no longer needed
	_RELEASE(VSBuffer);


	// ----------------- CREATION OF A DYNAMIC MATRIX CONSTANT BUFFER ------------------ //
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant 
	// buffer from withing this class
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the matrix constant buffer");
		return false;
	}

	// ------------------- CREATION OF THE TEXTURE SAMPLER STATE -------------------- //
	// Create the texture sampler state description
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

	// Create the texture sampler state
	hr = device->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the texture sampler state");
		return false;
	}


	Log::Get()->Debug(THIS_FUNC, "shaders are initialized successfully");

	return true;
}