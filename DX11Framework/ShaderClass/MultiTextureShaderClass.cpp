////////////////////////////////////////////////////////////////////
// Filename:    MultiTextureShaderClass.cpp
// Description: an implementation of the MultiTextureShaderClass
// Created:     09.01.23
////////////////////////////////////////////////////////////////////
#include "MultiTextureShaderClass.h"


MultiTextureShaderClass::MultiTextureShaderClass()
{
	Log::Debug(THIS_FUNC_EMPTY);
	className_ = __func__;
}

MultiTextureShaderClass::~MultiTextureShaderClass()
{
}



////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// initialize the multitexture HLSL shaders
bool MultiTextureShaderClass::Initialize(ID3D11Device* pDevice, 
										 ID3D11DeviceContext* pDeviceContext, 
										 HWND hwnd)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/MultiTextureVertex.hlsl";
		const WCHAR* psFilename = L"shaders/MultiTexturePixel.hlsl";

		// initialize the vertex and pixel shaders
		InitializeShaders(pDevice, pDeviceContext, hwnd, vsFilename, psFilename);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the multi texture shader class");
		return false;
	}

	Log::Debug(THIS_FUNC, "is initialized");

	return true;
}


// the Render() function takes as input a pointer to the texture array.
// This will give the shader access to the two textures for blending operations.
bool MultiTextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount,
	const DirectX::XMMATRIX & world,
	const DirectX::XMMATRIX & view,
	const DirectX::XMMATRIX & projection,
	ID3D11ShaderResourceView* const textureArray)
{
	try
	{
		// set the shaders parameters that will be used for rendering
		this->SetShadersParameters(pDeviceContext,
			world,
			view,
			projection,
			textureArray);

		// now render the prepared buffers with the shader
		this->RenderShaders(pDeviceContext, indexCount);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't render");
		return false;
	}

	return true;
}


const std::string & MultiTextureShaderClass::GetShaderName() const _NOEXCEPT
{
	return className_;
}


////////////////////////////////////////////////////////////////////
//
//                     PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// the InitializeShaders() loads the vertex and pixel shaders as well as 
// setting up the layout, matrix buffer, and sample state
void MultiTextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	constexpr UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum];


	// set up the input layout description
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
	result = this->vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	COM_ERROR_IF_FALSE(result, "can't initialize the multi texture vertex shader");

	// initialize the pixel shader
	result = this->pixelShader_.Initialize(pDevice, psFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the multi texture pixel shader");

	// initialize the sampler state
	result = this->samplerState_.Initialize(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the sampler state");

	// initialize the constant matrix buffer
	hr = this->matrixConstBuffer_.Initialize(pDevice, pDeviceContext);
	COM_ERROR_IF_FAILED(hr, "can't initialize the constant matrix buffer");

	return;
} // InitializeShaders()


// SetShadersParameters() sets the matrices and texture array 
// in the shaders before rendering;
void MultiTextureShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & worldMatrix,
	const DirectX::XMMATRIX & viewMatrix,
	const DirectX::XMMATRIX & projectionMatrix,
	ID3D11ShaderResourceView* const textureArray)
{
	// ------------------------- UPDATE THE VERTEX SHADER -------------------------- //

	// transpose matrices and update the matrix constant buffer
	this->matrixConstBuffer_.data.world      = DirectX::XMMatrixTranspose(worldMatrix);
	this->matrixConstBuffer_.data.view       = DirectX::XMMatrixTranspose(viewMatrix);
	this->matrixConstBuffer_.data.projection = DirectX::XMMatrixTranspose(projectionMatrix);

	bool result = this->matrixConstBuffer_.ApplyChanges();
	COM_ERROR_IF_FALSE(result, "can't update the constant matrix buffer");

	// set the matrix constant buffer in the vertex shader with the updated valuved
	pDeviceContext->VSSetConstantBuffers(0, 1, this->matrixConstBuffer_.GetAddressOf());


	// ------------------------- UPDATE THE PIXEL SHADER --------------------------- //

	// set shader texture array resource in the pixel shader
	pDeviceContext->PSSetShaderResources(0, 2, &textureArray); 


	return;
} // SetShadersParameters()


// The RenderShaders() sets the layout, shaders, and sampler.
// It then draws the model using the HLSL shaders
void MultiTextureShaderClass::RenderShaders(ID3D11DeviceContext* pDeviceContext,
	const UINT indexCount)
{
	// set the vertex input layout
	pDeviceContext->IASetInputLayout(this->vertexShader_.GetInputLayout());

	// set the vertex and pixel shaders that will be used for rendering
	pDeviceContext->VSSetShader(this->vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(this->pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
	
	return;
}
