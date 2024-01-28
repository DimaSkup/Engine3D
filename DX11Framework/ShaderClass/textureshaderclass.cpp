////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"



TextureShaderClass::TextureShaderClass(void) 
{
	Log::Debug(LOG_MACRO);
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
		Log::Error(LOG_MACRO, "can't initialize the texture shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	//Log::Print(LOG_MACRO);

	assert(pDataForShader != nullptr);

	try
	{
		// set the shader parameters
		this->SetShadersParameters(pDeviceContext,
			pDataForShader);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render the model");
	}


	return true;
}

///////////////////////////////////////////////////////////


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

	// initialize the constant camera buffer
	hr = this->cameraBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the camera buffer");

	// initialize the buffer per frame
	hr = this->bufferPerFrame_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the buffer per frame");


	return;
} // InitializeShader()


// Sets the variables which are used within the vertex shader.
// This function takes in a pointer to a texture resource and then assigns it to 
// the shader using a texture resource pointer. Note that the texture has to be set 
// before rendering of the buffer occurs.
void TextureShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	bool result = false;

	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //


	// update data of the matrix const buffer
	matrixConstBuffer_.data.world         = DirectX::XMMatrixTranspose(pDataForShader->world);
	matrixConstBuffer_.data.worldViewProj = DirectX::XMMatrixTranspose(pDataForShader->WVP);

	result = matrixConstBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "failed to update the matrix constant buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixConstBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                 PIXEL SHADER: UPDATE THE CONSTANT CAMERA BUFFER                    //
	// ---------------------------------------------------------------------------------- //

	// prepare data for the constant camera buffer
	cameraBuffer_.data.cameraPosition = pDataForShader->cameraPos;
	cameraBuffer_.data.padding = 0.0f;

	// update the constant camera buffer
	result = cameraBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the camera buffer");

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(0, 1, cameraBuffer_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                    PIXEL SHADER: UPDATE THE BUFFER PER FRAME                       //
	// ---------------------------------------------------------------------------------- //

	// only if fog enabled we update its params
	if (pDataForShader->fogEnabled)
	{
		bufferPerFrame_.data.fogColor = pDataForShader->fogColor;
		bufferPerFrame_.data.fogStart = pDataForShader->fogStart;
		bufferPerFrame_.data.fogRange_inv = pDataForShader->fogRange_inv;
	}

	// setup if the fog is enabled for pixel shader
	bufferPerFrame_.data.fogEnabled = pDataForShader->fogEnabled;
	
	// setup if the pixel shader will execute alpha clipping
	bufferPerFrame_.data.useAlphaClip = pDataForShader->useAlphaClip;

	// update the constant camera buffer
	result = bufferPerFrame_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the buffer per frame");

	// set the buffer for the vertex shader
	pDeviceContext->PSSetConstantBuffers(1, 1, bufferPerFrame_.GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                            PIXEL SHADER: SET TEXTURES                              //
	// ---------------------------------------------------------------------------------- //

	try
	{
		// Set shader texture resource for the pixel shader
		pDeviceContext->PSSetShaderResources(0, 1, pDataForShader->texturesMap.at("diffuse"));
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture by such a key");
	}
	

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