/////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
/////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"

// initialize some internal variables 
FontShaderClass::FontShaderClass()
{
	Log::Debug(THIS_FUNC_EMPTY);
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
		Log::Error(THIS_FUNC, "can't initialize the font shader class");
		return false;
	}

	return true;
} // Initialize()


// Render() renders fonts on the screen using HLSL shaders
bool FontShaderClass::Render(ID3D11DeviceContext* pDeviceContext, 
	UINT indexCount,
	const DirectX::XMMATRIX & world, 
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & ortho,
	ID3D11ShaderResourceView* const texture, 
	const DirectX::XMFLOAT4 & textColor)
{
	try
	{
		// set up parameters for the vertex and pixel shaders
		SetShaderParameters(pDeviceContext,
			world, 
			view, 
			ortho, 
			texture,
			textColor);

		// render fonts on the screen using HLSL shaders
		RenderShaders(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render using the shader");
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
void FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const DirectX::XMMATRIX & world, 
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & ortho,
	ID3D11ShaderResourceView* const texture,
	const DirectX::XMFLOAT4 & pixelColor)
{
	bool result = false;
	HRESULT hr = S_OK;
	UINT bufferNumber = 0;


	// ---------------- SET PARAMS FOR THE VERTEX SHADER ------------------- //

	// prepare matrices for using in the vertex shader
	matrixBuffer_.data.world = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view  = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.ortho = DirectX::XMMatrixTranspose(ortho);

	// update the constant matrix buffer
	result = matrixBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the number of the buffer in the vertex shader
	bufferNumber = 0;

	// set up parameters for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, matrixBuffer_.GetAddressOf());



	// ---------------- SET PARAMS FOR THE PIXEL SHADER -------------------- //

	// prepare data for the pixel shader
	pixelBuffer_.data.pixelColor = pixelColor;

	// update the constant pixel buffer
	result = pixelBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the pixel buffer");


	// set the number of the buffer in the pixel shader
	bufferNumber = 0;

	// set up parameters for the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, pixelBuffer_.GetAddressOf());
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, this->samplerState_.GetAddressOf());


	return;
} // SetShaderParameters()


// RenderShaders() renders fonts on the screen using HLSL shaders
void FontShaderClass::RenderShaders(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
{
	// set vertex and pixel shaders for rendering
	pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the input layout 
	pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// render the fonts on the screen
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}