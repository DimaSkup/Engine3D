////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"



TextureShaderClass::TextureShaderClass(void) 
	: className_{__func__}
{
	Log::Debug(LOG_MACRO);
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass&)
{
}

TextureShaderClass::~TextureShaderClass(void) 
{
}


// ************************************************************************************
//                           PUBLIC MODIFICATION API
// ************************************************************************************

// Loads the texture HLSL files for this shader
bool TextureShaderClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		const WCHAR* vsFilename = L"shaders/textureVS.hlsl";
		const WCHAR* psFilename = L"shaders/texturePS.hlsl";

		InitializeShaders(pDevice, 
			pDeviceContext, 
			vsFilename,
			psFilename);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the texture shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}


// ************************************************************************************
//                             PUBLIC RENDERING API
// ************************************************************************************

void TextureShaderClass::PrepareShaderForRendering(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3 & cameraPosition)
{
	// prepare the shaders for rendering; setup some data for it;
	// 
	// Note: YOU MUST CALL THIS FUNC right before the Render function

	AddressesOfMembers addr = addresses_;

	// setup buffers for the HLSL shader
	pDeviceContext->VSSetConstantBuffers(0, 1, addr.constBuffPerObjAddr);
	pDeviceContext->PSSetConstantBuffers(0, 1, addr.constBuffPerFrameAddr);
	pDeviceContext->PSSetConstantBuffers(1, 1, addr.constBuffRareChangedAddr);

	// Set the vertex input layout
	pDeviceContext->IASetInputLayout(addr.pVertexShaderInputLayout);

	// Set the vertex and pixels shaders that will be used to render the model
	pDeviceContext->VSSetShader(addr.pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(addr.pPixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, addr.ppSamplerState);


	// -----------------------------------------------------------------------------
	//                   UPDATE THE CONST BUFFER PER FRAME
	// -----------------------------------------------------------------------------

	// prepare data for the constant camera buffer
	constBuffPerFrame_.data.cameraPos = cameraPosition;

	// load camera position into GPU
	constBuffPerFrame_.ApplyChanges(pDeviceContext);


}

///////////////////////////////////////////////////////////

void TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext,
	const std::vector<DirectX::XMMATRIX> & worldMatrices, // each model has its own world matrix
	const DirectX::XMMATRIX & viewProj,                   // common view_matrix * proj_matrix
	const DirectX::XMMATRIX & texTransform,               // transformation for the textures
	const std::map<aiTextureType, ID3D11ShaderResourceView* const*> & textures,
	const UINT indexCount)
{
	try
	{
		// Sets the variables which are used within the vertex shader.
		// This function takes in a pointer to a texture resource and then assigns it to 
		// the shader using a texture resource pointer. Note that the texture has to be set 
		// before rendering of the buffer occurs.
		//
		// After all the preparations we render the model using the HLSL shader


		// we have the same texture transformation for all the current geometry objects
		constBuffPerObj_.data.texTransform = DirectX::XMMatrixTranspose(texTransform);

		// setup textures for the pixel shader
		pDeviceContext->PSSetShaderResources(0, 1, textures.at(aiTextureType_DIFFUSE));
		pDeviceContext->PSSetShaderResources(1, 1, textures.at(aiTextureType_LIGHTMAP));
		
		
		// -------------------------------------------------------------------------
		//          SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL
		// -------------------------------------------------------------------------

		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{
			// update data of the matrix const buffer
			constBuffPerObj_.data.world = DirectX::XMMatrixTranspose(worldMatrices[idx]);
			constBuffPerObj_.data.worldViewProj = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);

			// load matrices data into GPU
			constBuffPerObj_.ApplyChanges(pDeviceContext);

			// render the geometry
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't find a texture with particular type");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't find a texture with particular type");
	}

	return;
}



// ************************************************************************************
//                            PUBLIC UPDATING API
// ************************************************************************************
void TextureShaderClass::SwitchFog(ID3D11DeviceContext* pDeviceContext)
{
	// enable / disable rendering of the fog
	constBuffRareChanged_.data.fogEnabled = !(bool)constBuffRareChanged_.data.fogEnabled;
	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}

void TextureShaderClass::SwitchAplhaClipping(ID3D11DeviceContext* pDeviceContext)
{
	// enable / disable alpha clipping
	constBuffRareChanged_.data.useAlphaClip = !(bool)constBuffRareChanged_.data.useAlphaClip;
	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}

void TextureShaderClass::SetForParams(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3 & fogColor, 
	const float fogStart, 
	const float fogRange)
{
	// setup some for params
	constBuffRareChanged_.data.fogStart = fogStart;
	constBuffRareChanged_.data.fogRange = fogRange;
	constBuffRareChanged_.data.fogColor = fogColor;

	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}



///////////////////////////////////////////////////////////

const std::string & TextureShaderClass::GetShaderName() const
{
	return className_;
}






// ************************************************************************************
//                         PRIVATE MODIFICATION API
// ************************************************************************************

// initialized the vertex shader, pixel shader, input layout, and sampler;
void TextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	HRESULT hr = S_OK;
	bool result = false;
	const UINT layoutElemNum = 2;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[layoutElemNum];


	// --------------------------- INPUT LAYOUT DESC -------------------------------

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


	// ------------------------  SHADERS / SAMPLER STATE  --------------------------

	result = vertexShader_.Initialize(pDevice, vsFilename, polygonLayout, layoutElemNum);
	ASSERT_TRUE(result, "can't initialize the vertex shader");

	result = pixelShader_.Initialize(pDevice, psFilename);
	ASSERT_TRUE(result, "can't initialize the pixel shader");

	result = samplerState_.Initialize(pDevice);
	ASSERT_TRUE(result, "can't initialize the sampler state");



	// ---------------------------  CONSTANT BUFFERS  ------------------------------

	hr = constBuffPerObj_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the const buffer per object");

	hr = constBuffPerFrame_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the const buffer per frame");

	hr = constBuffRareChanged_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the const buffer rare changed");


	// ----------------  SETUP CONST BUFFER WITH DEFAULT PARAMS  -------------------

	constBuffRareChanged_.data.fogEnabled = 1.0f;         // by default the fog is enabled
	constBuffRareChanged_.data.useAlphaClip = 1.0f;       // by default we use alpha clipping
	constBuffRareChanged_.data.fogStart = 5.0f;           // where the fog starts
	constBuffRareChanged_.data.fogRange = 100.0f;         // distance from camera where geometry is fully fogged
	constBuffRareChanged_.data.fogColor = { 1, 1, 1 };

	// load data into GPU
	constBuffRareChanged_.ApplyChanges(pDeviceContext);


	// ----------------------  GET ADDRESSES OF MEMBERS  ---------------------------

	// later we will use all these addresses during rendering
	addresses_.ppSamplerState = samplerState_.GetAddressOf();
	addresses_.pVertexShader = vertexShader_.GetShader();
	addresses_.pPixelShader = pixelShader_.GetShader();
	addresses_.pVertexShaderInputLayout = vertexShader_.GetInputLayout();
	
	addresses_.constBuffPerObjAddr      = constBuffPerObj_.GetAddressOf();
	addresses_.constBuffPerFrameAddr    = constBuffPerFrame_.GetAddressOf();
	addresses_.constBuffRareChangedAddr = constBuffRareChanged_.GetAddressOf();

	return;
}
