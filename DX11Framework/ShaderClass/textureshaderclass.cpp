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
	try
	{
		const WCHAR* vsFilename = L"shaders/textureVertex.hlsl";
		const WCHAR* psFilename = L"shaders/texturePixel.hlsl";

		InitializeShaders(pDevice, 
			pDeviceContext, 
			hwnd,
			vsFilename,
			psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the texture shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	//Log::Print(THIS_FUNC_EMPTY);

	assert(pDataForShader != nullptr);

	try
	{
		// set the shader parameters
		this->SetShadersParameters(pDeviceContext,
			pDataForShader->world,
			pDataForShader->view,
			pDataForShader->orthoOrProj,
			pDataForShader->ppTextures);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render the model");
	}


	return true;
}

///////////////////////////////////////////////////////////


// Sets variables are used inside the shaders and renders the model using these shaders. 
// Also this function takes a parameters called texture
// which is the pointer to the texture resource.
bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,            // it also can be baseViewMatrix for UI rendering
	const DirectX::XMMATRIX & projection,      // it also can be orthographic matrix for UI rendering
	ID3D11ShaderResourceView* const* pTextureArray)
{
	try
	{
		// Set the shaders parameters that will be used for rendering
		SetShadersParameters(pDeviceContext,
			world,                                   
			view,
			projection,
			pTextureArray);

		// Now render the prepared buffers with the shaders
		RenderShader(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

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
void TextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[layoutElemNum];


	// ------------------------------- INPUT LAYOUT DESC -------------------------------- //

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


	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, polygonLayout, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");



	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the matrix const buffer (for the vertex shader)
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	return;
} // InitializeShader()


// Sets the variables which are used within the vertex shader.
// This function takes in a pointer to a texture resource and then assigns it to 
// the shader using a texture resource pointer. Note that the texture has to be set 
// before rendering of the buffer occurs.
void TextureShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const* pTextureArray)
{
	bool result = false;

	// ---------------- SET PARAMS FOR THE VERTEX SHADER ------------------- //

	// update data of the matrix const buffer
	matrixConstBuffer_.data.world      = DirectX::XMMatrixTranspose(world);
	matrixConstBuffer_.data.view       = DirectX::XMMatrixTranspose(view);
	matrixConstBuffer_.data.projection = DirectX::XMMatrixTranspose(projection);

	result = matrixConstBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "failed to update the matrix constant buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixConstBuffer_.GetAddressOf());



	// ---------------- SET PARAMS FOR THE PIXEL SHADER -------------------- //

	// Set shader texture resource for the pixel shader
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureArray[0]);

	return;
} // SetShadersParameters()


// Calls the shader technique to render the polygons
void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, 
	const UINT indexCount)
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