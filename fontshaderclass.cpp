/////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
// Revising: 23.07.22
/////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"

// initialize some internal variables 
FontShaderClass::FontShaderClass(void)
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pInputLayout = nullptr;
	m_pSamplerState = nullptr;
	m_pMatrixBuffer = nullptr;
	m_pPixelBuffer = nullptr;
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
bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result = false;

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// create shader objects, buffers, etc.
	result = InitializeShaders(device, hwnd, L"shaders/font.vs", L"shaders/font.ps");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize shaders");
		return false;
	}

	Log::Get()->Debug(THIS_FUNC, "is initialized()");

	return true;
} // Initialize()

// Shutdown() releases the memory from the shaders as well as the related objects
void FontShaderClass::Shutdown(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	ShutdownShaders();   // call the helper

	return;
}

// Render() renders fonts on the screen using HLSL shaders
bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	        DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX ortho,
	        ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 pixelColor)
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
bool FontShaderClass::InitializeShaders(ID3D11Device* device, HWND hwnd, 
	                                    WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* vsBuffer = nullptr;   // a buffer for vertex shader bytecode
	ID3DBlob* psBuffer = nullptr;   // a buffer for pixel shader bytecode
	D3D11_INPUT_ELEMENT_DESC layoutDesc[2]; // a description of the vertex input layout
	D3D11_BUFFER_DESC matrixBufferDesc;     // a description of the matrix buffer
	D3D11_BUFFER_DESC pixelBufferDesc;      // a description of the pixel buffer
	D3D11_SAMPLER_DESC samplerDesc;         // a description of the sampler state
	UINT layoutElemNum = 0;
	//D3D11_SUBRESOURCE_DATA bufferData;      // here we will set the initial data for buffers

	Log::Get()->Debug(THIS_FUNC_EMPTY);


	// ---------------------- VERTEX AND PIXEL SHADER ---------------------- //

	// compile the vertex shader code into a bytecode
	hr = compileShaderFromFile(vsFilename, "FontVertexShader", "vs_5_0", &vsBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't compile the vertex shader");
		return false;
	}

	// compile the pixel shader code into a bytecode
	hr = compileShaderFromFile(psFilename, "FontPixelShader", "ps_5_0", &psBuffer);
	if (FAILED(hr))
	{
		_RELEASE(vsBuffer); // because of error we need to release the vertex bytecode
		Log::Get()->Error(THIS_FUNC, "can't compile the pixel shader");
		return false;
	}

	// create a vertex shader object
	hr = device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),
		                            nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		// because of error we need to release both the vertex and pixel bytecodes
		_RELEASE(vsBuffer);
		_RELEASE(psBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create a vertex shader object");
		return false;
	}


	// create a pixel shader object
	hr = device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(),
		                           nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		// because of error we need to release both the vertex and pixel bytecodes
		_RELEASE(vsBuffer);
		_RELEASE(psBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create a pixel shader object");
		return false;
	}


	// ---------------------------- INPUT LAYOUT --------------------------- //

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

	layoutElemNum = ARRAYSIZE(layoutDesc); // define the count of elements in the input layout

	// create the input layout object
	hr = device->CreateInputLayout(layoutDesc, layoutElemNum,
		                           vsBuffer->GetBufferPointer(),
		                           vsBuffer->GetBufferSize(),
		                           &m_pInputLayout);
	if (FAILED(hr))
	{
		// because of error we need to release both the vertex and pixel bytecodes
		_RELEASE(vsBuffer);
		_RELEASE(psBuffer);
		Log::Get()->Error(THIS_FUNC, "can't create the vertex input layout");
		return false;
	}



	// release the memory from the vertex and pixel shader bytecodes because as they are no longer needed
	_RELEASE(vsBuffer);
	_RELEASE(psBuffer);



	// ---------------------------- SAMPLER STATE -------------------------- //

	// set up the sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;

	// create a sampler state object
	hr = device->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the sampler state object");
		return false;
	}

	// -------------------------- MATRICES BUFFER -------------------------- //

	// set up the matrices constant buffer description
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// create the matrices buffer object
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the matrices buffer");
		return false;
	}

	// ---------------------------- PIXEL BUFFER --------------------------- //

	// set up the pixel buffer description
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// create the pixel constant buffer object
	hr = device->CreateBuffer(&pixelBufferDesc, nullptr, &m_pPixelBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the pixel buffer");
		return false;
	}
	

	return true;
} // InitializeShaders()

// ShutdownShaders() helps to release the memory from 
// the shaders, input layout, sampler state and buffers
void FontShaderClass::ShutdownShaders(void)
{
	_RELEASE(m_pPixelBuffer);
	_RELEASE(m_pMatrixBuffer);
	_RELEASE(m_pSamplerState);
	_RELEASE(m_pInputLayout);
	_RELEASE(m_pPixelShader);
	_RELEASE(m_pVertexShader);

	return;
}

// SetShaderParameters() sets up parameters for the vertex and pixel shaders
bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	                                      DirectX::XMMATRIX world,
	                                      DirectX::XMMATRIX view,
	                                      DirectX::XMMATRIX ortho,
	                                      ID3D11ShaderResourceView* texture,
	                                      DirectX::XMFLOAT4 pixelColor)
{
	HRESULT hr = S_OK;
	MatrixBufferType* matrixDataPtr = nullptr;  // a ptr to the matrices buffer structure
	PixelBufferType*  pixelDataPtr = nullptr;   // a ptr to the pixels buffer structure
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber = 0;


	// ---------------- SET PARAMS FOR THE VERTEX SHADER ------------------- //

	// lock the matrices constant buffer for writing in
	hr = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't lock the matrices constant buffer");
		return false;
	}

	// prepare matrices for using in the vertex shader
	world = DirectX::XMMatrixTranspose(world);
	view = DirectX::XMMatrixTranspose(view);
	ortho = DirectX::XMMatrixTranspose(ortho);

	// get a pointer to the data in the matrices buffer
	matrixDataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

	// write in the data 
	matrixDataPtr->world = world;
	matrixDataPtr->view = view;
	matrixDataPtr->ortho = ortho;

	// unlock the matrices constant buffer
	deviceContext->Unmap(m_pMatrixBuffer, 0);

	// set the number of the buffer in the vertex shader
	bufferNumber = 0;

	// set up parameters for the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	matrixDataPtr = nullptr;




	// ---------------- SET PARAMS FOR THE PIXEL SHADER -------------------- //

	// lock the pixels constant buffer for writing in
	hr = deviceContext->Map(m_pPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't lock the pixels constant buffer");
		return false;
	}

	// get a pointer to the data in the pixels buffer
	pixelDataPtr = static_cast<PixelBufferType*>(mappedResource.pData);

	// write in the data
	pixelDataPtr->pixelColor = pixelColor;

	// unlock the pixels constant buffer
	deviceContext->Unmap(m_pPixelBuffer, 0);

	// set the number of the buffer in the pixel shader
	bufferNumber = 0;

	// set up parameters for the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pPixelBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

	matrixDataPtr = nullptr;

	return true;
} // SetShaderParameters()

// RenderShaders() helps to render fonts on the screen using HLSL shaders
void FontShaderClass::RenderShaders(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set vertex and pixel shaders for rendering
	deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// set the input layout 
	deviceContext->IASetInputLayout(m_pInputLayout);

	// render the fonts on the screen
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}


// compiles an HLSL shader code into shader byte code
HRESULT FontShaderClass::compileShaderFromFile(WCHAR* shaderFilename, LPCSTR functionName,
	                                           LPCSTR shaderModel, ID3DBlob** shaderOutput)
{
	return ShaderClass::compileShaderFromFile(shaderFilename, functionName, shaderModel, shaderOutput);
}