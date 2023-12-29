////////////////////////////////////////////////////////////////////
// Filename:     SpriteShaderClass.cpp
// Description:  an implementation of the SpriteShaderClass functional;
//
// Created:      28.12.23
////////////////////////////////////////////////////////////////////
#include "SpriteShaderClass.h"






SpriteShaderClass::SpriteShaderClass(void)
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;

	// allocate memory for the sprite shader class members
	try
	{
		pVertexShader_ = std::make_unique<VertexShader>();
		pPixelShader_  = std::make_unique<PixelShader>();
		pSamplerState_ = std::make_unique<SamplerState>();
		pMatrixBuffer_ = std::make_unique<ConstantBuffer<ConstantMatrixBuffer_VS_WVP>>();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the members of the class");
	}
}

SpriteShaderClass::~SpriteShaderClass(void)
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool SpriteShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/spriteVS.hlsl";
		const WCHAR* psFilename = L"shaders/spritePS.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the light shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
} // end Initialize

  ///////////////////////////////////////////////////////////

bool SpriteShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	DataContainerForShaders* pDataForShader)
{
	// we call this function from the model_to_shader mediator

	try
	{
		// set the shader parameters
		SetShadersParameters(pDeviceContext,
			pDataForShader->WVP,             // world * base_view * ortho matrix
			pDataForShader->texturesMap);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

	return true;

} // end Render

  ///////////////////////////////////////////////////////////


  // 1. Sets the parameters for HLSL shaders which are used for rendering
  // 2. Renders the model using the HLSL shadersbool Render(ID3D11DeviceContext* deviceContext,
bool SpriteShaderClass::Render(ID3D11DeviceContext* deviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & WVO,   // world * base_view * ortho matrix
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap)
{

	COM_ERROR_IF_ZERO(texturesMap.size(), "there is no data in the textures map");

	try
	{
		// set the shader parameters
		SetShadersParameters(deviceContext,
			WVO,                     // world * base_view * ortho matrix
			texturesMap);

		// render the model using this shader
		RenderShader(deviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

	return true;
}


const std::string & SpriteShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}


// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PRIVATE FUNCTIONS                                        //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// helps to initialize the HLSL shaders, layout, sampler state, and buffers
void SpriteShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	// before initialization check all the important members/parts of the class
	assert(pVertexShader_);
	assert(pPixelShader_);
	assert(pSamplerState_);
	assert(pMatrixBuffer_);
	assert(textureKeys_.empty() != true);


	const UINT layoutElemNum = 2;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	bool result = false;
	HRESULT hr = S_OK;


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

	// -------------------------- SHADERS / SAMPLER STATE ------------------------------- //

	// initialize the vertex shader
	result = this->pVertexShader_->Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pPixelShader_->Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the sampler state
	result = this->pSamplerState_->Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");


	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = this->pMatrixBuffer_->Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix buffer");

	return;
}

///////////////////////////////////////////////////////////



void SpriteShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & WVO,    // world_matrix * base_view_matrix * ortho_matrix
	const std::map<std::string, ID3D11ShaderResourceView**> & texturesMap)
{
	// sets parameters for the HLSL shaders

	bool result = false;
	HRESULT hr = S_OK;
	UINT bufferPosition = 0;   // set the buffer position in a HLSL shader


	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT MATRIX BUFFER                              //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer
	pMatrixBuffer_->data.WVP = WVO;

	// update the constant matrix buffer
	result = pMatrixBuffer_->ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(bufferPosition, 1, pMatrixBuffer_->GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                  PIXEL SHADER: UPDATE SHADER TEXTURE RESOURCES                     //
	// ---------------------------------------------------------------------------------- //

	try
	{
		// because for 2D sprites we use only one diffuse texture we try to find it in the map;
		pDeviceContext->PSSetShaderResources(0, 1, texturesMap.at(textureKeys_.front()));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}

	return;

} // end SetShaderParameters

  ///////////////////////////////////////////////////////////

void SpriteShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, const UINT indexCount)
{
	// this function sets stuff which we will use: layout, vertex and pixel shader,
	// sampler state, and also renders our 3D model

	// set the input layout for the vertex shader
	deviceContext->IASetInputLayout(pVertexShader_->GetInputLayout());

	// set shader which we will use for rendering
	deviceContext->VSSetShader(pVertexShader_->GetShader(), nullptr, 0);
	deviceContext->PSSetShader(pPixelShader_->GetShader(), nullptr, 0);

	// set the sampler state for the pixel shader
	deviceContext->PSSetSamplers(0, 1, pSamplerState_->GetAddressOf());

	// render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
