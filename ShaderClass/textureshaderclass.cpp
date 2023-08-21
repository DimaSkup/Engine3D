////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"



TextureShaderClass::TextureShaderClass(void) 
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass&)
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
bool TextureShaderClass::Initialize(ID3D11Device* pDevice, 
									ID3D11DeviceContext* pDeviceContext, 
									HWND hwnd)
{
	bool result = false;
	WCHAR* vsFilename = L"shaders/textureVertex.hlsl";
	WCHAR* psFilename = L"shaders/texturePixel.hlsl";

	result = InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the texture shaders");


	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// Sets variables are used inside the shaders and renders the model using these shaders. 
// Also this function takes a parameters called texture
// which is the pointer to the texture resource.
bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const int indexCount,
	const DirectX::XMMATRIX & world,
	ID3D11ShaderResourceView* const* textureArray,
	DataContainerForShadersClass* pDataForShader)
{
	bool result = false;
	float alpha = 1.0f;  // a value for the alpha-channel of colour

	// Set the shaders parameters that will be used for rendering
	result = SetShadersParameters(pDeviceContext,
		world,                                     // model's world
		pDataForShader->GetViewMatrix(),
		pDataForShader->GetProjectionMatrix(), 
		textureArray[0], 
		alpha);
	COM_ERROR_IF_FALSE(result, "can't set texture shader parameters");


	// Now render the prepared buffers with the shaders
	RenderShaders(pDeviceContext, indexCount);

	return true;
}



bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,         // it also can be baseViewMatrix for UI rendering
	const DirectX::XMMATRIX & projection,   // it also can be orthographic matrix for UI rendering
	ID3D11ShaderResourceView* const textureArray)
{
	bool result = false;
	float alpha = 1.0f;  // a value for the alpha-channel of colour

						 // Set the shaders parameters that will be used for rendering
	result = SetShadersParameters(pDeviceContext,
		world,                                     // model's world
		view,
		projection,
		textureArray,
		alpha);
	COM_ERROR_IF_FALSE(result, "can't set texture shader parameters");


	// Now render the prepared buffers with the shaders
	RenderShaders(pDeviceContext, indexCount);

	return true;
}


const std::string & TextureShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}

// ------------------------------------------------------------------------- //
//
//                        PRIVATE METHODS
//
// ------------------------------------------------------------------------- //

// initialized the vertex shader, pixel shader, input layout, and sampler;
bool TextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
										   ID3D11DeviceContext* pDeviceContext,
										   HWND hwnd,
	                                       WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[layoutElemNum];

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


	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, polygonLayout, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");




	// initialize the matrix const buffer (for the vertex shader)
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	// initialize the alpha const buffer (for the pixel shader)
	hr = this->alphaConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the alpha const buffer");




	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");


	return true;
} // InitializeShader()


// Sets the variables which are used within the vertex shader.
// This function takes in a pointer to a texture resource and then assigns it to 
// the shader using a texture resource pointer. Note that the texture has to be set 
// before rendering of the buffer occurs.
bool TextureShaderClass::SetShadersParameters(ID3D11DeviceContext* deviceContext,
	                                         DirectX::XMMATRIX worldMatrix,
	                                         DirectX::XMMATRIX viewMatrix,
	                                         DirectX::XMMATRIX projectionMatrix,
	                                         ID3D11ShaderResourceView* texture,
											 float alpha)
{
	HRESULT hr = S_OK;
	UINT bufferNumber = 0;	// Set the position of the constant buffer in the vertex shader
	bool result = false;


	// --------------- UPDATE THE VERTEX SHADER ------------------- //


	// update data of the matrix const buffer
	matrixConstBuffer_.data.world = DirectX::XMMatrixTranspose(worldMatrix);
	matrixConstBuffer_.data.view = DirectX::XMMatrixTranspose(viewMatrix);
	matrixConstBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	result = matrixConstBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "failed to update the matrix constant buffer");


	// Now set the matrix const buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, matrixConstBuffer_.GetAddressOf());



	// --------------- UPDATE THE PIXEL SHADER -------------------- //

	// update data of the alpha const buffer
	alphaConstBuffer_.data.alpha = alpha;

	result = alphaConstBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "failed to update the alpha const buffer");


	// set the alpha const buffer in the pixel shader with the updated values
	deviceContext->PSSetConstantBuffers(0, 1, alphaConstBuffer_.GetAddressOf());

	// Set shader texture resource for the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
} // SetShadersParameters()


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