////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"

FontShaderClass::FontShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayout = nullptr;
	m_pConstantBuffer = nullptr;
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
	                         D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	                         ID3D11ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	bool result = false;

	// set the shader paremeters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
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
	UINT numElements = 0;
	D3D11_BUFFER_DESC constantBufferDesc;
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

}

// compiles a shader code from file and returns its byte-code
HRESULT FontShaderClass::compileShaderFromFile(WCHAR* filename, LPCSTR functionName,
	                                           LPCSTR shaderModel, ID3DBlob** shaderOutput)
{
	return ShaderClass::compileShaderFromFile(filename, functionName, shaderModel, shaderOutput);
}

