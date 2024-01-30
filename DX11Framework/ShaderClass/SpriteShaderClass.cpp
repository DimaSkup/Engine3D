////////////////////////////////////////////////////////////////////
// Filename:     SpriteShaderClass.cpp
// Description:  an implementation of the SpriteShaderClass functional;
//
// Created:      28.12.23
////////////////////////////////////////////////////////////////////
#include "SpriteShaderClass.h"






SpriteShaderClass::SpriteShaderClass(void)
{
	Log::Debug(LOG_MACRO);
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
		Log::Error(LOG_MACRO, e.what());
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
		Log::Error(LOG_MACRO, "can't initialize the light shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

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
		SetShadersParameters(pDeviceContext, pDataForShader);

		// render the model using this shader
		RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render");
		return false;
	}

	return true;

} // end Render

///////////////////////////////////////////////////////////

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
	const DataContainerForShaders* pDataForShader)
{
	// sets parameters for the HLSL shaders

	bool result = false;

	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT MATRIX BUFFER                              //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer;
	// (WVO == world_matrix * base_view_matrix * ortho_matrix)
	// NOTE: WVO is already transposed
	pMatrixBuffer_->data.WVP = pDataForShader->WVO;

	// update the constant matrix buffer
	result = pMatrixBuffer_->ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, pMatrixBuffer_->GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                  PIXEL SHADER: UPDATE SHADER TEXTURE RESOURCES                     //
	// ---------------------------------------------------------------------------------- //

	try
	{
		// because for 2D sprites we use only one diffuse texture we try to find it in the map;
		pDeviceContext->PSSetShaderResources(0, 1, pDataForShader->texturesMap.at("diffuse"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
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
