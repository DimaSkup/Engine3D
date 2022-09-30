/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pMatrixBuffer = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& another)
{
}

ColorShaderClass::~ColorShaderClass(void)
{
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the ColorShaderClass
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShader(device, hwnd, L"color.vs", L"color.ps"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize shaders");
		return false;
	}

	Log::Get()->Debug(THIS_FUNC, "ColorShaderClass is initialized successfully");
	return true;
}

// Releases the memory
void ColorShaderClass::Shutdown(void)
{
	ShutdownShader();

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Sets shaders parameters and renders our 3D model using HLSL shaders
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	                          DirectX::XMMATRIX worldMatrix,
	                          DirectX::XMMATRIX viewMatrix,
	                          DirectX::XMMATRIX projectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set shader parameters");
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

// ------------------------------------------------------------------------------ //
//
//                         PRIVATE FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the shaders, input vertex layout and constant matrix buffer.
// This function is called from the Initialize() function
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
	                                    WCHAR* vertexShaderFilename,
                                        WCHAR* pixelShaderFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* VSBlob = nullptr;        // here we put a vertex shader byte code
	ID3DBlob* PSBlob = nullptr;        // here we put a pixel shader byte code


	// ----------------------- COMPILE SHADERS --------------------------------- //
	hr = CompileShaderFromFile(vertexShaderFilename, "ColorVertexShader", "vs_5_0", &VSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader");
		return false;
	}

	hr = CompileShaderFromFile(pixelShaderFilename, "ColorPixelShader", "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader");
		return false;
	}

	Log::Get()->Debug(THIS_FUNC, "shaders are compiled successfully");


	// ------------------- INCAPSULATE SHADERS INTO OBJECTS --------------------- //
	hr = device->CreateVertexShader(VSBlob->GetBufferPointer(),
                                    VSBlob->GetBufferSize(),
                                    nullptr,
                                    &m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex object");
		return false;
	}

	hr = device->CreatePixelShader(PSBlob->GetBufferPointer(),
                                   PSBlob->GetBufferSize(),
                                   nullptr,
                                   &m_pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the pixel object");
		return false;
	}


	// -------------------- CREATE INPUT VERTEX LAYOUT ---------------------------- //
	D3D11_INPUT_ELEMENT_DESC layoutDesc[2];

	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	layoutDesc[1].SemanticName = "COLOR";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;

	hr = device->CreateInputLayout(layoutDesc, 2, 
                                   VSBlob->GetBufferPointer(), 
                                   VSBlob->GetBufferSize(),
                                   &m_pLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex input layout");
		return false;
	}

	// ----------------- CREATION OF A CONSTANT MATRIX SHADER BUFFER ------------------- //
	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.MiscFlags = 0;

	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant matrix shader buffer");
		return false;
	}

	// release the memory from Blob objects
	_RELEASE(PSBlob);
	_RELEASE(VSBlob);

	Log::Get()->Debug(THIS_FUNC, "shaders are initialized successfully");
	return true;
}

// Releases the memory from shader interfaces, input layout, matrices buffer
// This function is called from the Shutdown() function
void ColorShaderClass::ShutdownShader(void)
{
	_RELEASE(m_pMatrixBuffer);
	_RELEASE(m_pLayout);
	_RELEASE(m_pPixelShader);
	_RELEASE(m_pVertexShader);

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Setup parameters of shaders
// This function is called from the Render() function
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	                                       DirectX::XMMATRIX worldMatrix,
	                                       DirectX::XMMATRIX viewMatrix,
	                                       DirectX::XMMATRIX projectionMatrix)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	MatrixBufferType* dataPtr = nullptr;

	// Transpose matrices to prepare them for the shader
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	// Transform the world matrix
	static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	t = (dwTimeCur - dwTimeStart) / 100.0f;

	worldMatrix = DirectX::XMMatrixRotationZ(t);

	// Lock the constant buffer so it can be written to
	hr = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant matrix buffer");
		return false;
	}

	// Get a pointer to the data in the constant buffer
	dataPtr = static_cast<MatrixBufferType*>(mappedData.pData);

	// Setup constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer
	deviceContext->Unmap(m_pMatrixBuffer, 0);

	return true;
}

// Sets as active the vertex and pixel shader, input vertex layout and matrix buffer
// Renders the model
// This function is called from the Render() function
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set shaders which will be used to render the model
	deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// set the format of input shader data
	deviceContext->IASetInputLayout(m_pLayout);

	// set the input shader data (constant buffer)
	deviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);	

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

// compiles shader from file
HRESULT ColorShaderClass::CompileShaderFromFile(WCHAR* fileName,
                                                LPCSTR functionName,
                                                LPCSTR shaderModel,
                                                ID3DBlob** shaderBlob)
{
	Log::Get()->Debug("%s()::%d: compilation of %s:%s", 
                      __FUNCTION__, __LINE__, fileName, functionName);
	HRESULT hr = S_OK;
	ID3DBlob* errorMessage = nullptr;
	UINT flags = D3D10_SHADER_WARNINGS_ARE_ERRORS | D3D10_SHADER_ENABLE_STRICTNESS;

	hr = D3DX11CompileFromFile(fileName, nullptr, NULL,
                               functionName, shaderModel, 
                               flags, NULL, nullptr,
                               shaderBlob, &errorMessage, nullptr);
	if (errorMessage != nullptr)
	{
		Log::Get()->Error(THIS_FUNC, static_cast<char*>(errorMessage->GetBufferPointer()));
		_RELEASE(errorMessage);
	}

	return hr;
}