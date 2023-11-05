/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"


// -------------------------------------------------------------------- //
//                                                                      //
//                      PUBLIC FUNCTIONS                                //
//                                                                      //
// -------------------------------------------------------------------- // 

ColorShaderClass::ColorShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pMatrixBuffer = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass(void)
{
}

// This function calls the initialization function for the shaders
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// try to initialize the vertex and pixel shaders
	if (!InitializeShader(device, hwnd, L"./color.vs", L"./color.ps"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the vertex and pixel shaders");
		return false;
	}

	Log::Get()->Debug(THIS_FUNC, "shaders are initialized successfully");

	return true;
}

// This function calls the shutdown of the shader
void ColorShaderClass::Shutdown(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
	// Shutdown the vertex and pixel shaders and other related objects
	ShutdownShader();

	return;
}

// Will first set the parameters inside the shader using the SetShaderParameters function.
// Once the parameters are set it then calls RenderShader to draw model using the HLSH shader
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, 
								int indexCount,
								D3DXMATRIX worldMatrix, 
								D3DXMATRIX viewMatrix, 
								D3DXMATRIX projectionMatrix)
{

	// Set the shader parameters that it will use for rendering 
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
	{
		Log::Get()->Error(THIS_FUNC, "can't set the parameters for shader");
		return false;
	}

	// render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}


// -------------------------------------------------------------------- //
//                                                                      //
//                      PRIVATE FUNCTIONS                               //
//                                                                      //
// -------------------------------------------------------------------- // 

// here is where we compile the shader programs into buffers
HRESULT ColorShaderClass::CompileShaderFromFile(WCHAR* fileName, 
												LPCSTR functionName,
												LPCSTR shaderModel, 
												ID3D10Blob** shaderBuffer)
{
	Log::Get()->Debug("ColorShaderClass::CompileShaderFromFile(): "
						"\"%s:%s\"()", 
						fileName, functionName);
	HRESULT hr = S_OK;
	ID3D10Blob* errorMessage = nullptr;
	UINT shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_WARNINGS_ARE_ERRORS;

	hr = D3DX11CompileFromFile(fileName, nullptr, NULL, functionName, shaderModel,
		shaderFlags, 0, nullptr, shaderBuffer, &errorMessage, nullptr);
	if (errorMessage != nullptr)
	{
		// If the shader failed to compile it should have writen something to the error message
		
		Log::Get()->Error("ColorShaderClass::CompileShaderFromFile(): %s", 
							(char*)(errorMessage->GetBufferPointer()));

		_RELEASE(errorMessage);

	}

	return hr;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, 
										WCHAR* vsFilename, WCHAR* psFilename)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	HRESULT hr;

	ID3DBlob* vertexShaderBuffer = nullptr;
	ID3DBlob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	UINT numElements = 0;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// -------------------------------------------------------------------------- //
	//                       CREATION OF THE SHADERS                              //
	// -------------------------------------------------------------------------- //

	// compile the vertex shader code
	hr = CompileShaderFromFile(vsFilename, "ColorVertexShader", "vs_5_0", 
								&vertexShaderBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader");
		return false;
	}
	Log::Get()->Debug(THIS_FUNC, "vertex shader is compiled successfully");

	// compile the pixel shader code
	hr = CompileShaderFromFile(psFilename, "ColorPixelShader", "ps_5_0",
								&pixelShaderBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader");
		return false;
	}
	Log::Get()->Debug(THIS_FUNC, "pixel shader is compiled successfully");
	

	// Create the vertex shader from the buffer
	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), 
									vertexShaderBuffer->GetBufferSize(),
									nullptr,
									&m_pVertexShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex shader object");
		return false;
	}

	// Create the pixel shader from the buffer
	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
									pixelShaderBuffer->GetBufferSize(),
									nullptr,
									&m_pPixelShader);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the pixel shader object");
		return false;
	}

	


	// -------------------------------------------------------------------------- //
	//                      CREATION OF THE INPUT LAYOUT                          //
	// -------------------------------------------------------------------------- //

	// Now setup the layout of the data that goes into the shader
	// This setup needs to match the VERTEX structure in the ModelClass and in the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout
	numElements = ARRAYSIZE(polygonLayout);

	// Create the vertex input layout
	hr = device->CreateInputLayout(polygonLayout, numElements,
									vertexShaderBuffer->GetBufferPointer(),
									vertexShaderBuffer->GetBufferSize(),
									&m_pLayout);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex input layout");
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed
	_RELEASE(vertexShaderBuffer);
	_RELEASE(pixelShaderBuffer);





	// -------------------------------------------------------------------------- //
	//              CREATION OF THE DYNAMIC MATRIX CONSTANT BUFFER                //
	// -------------------------------------------------------------------------- //

	// Setup the description of the dynamic matrix constant buffer 
	// that is in the vertex buffer
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex constant buffer
	// from within this class
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the matrix constant buffer");
		return false;
	}

	return true;
} // InitializeShader

// Releases the interfaces that were setup in the InitilizeShader function
void ColorShaderClass::ShutdownShader(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	_RELEASE(m_pMatrixBuffer);	// release the matrix constant buffer
	_RELEASE(m_pLayout);		// release the input vertex layout
	_RELEASE(m_pPixelShader);	// release the pixel shader
	_RELEASE(m_pVertexShader);	// release the vertex shader

	return;
} // ShutdownShader


// Makes setting the global variables in the shader
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
											D3DXMATRIX worldMatrix,
											D3DXMATRIX viewMatrix,
											D3DXMATRIX projectionMatrix)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to
	hr = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant buffer");
		return false;
	}

	// Get a pointer to the data in the constant buffer
	dataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

	// Copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer
	deviceContext->Unmap(m_pMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	deviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	return true;
} // SetShaderParameters


// 1. Sets our input layout to active in the input assembler
// 2. Sets the vertex shader and pixel shader to render our vertex buffer
// 3. Calls the DrawIndexed function to render the green triangle
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(m_pLayout);

	// Set the vertex and pixel shaders that will be used to render this model
	deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
} // RenderShader