/////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
/////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"

// initialize some internal variables 
FontShaderClass::FontShaderClass(void)
{
}

// we don't use copy constructor and destructor in this class
FontShaderClass::FontShaderClass(const FontShaderClass& copy) {}
FontShaderClass::~FontShaderClass(void) {}


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
	bool result = false;

	// create shader objects, buffers, etc.
	result = InitializeShaders(pDevice, 
							   pDeviceContext, 
							   hwnd,
							   L"shaders/fontVertex.hlsl", 
							   L"shaders/fontPixel.hlsl");
	COM_ERROR_IF_FALSE(result, "can't initialize shaders");


	Log::Debug(THIS_FUNC, "is initialized");

	return true;
} // Initialize()


// Render() renders fonts on the screen using HLSL shaders
bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, 
							 int indexCount,
							 DirectX::XMMATRIX world, 
							 DirectX::XMMATRIX view, 
							 DirectX::XMMATRIX ortho,
							 ID3D11ShaderResourceView* texture, 
							 DirectX::XMFLOAT4 pixelColor)
{
	bool result = false;

	// set up parameters for the vertex and pixel shaders
	result = SetShaderParameters(deviceContext, world, view, ortho, texture, pixelColor);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set shaders parameters");
		return false;
	}


	// render fonts on the screen using HLSL shaders
	RenderShaders(deviceContext, indexCount);

	return true;
} // Render()



// memory allocation
void* FontShaderClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for this object");
		return nullptr;
	}

	return ptr;
}

void FontShaderClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}




// --------------------------------------------------------------- //
//
//                      PRIVATE FUNCTIONS
//
// --------------------------------------------------------------- //

// InitializeShaders() helps to initialize the vertex and pixel shaders,
// input layout, sampler state, matrix and pixel buffers
bool FontShaderClass::InitializeShaders(ID3D11Device* pDevice,
										ID3D11DeviceContext* pDeviceContext, 
										HWND hwnd,
	                                    WCHAR* vsFilename, WCHAR* psFilename)
{
	//Log::Debug(THIS_FUNC_EMPTY);


	bool result = false;
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // a description of the vertex input layout

	// ------------------------------- INPUT LAYOUT DESC -------------------------------- //

	// set up description of the vertex input layout 
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




	// ---------------------------------- SHADERS --------------------------------------- //

	// initialize the vertex shader
	if (!vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum))
		return false;

	// initialize the pixel shader
	if (!pixelShader_.Initialize(pDevice, psFilename))
		return false;

	// -------------------------------- SAMPLER STATE ----------------------------------- //

	// initialize the sampler state
	if (!this->samplerState_.Initialize(pDevice))
		return false;


	// ------------------------------- CONSTANT BUFFERS --------------------------------- //

	// initialize the matrix buffer
	hr = this->matrixBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;
	
	
	// initialize the pixel buffer
	hr = this->pixelBuffer_.Initialize(pDevice, pDeviceContext);
	if (FAILED(hr))
		return false;
	

	return true;
} // InitializeShaders()


// SetShaderParameters() sets up parameters for the vertex and pixel shaders
bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	                                      DirectX::XMMATRIX world,
	                                      DirectX::XMMATRIX view,
	                                      DirectX::XMMATRIX ortho,
	                                      ID3D11ShaderResourceView* texture,
	                                      DirectX::XMFLOAT4 pixelColor)
{
	HRESULT hr = S_OK;
	UINT bufferNumber = 0;


	// ---------------- SET PARAMS FOR THE VERTEX SHADER ------------------- //

	// prepare matrices for using in the vertex shader
	matrixBuffer_.data.world = DirectX::XMMatrixTranspose(world);
	matrixBuffer_.data.view  = DirectX::XMMatrixTranspose(view);
	matrixBuffer_.data.ortho = DirectX::XMMatrixTranspose(ortho);

	// update the constant matrix buffer
	if (!matrixBuffer_.ApplyChanges())
		return false;

	// set the number of the buffer in the vertex shader
	bufferNumber = 0;

	// set up parameters for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, matrixBuffer_.GetAddressOf());



	// ---------------- SET PARAMS FOR THE PIXEL SHADER -------------------- //

	// prepare data for the pixel shader
	pixelBuffer_.data.pixelColor = pixelColor;

	// update the constant pixel buffer
	if (!pixelBuffer_.ApplyChanges())
		return false;


	// set the number of the buffer in the pixel shader
	bufferNumber = 0;

	// set up parameters for the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, pixelBuffer_.GetAddressOf());
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, this->samplerState_.GetAddressOf());


	return true;
} // SetShaderParameters()

// RenderShaders() renders fonts on the screen using HLSL shaders
void FontShaderClass::RenderShaders(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set vertex and pixel shaders for rendering
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the input layout 
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// render the fonts on the screen
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
