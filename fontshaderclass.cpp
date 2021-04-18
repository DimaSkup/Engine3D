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
}