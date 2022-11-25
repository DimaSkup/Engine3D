/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass(void)
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& another) {}

ColorShaderClass::~ColorShaderClass(void)
{
	Shutdown();
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the ColorShaderClass
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShaders(device, hwnd, 
		                   L"shaders/colorVertex.hlsl", 
		                   L"shaders/colorPixel.hlsl"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize shaders");
		return false;
	}

	Log::Get()->Debug(THIS_FUNC, "ColorShaderClass is initialized successfully");
	return true;
}

// Releases the memory
void ColorShaderClass::Shutdown(void)
{
	ShutdownShader();
	Log::Debug(THIS_FUNC_EMPTY);

	return;
}

// Sets shaders parameters and renders our 3D model using HLSL shaders
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	                          DirectX::XMMATRIX worldMatrix,
	                          DirectX::XMMATRIX viewMatrix,
	                          DirectX::XMMATRIX projectionMatrix)
{
	bool result = false;

	// set the shader parameters
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set shader parameters");
		return false;
	}

	// render the model using this shader
	RenderShader(deviceContext, indexCount);

	return true;
}



// memory allocation
void* ColorShaderClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void ColorShaderClass::operator delete(void* p)
{
	_aligned_free(p);
}

// ------------------------------------------------------------------------------ //
//
//                         PRIVATE FUNCTIONS
//
// ------------------------------------------------------------------------------ //

// Initializes the shaders, input vertex layout and constant matrix buffer.
// This function is called from the Initialize() function
bool ColorShaderClass::InitializeShaders(ID3D11Device* pDevice, HWND hwnd,
	                                     WCHAR* vsFilename,
                                         WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	const UINT layoutElemNum = 4;      // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];
	

	// ---------------------------------------------------------------------------------- //
	//                         CREATION OF THE VERTEX SHADER                              //
	// ---------------------------------------------------------------------------------- //

	// set the description for the input layout
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

	layoutDesc[2].SemanticName = "NORMAL";
	layoutDesc[2].SemanticIndex = 0;
	layoutDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[2].InputSlot = 0;
	layoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[2].InstanceDataStepRate = 0;

	layoutDesc[3].SemanticName = "COLOR";
	layoutDesc[3].SemanticIndex = 0;
	layoutDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layoutDesc[3].InputSlot = 0;
	layoutDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[3].InstanceDataStepRate = 0;


	// initialize the vertex shader
	if (!this->vertexShader.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum))
		return false;



	// ---------------------------------------------------------------------------------- //
	//                         CREATION OF THE PIXEL SHADER                               //
	// ---------------------------------------------------------------------------------- //

	// initialize the pixel shader
	if (!this->pixelShader.Initialize(pDevice, psFilename))
		return false;



	// ---------------------------------------------------------------------------------- //
	//                        CREATION OF CONSTANT BUFFERS                                //
	// ---------------------------------------------------------------------------------- //

	// ----------------- CREATION OF A CONSTANT MATRIX SHADER BUFFER ------------------- //
	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.MiscFlags = 0;

	hr = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &pMatrixBuffer_);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the constant matrix shader buffer");
		return false;
	}

	return true;
} // InitializeShader()


// Releases the memory from shader interfaces, input layout, matrices buffer
// This function is called from the Shutdown() function
void ColorShaderClass::ShutdownShader(void)
{
	_RELEASE(pMatrixBuffer_);
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Setup parameters of shaders
// This function is called from the Render() function
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	                                       DirectX::XMMATRIX worldMatrix,
	                                       DirectX::XMMATRIX viewMatrix,
	                                       DirectX::XMMATRIX projectionMatrix)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	MatrixBufferType* dataPtr = nullptr;

	// Transpose matrices to prepare them for the shader
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to
	hr = deviceContext->Map(pMatrixBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant matrix buffer");
		return false;
	}

	// Get a pointer to the data in the constant buffer
	dataPtr = static_cast<MatrixBufferType*>(mappedData.pData);

	// Setup constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer
	deviceContext->Unmap(pMatrixBuffer_, 0);

	return true;
}

// Sets as active the vertex and pixel shader, input vertex layout and matrix buffer
// Renders the model
// This function is called from the Render() function
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// set shaders which will be used to render the model
	deviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0);

	// set the format of input shader data
	deviceContext->IASetInputLayout(vertexShader.GetInputLayout());

	// set the input shader data (constant buffer)
	deviceContext->VSSetConstantBuffers(0, 1, &pMatrixBuffer_);	

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
