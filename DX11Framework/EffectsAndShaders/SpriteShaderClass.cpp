////////////////////////////////////////////////////////////////////
// Filename:     SpriteShaderClass.cpp
// Description:  an implementation of the SpriteShaderClass functional;
//
// Created:      28.12.23
////////////////////////////////////////////////////////////////////
#include "SpriteShaderClass.h"






SpriteShaderClass::SpriteShaderClass()
	: className_ {__func__}
{
	Log::Debug(LOG_MACRO);
}

SpriteShaderClass::~SpriteShaderClass()
{
}



// ---------------------------------------------------------------------------------- //
//                                                                                    //
//                           PUBLIC FUNCTIONS                                         //
//                                                                                    //
// ---------------------------------------------------------------------------------- //

// Initializes the shaders for rendering of the model
bool SpriteShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/spriteVS.hlsl";
		const WCHAR* psFilename = L"shaders/spritePS.hlsl";

		// try to initialize the vertex and pixel HLSL shaders
		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
	}
	catch (EngineException & e)
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
	const UINT indexCount)
{
	// we call this function from the model_to_shader mediator

	try
	{
		// set the shader parameters
		SetShadersParameters(pDeviceContext);

		// render the model using this shader
		RenderShader(pDeviceContext, indexCount);
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render");
		return false;
	}

	return true;

} // end Render

///////////////////////////////////////////////////////////

const std::string & SpriteShaderClass::GetShaderName() const
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
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
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
	result = vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	ASSERT_TRUE(result, "can't initialize the vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	ASSERT_TRUE(result, "can't initialize the vertex shader");

	// initialize the sampler state
	result = samplerState_.Initialize(pDevice);
	ASSERT_TRUE(result, "can't initialize the vertex shader");


	// ----------------------------- CONSTANT BUFFERS ----------------------------------- //

	// initialize the constant matrix buffer
	hr = matrixBuffer_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the matrix buffer");

	return;
}

///////////////////////////////////////////////////////////



void SpriteShaderClass::SetShadersParameters(ID3D11DeviceContext* pDeviceContext)
{
	// sets parameters for the HLSL shaders

	bool result = false;
#if 0
	// ---------------------------------------------------------------------------------- //
	//                     UPDATE THE CONSTANT MATRIX BUFFER                              //
	// ---------------------------------------------------------------------------------- //

	// prepare matrices for using in the HLSL constant matrix buffer;
	// (WVO == world_matrix * base_view_matrix * ortho_matrix)
	// NOTE: WVO is already transposed
	matrixBuffer_->data.WVP = pDataForShader->WVO;

	// update the constant matrix buffer
	result = matrixBuffer_->ApplyChanges(pDeviceContext);
	ASSERT_TRUE(result, "can't update the matrix buffer");

	// set the buffer for the vertex shader
	pDeviceContext->VSSetConstantBuffers(0, 1, matrixBuffer_->GetAddressOf());


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
		ASSERT_TRUE(false, "there is no texture with such a key");
	}
#endif
	return;

} // end SetShaderParameters

  ///////////////////////////////////////////////////////////

void SpriteShaderClass::RenderShader(ID3D11DeviceContext* pDeviceContext, const UINT indexCount)
{
	// this function sets stuff which we will use: layout, vertex and pixel shader,
	// sampler state, and also renders our 3D model

	// set the input layout for the vertex shader
	pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// set shader which we will use for rendering
	pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state for the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// render the model
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
