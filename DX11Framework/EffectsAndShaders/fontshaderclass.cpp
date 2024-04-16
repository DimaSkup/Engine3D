// ***********************************************************************************
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
// ***********************************************************************************
#include "fontshaderclass.h"

// initialize some internal variables 
FontShaderClass::FontShaderClass()
{
	Log::Debug(LOG_MACRO);
	className_ = __func__;
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
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the font shader class");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void FontShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & WVO,               // world * basic_view * ortho
	const DirectX::XMFLOAT3 & textColor,
	ID3D11ShaderResourceView* const* ppTexture)
{
	// THIS FUNC sets up parameters for the vertex and pixel shaders

	bool result = false;

	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE MATRIX BUFFER
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the vertex shader
	// (the WVO matrix is already transposed)
	matrixBuffer_.data.worldViewProj = WVO;

	// update the constant matrix buffer
	result = matrixBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set up parameters for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                      PIXEL SHADER: UPDATE THE TEXT COLOR
	// ---------------------------------------------------------------------------------- //

	// prepare data for the pixel shader
	pixelBuffer_.data.pixelColor = textColor;

	// update the constant pixel buffer
	result = pixelBuffer_.ApplyChanges(pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't update the pixel buffer");

	// set up parameters for the pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, pixelBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                          PIXEL SHADER: SET TEXTURES
	// ---------------------------------------------------------------------------------- //

	pDeviceContext->PSSetShaderResources(0, 1, ppTexture);


	return;
} 

///////////////////////////////////////////////////////////

void FontShaderClass::Render(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
{
	// THIS FUNC renders fonts on the screen using HLSL shaders

	try
	{
		// set vertex and pixel shaders for rendering
		pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
		pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

		// set the sampler state for the pixel shader
		pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

		// set the input layout 
		pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

		// render the fonts on the screen
		pDeviceContext->DrawIndexed(indexCount, 0, 0);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't render using the shader");
	}

	return;
}

///////////////////////////////////////////////////////////

const std::string & FontShaderClass::GetShaderName() const
{
	return className_;
}


// ***********************************************************************************
//
//                            PRIVATE FUNCTIONS
//
// ***********************************************************************************

void FontShaderClass::InitializeShaders(ID3D11Device* pDevice,
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

	// ------------------------- INPUT LAYOUT DESC ---------------------------- //

	// setup description of the vertex input layout 
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


	// ----------------------- SHADERS / SAMPLER STATE ------------------------ //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ------------------------- CONSTANT BUFFERS ----------------------------- //

	// initialize the matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");
	
	// initialize the pixel buffer
	hr = this->pixelBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the pixel buffer");
	

	return;
}
