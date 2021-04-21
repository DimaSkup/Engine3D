//////////////////////////////////
// Filename: fontshaderclass.cpp
//////////////////////////////////
#include "fontshaderclass.h"

FontShaderClass::FontShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_constantBuffer = nullptr;
	m_sampleState = nullptr;
	m_pixelBuffer = nullptr;
}

FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}

FontShaderClass::~FontShaderClass()
{
}

//////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"../Engine/font.vs", L"../Engine/font.ps");
	if (!result)
	{
		return false;
	}

	return true;
}


void FontShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
							 D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
							 ID3D11ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	bool result;

	// Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, projectionMatrix, texture, pixelColor);

	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}



//////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////
bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;


	// Initialize the pointers this function will use to null
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;


	// Compile the vertex shader code
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, 
									"FontVertexShader", "vs_5_0",
									D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
									&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen
		// something to the error messag
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could
		// not find the shader file itself
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}


	// Compile the pixel shader code
	result = D3DX11CompileFromFile(psFilename, NULL, NULL,
									"FontPixelShader", "ps_5_0", 
									D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
									&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen
		// something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply 
		// could not find the file itself
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(),
										NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
										pixelShaderBuffer->GetBufferSize(),
										NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}


	// Create the vertex input layout description
	// This setup need to match the VertexType structure in the 
	// ModelClass and in the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA:
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex  input layout
	result = device->CreateInputLayout(polygonLayout, numElements, 
										vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(),
										&m_layout);
}		