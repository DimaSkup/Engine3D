////////////////////////////////////////////////////////////////////
// Filename:     AlphaMapShaderClass.cpp
// Description:  an implementation of the AlphaMapShaderClass class;
//
// Created:      15.01.23
////////////////////////////////////////////////////////////////////
#include "AlphaMapShaderClass.h"



// class constructor
AlphaMapShaderClass::AlphaMapShaderClass()
{
	Log::Debug(LOG_MACRO);
	className_ = __func__;

	

	try
	{
		pVertexShader_ = new VertexShader();
		pPixelShader_ = new PixelShader();
		pSamplerState_ = new SamplerState();

		//pMatrixBuffer_ = new ConstantBufferInterface<ConstantMatrixBuffer_VS>();
		pMatrixBuffer_ = new ConstantBuffer<ConstantMatrixBuffer_VS>();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't create an instance of the alpha map shader class");
	}
};

// class copy constructor
AlphaMapShaderClass::AlphaMapShaderClass(const AlphaMapShaderClass& copy) {};

// class destructor
AlphaMapShaderClass::~AlphaMapShaderClass() 
{
	_DELETE(pMatrixBuffer_);
};



/////////////////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialize the alpha map HLSL shaders
bool AlphaMapShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	try
	{
		WCHAR* vsFilename = L"shaders/alphaMapVertex.hlsl";
		WCHAR* psFilename = L"shaders/alphaMapPixel.hlsl";

		// initialize the vertex and pixel shaders
		this->InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the alpha map shader");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}


// render alpha mapped textures using HLSL shaders
bool AlphaMapShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	                             DataContainerForShaders* pDataForShader)
{
	try
	{
		// set the shaders parameters that it will use for rendering
		this->SetShadersParameters(pDeviceContext, pDataForShader);

		// render prepared buffers with the shaders
		this->RenderShader(pDeviceContext, pDataForShader->indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render a model using alpha map shader");
		return false;
	}

	return true;
}


const std::string & AlphaMapShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


void AlphaMapShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	WCHAR* vsFilename,
	WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	constexpr UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];


	// set up the vertex input layout description;
	// this setup needs to match the VERTEX structure in the ModelClass and in the shader
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

	layoutDesc[1].SemanticName = "TEXCOORD";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[1].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;

	// initialize the vertex shader
	result = this->pVertexShader_->Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = this->pPixelShader_->Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the pixel shader");

	// initialize the sampler state
	result = this->pSamplerState_->Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");

	// initialize the matrix const buffer
	hr = this->pMatrixBuffer_->Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the matrix const buffer");

	return;
} // InitializeShaders()



  // SetShadersParameters() sets the matrices and texture array 
  // in the shaders before rendering;
void AlphaMapShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DataContainerForShaders* pDataForShader)
{
	bool result = false;


	// ---------------------------------------------------------------------------------- //
	//                 VERTEX SHADER: UPDATE THE CONSTANT MATRIX BUFFER                   //
	// ---------------------------------------------------------------------------------- //

	// update matrix buffer data
	pMatrixBuffer_->data.world      = DirectX::XMMatrixTranspose(pDataForShader->world);
	pMatrixBuffer_->data.view       = DirectX::XMMatrixTranspose(pDataForShader->view);
	pMatrixBuffer_->data.projection = DirectX::XMMatrixTranspose(pDataForShader->projection);

	result = this->pMatrixBuffer_->ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the matrix const buffer");

	// set the matrix const buffer in the vertex shader with the updated values
	pDeviceContext->VSSetConstantBuffers(0, 1, this->pMatrixBuffer_->GetAddressOf());


	// ---------------------------------------------------------------------------------- //
	//                            PIXEL SHADER: SET TEXTURES                              //
	// ---------------------------------------------------------------------------------- //

	try
	{
		assert("FIX SETTING OF THESE TEXTURES" && 0);
		pDeviceContext->PSSetShaderResources(0, 1, pDataForShader->texturesMap.at("diffuse"));
	}
	// in case if there is no such a key in the textures map we catch an exception about it;
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no texture with such a key");
	}

	return;
} // SetShadersParameters()


// render the alpha mapped textures and model using the HLSL shaders
void AlphaMapShaderClass::RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
{
	// set the vertex input layout
	pDeviceContext->IASetInputLayout(this->pVertexShader_->GetInputLayout());

	// set the vertex and pixel shader that will be used to render the model
	pDeviceContext->VSSetShader(this->pVertexShader_->GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(this->pPixelShader_->GetShader(), nullptr, 0);

	// set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, this->pSamplerState_->GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
