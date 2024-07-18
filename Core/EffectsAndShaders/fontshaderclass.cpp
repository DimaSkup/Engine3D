// ***********************************************************************************
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
// ***********************************************************************************
#include "fontshaderclass.h"

// initialize some internal variables 
FontShaderClass::FontShaderClass() : className_(__func__)
{
	Log::Debug(LOG_MACRO);
}

FontShaderClass::~FontShaderClass() 
{
}


// ***********************************************************************************
//
//                              PUBLIC FUNCTIONS
//
// ***********************************************************************************


bool FontShaderClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext)
{
	// Initialize() initializes the vertex and pixel shaders, input layout,
	// sampler state, matrix and pixel buffers

	try
	{
		const WCHAR* vsFilename = L"shaders/fontVS.hlsl";
		const WCHAR* psFilename = L"shaders/fontPS.hlsl";

		// create shader objects, buffers, etc.
		InitializeShaders(pDevice,
			pDeviceContext,
			vsFilename,
			psFilename);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the font shader class");
		return false;
	}

	return true;
}


// ************************************************************************************
//                             PUBLIC RENDERING API
// ************************************************************************************

void FontShaderClass::Prepare(ID3D11DeviceContext* pDeviceContext)
{
	// THIS FUNC prepares the Input Assember (IA) stage for rendering with this shader;

	// set vertex and pixel shaders for rendering
	pDeviceContext->VSSetShader(vs_.GetShader(), nullptr, 0U);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0U);

	// set the sampler state for the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// set the input layout 
	pDeviceContext->IASetInputLayout(vs_.GetInputLayout());
}

///////////////////////////////////////////////////////////

void FontShaderClass::Render(
	ID3D11DeviceContext* pDeviceContext, 
	const UINT indexCount)
{
	// THIS FUNC renders fonts on the screen using HLSL shaders

	try
	{
		// render the fonts on the screen
		pDeviceContext->DrawIndexed(indexCount, 0, 0);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't render using the shader");
	}
}



// ************************************************************************************
//                             PUBLIC MODIFICATION API
// ************************************************************************************

void FontShaderClass::SetWorldViewOrtho(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& WVO)
{
	// prepare matrices for using in the vertex shader
	// (the WVO matrix must be already transposed)
	matrixBuffer_.data.worldViewProj = WVO;

	// load matrices data into GPU
	matrixBuffer_.ApplyChanges(pDeviceContext);

	// set up parameters for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());
}

///////////////////////////////////////////////////////////

void FontShaderClass::SetFontColor(
	ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMFLOAT3& textColor)
{
	// prepare data for the pixel shader
	pixelBuffer_.data.pixelColor = textColor;

	// load the pixel color data into GPU
	pixelBuffer_.ApplyChanges(pDeviceContext);

	// set up parameters for the pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, pixelBuffer_.GetAddressOf());

}

///////////////////////////////////////////////////////////

void FontShaderClass::SetFontTexture(
	ID3D11DeviceContext* pDeviceContext,
	ID3D11ShaderResourceView* const* ppFontTexture)
{
	pDeviceContext->PSSetShaderResources(0, 1, ppFontTexture);
}




// ***********************************************************************************
//
//                              PRIVATE FUNCTIONS
//
// ***********************************************************************************

void FontShaderClass::InitializeShaders(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext, 
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	// InitializeShaders() helps to initialize the vertex and pixel shaders,
	// input layout, sampler state, matrix and pixel buffers

	bool result = false;
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 2;

	// a description of the vertex input layout
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; 

	// --------------------------  INPUT LAYOUT DESC  ---------------------------------

	// setup description of the vertex input layout 
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
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


	// -----------------------  SHADERS / SAMPLER STATE  ------------------------------

	// initialize the vertex shader
	result = vs_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	ASSERT_TRUE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	ASSERT_TRUE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = samplerState_.Initialize(pDevice);
	ASSERT_TRUE(result, "can't initialize the sampler state");



	// ---------------------------  CONSTANT BUFFERS  ---------------------------------

	// initialize the matrix buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the matrix buffer");
	
	// initialize the pixel buffer
	hr = pixelBuffer_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the pixel buffer");
	

	// ---------------- SET DEFAULT PARAMS FOR CONST BUFFERS --------------------------

	SetFontColor(pDeviceContext, { 1, 1, 1 });  // set white colour by default
	SetWorldViewOrtho(pDeviceContext, DirectX::XMMatrixIdentity());

	return;
}
