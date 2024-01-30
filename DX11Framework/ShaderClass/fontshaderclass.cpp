/////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
/////////////////////////////////////////////////////////////////////
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


// --------------------------------------------------------------- //
//
//                      PUBLIC FUNCTIONS
//
// --------------------------------------------------------------- //

// Initialize() initializes the vertex and pixel shaders, input layout,
// sampler state, matrix and pixel buffers
bool FontShaderClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/fontVertex.hlsl";
		const WCHAR* psFilename = L"shaders/fontPixel.hlsl";

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
} // Initialize()


// Render() renders fonts on the screen using HLSL shaders
bool FontShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	                         DataContainerForShaders* pDataForShader)
{
	try
	{
		// set up parameters for the vertex and pixel shaders
		SetShaderParameters(pDeviceContext, pDataForShader);

		// render fonts on the screen using HLSL shaders
		RenderShaders(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render using the shader");
		return false;
	}

	return true;
} // Render()


const std::string & FontShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}


// --------------------------------------------------------------- //
//
//                      PRIVATE FUNCTIONS
//
// --------------------------------------------------------------- //

// InitializeShaders() helps to initialize the vertex and pixel shaders,
// input layout, sampler state, matrix and pixel buffers
void FontShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext, 
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	bool result = false;
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 2;

	// a description of the vertex input layout
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; 

	// ------------------------------- INPUT LAYOUT DESC -------------------------------- //

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



	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");
	
	
	// initialize the pixel buffer
	hr = this->pixelBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the pixel buffer");
	

	return;
} // InitializeShaders()


// SetShaderParameters() sets up parameters for the vertex and pixel shaders
void FontShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
	const DataContainerForShaders* pDataForShader)
{
	bool result = false;
	HRESULT hr = S_OK;


	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the vertex shader
	// (the WVO matrix is already transposed)
	matrixBuffer_.data.worldViewProj = pDataForShader->WVO;  

	// update the constant matrix buffer
	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set up parameters for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_.GetAddressOf());



	// ---------------------------------------------------------------------------------- //
	//                      PIXEL SHADER: UPDATE THE TEXT COLOR                           //
	// ---------------------------------------------------------------------------------- //

	// prepare data for the pixel shader
	pixelBuffer_.data.pixelColor = pDataForShader->color;

	// update the constant pixel buffer
	result = pixelBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the pixel buffer");

	// set up parameters for the pixel shader
	pDeviceContext->PSSetConstantBuffers(0, 1, pixelBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                          PIXEL SHADER: SET TEXTURES                                //
	// ---------------------------------------------------------------------------------- //

	try
	{
		pDeviceContext->PSSetShaderResources(0, 1, pDataForShader->texturesMap.at("diffuse"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}

	return;
} // SetShaderParameters()


// RenderShaders() renders fonts on the screen using HLSL shaders
void FontShaderClass::RenderShaders(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
{
	// set vertex and pixel shaders for rendering
	pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state for the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, this->samplerState_.GetAddressOf());

	// set the input layout 
	pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// render the fonts on the screen
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
