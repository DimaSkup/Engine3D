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
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::LightShaderClass(void)
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
}

// Shuts down the vertex and pixel shaders, releases the memory from the related objects
void LightShaderClass::Shutdown(void)
{
	// Shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

// Sets the shader parameters for rendering. Renders the model
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	                          D3DXMATRIX worldMatrix,
	                          D3DXMATRIX viewMatrix,
	                          D3DXMATRIX projectionMatrix,
	                          ID3D11ShaderResourceView* texture,
	                          D3DXVECTOR4 diffuseColor,
	                          D3DXVECTOR3 lightDirection)
{
	bool result;

	// Set the shader parameters which are necessary for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix,
		                         texture, diffuseColor, lightDirection);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set shader parameters");
		return false;
	}

	// Render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

// ---------------------------------------------------------------------------------- //
//
//                           PRIVATE FUNCTIONS
//
// ---------------------------------------------------------------------------------- //

// Compiles a shader from file
HRESULT CompileShaderFromFile(WCHAR* filename, LPCSTR functionName, 
	                          LPCSTR shaderModel, ID3DBlob** shaderBlob)
{
	Log::Get()->Debug("%s::%d: compilation of %s:%s shader",
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
}

// Initializes the vertex and pixel shaders, vertex input layout, sampler state, 
// constant matrix buffer, and constant light buffer
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
	                                    WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* VSBuffer = nullptr;   // here we place the vertex shader bytecode
	ID3DBlob* PSBuffer = nullptr;   // here we place the pixel shader bytecode
	D3D11_INPUT_ELEMENT_DESC layoutDesc[3]; // setup of the vertex input layout
	D3D11_BUFFER_DESC matrixBufferDesc;     // description for the constant matrix buffer
	D3D11_BUFFER_DESC lightBufferDesc;      // description for the constant light buffer

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


	return true;
}