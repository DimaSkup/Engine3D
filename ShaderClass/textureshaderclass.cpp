////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass(void)
{
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass(void)
{
}

// ------------------------------------------------------------------------- //
//
//                        PUBLIC METHODS
//
// ------------------------------------------------------------------------- //

// Loads the texture HLSL files for this shader
bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!InitializeShaders(device, hwnd, 
		L"shaders/textureVertex.hlsl", L"shaders/texturePixel.hlsl"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture shaders");
		return false;
	}

	return true;
}

// Releases the shader variables
void TextureShaderClass::Shutdown(void)
{
	ShutdownShaders();

	return;
}

// Sets variables are used inside the shaders and renders the model using these shaders. 
// Also this function takes a parameters called texture
// which is the pointer to the texture resource.
bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                                DirectX::XMMATRIX worldMatrix, 
                                DirectX::XMMATRIX viewMatrix, 
	                            DirectX::XMMATRIX projectionMatrix,
                                ID3D11ShaderResourceView* texture)
{
	bool result;

	// Set the shaders parameters that will be used for rendering
	result = SetShadersParameters(deviceContext, worldMatrix, viewMatrix,
		                         projectionMatrix, texture);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set texture shader parameters");
		return false;
	}

	// Now render the prepared buffers with the shaders
	RenderShaders(deviceContext, indexCount);

	return true;
}

// memory allocation
void* TextureShaderClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void TextureShaderClass::operator delete(void* p)
{
	_aligned_free(p);
}

// ------------------------------------------------------------------------- //
//
//                        PRIVATE METHODS
//
// ------------------------------------------------------------------------- //

// initialized the vertex shader, pixel shader, input layout, and sampler;
bool TextureShaderClass::InitializeShaders(ID3D11Device* pDevice, HWND hwnd,
	                                      WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[layoutElemNum];
	D3D11_BUFFER_DESC matrixBufferDesc;	// a description of the constant buffer for the vertex shader


	// ------------------ CREATION OF THE VERTEX INPUT DESC ------------------- //
	// Create the vertex input layout description
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;




	// -------------------  INITIALIZATION OF THE SHADERS -------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, polygonLayout, layoutElemNum);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the vertex shader");
		return false;
	}

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the pixel shader");
		return false;
	}



	// ----------------- CREATION OF A DYNAMIC MATRIX CONSTANT BUFFER ------------------ //

	// Setup the description of the dynamic matrix constant buffer that is used in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant 
	// buffer from withing this class
	hr = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &pMatrixBuffer_);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the matrix constant buffer");
		return false;
	}



	// ------------------- CREATION OF THE TEXTURE SAMPLER STATE -------------------- //

	if (!this->samplerState_.Initialize(pDevice))
		return false;



	Log::Get()->Debug(THIS_FUNC, "shaders are initialized successfully");

	return true;
} // InitializeShader()

// Releases all the variables used in the TextureShaderClass
void TextureShaderClass::ShutdownShaders(void)
{
	_RELEASE(pMatrixBuffer_);

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Sets the variables which are used within the vertex shader.
// This function takes in a pointer to a texture resource and then assigns it to 
// the shader using a texture resource pointer. Note that the texture has to be set 
// before rendering of the buffer occurs.
bool TextureShaderClass::SetShadersParameters(ID3D11DeviceContext* deviceContext,
	                                         DirectX::XMMATRIX worldMatrix,
	                                         DirectX::XMMATRIX viewMatrix,
	                                         DirectX::XMMATRIX projectionMatrix,
	                                         ID3D11ShaderResourceView* texture)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr = nullptr;
	UINT bufferNumber = 0;	// Set the position of the constant buffer in the vertex shader

	// Transpose the matrices to prepare them for the shader
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);



	// Lock the constant buffer so it can be written to
	hr = deviceContext->Map(pMatrixBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the constant buffer");
		return false;
	}

	// Get a pointer to the data in the constant buffer
	dataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

	// Copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer
	deviceContext->Unmap(pMatrixBuffer_, 0);

	// Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &pMatrixBuffer_);

	// Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}


// Calls the shader technique to render the polygons
void TextureShaderClass::RenderShaders(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// Set the vertex and pixels shaders that will be used to render the model
	deviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// Render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);
	
	return;
}