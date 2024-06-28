////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.cpp
// Description:  this class is needed for rendering 3D models, 
//               its texture, simple PARRALEL light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#include "LightShaderClass.h"
#include "../Common/MathHelper.h"

LightShaderClass::LightShaderClass()
	: className_{ __func__ }
{
	Log::Debug(LOG_MACRO);
}

LightShaderClass::~LightShaderClass()
{
}



// *********************************************************************************
//
//                           PUBLIC FUNCTIONS                                       
//
// *********************************************************************************


bool LightShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// Initializes the shaders for rendering of the lit geometry objects

	try
	{
		const WCHAR* vsFilename = L"shaders/LightVS.hlsl";
		const WCHAR* psFilename = L"shaders/LightPS.hlsl";

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
}

///////////////////////////////////////////////////////////

void LightShaderClass::PrepareForRendering(ID3D11DeviceContext* pDeviceContext)
{
#if 0
	// set the input layout 
	pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

	// set vertex and pixel shaders for rendering
	pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0);

	// set the sampler state for the pixel shader
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// set a ptr to the constant buffer with rare changed params
	pDeviceContext->PSSetConstantBuffers(2, 1, constBuffRareChanged_.GetAddressOf());
#endif
}

///////////////////////////////////////////////////////////

void LightShaderClass::SetLights(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3 & cameraPos,               // eyePos
	const std::vector<DirectionalLight> & dirLights,
	const std::vector<PointLight> & pointLights,
	const std::vector<SpotLight> & spotLights)
{
	//
	// prepare light sources of different types for rendering using HLSL shaders
	// NOTE: this function is supposed to be called only once per frame
	//

	// set new data for the constant buffer per frame
	constBuffPerFrame_.data.dirLights[0] = dirLights[0];
	constBuffPerFrame_.data.dirLights[1] = dirLights[1];
	constBuffPerFrame_.data.dirLights[2] = dirLights[2];
	constBuffPerFrame_.data.pointLights = pointLights[0];
	constBuffPerFrame_.data.spotLights = spotLights[0];
	constBuffPerFrame_.data.cameraPosW = cameraPos;

	// load new data into GPU
	constBuffPerFrame_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::RenderGeometry(
	ID3D11DeviceContext* pDeviceContext,
	const Material& material,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMMATRIX & texTransform,
	const std::vector<DirectX::XMMATRIX> & worldMatrices,
	const std::vector<ID3D11ShaderResourceView* const*>& textures,
	const UINT indexCount)
{
	// THIS FUNC setups the rendering pipeline and 
	// renders geometry objects onto the screen

	try
	{
		// -------------------------------------------------------------------------
		//         SETUP SHADER PARAMS WHICH ARE THE SAME FOR EACH MODEL
		// -------------------------------------------------------------------------
	
		// set the input layout 
		pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

		// set vertex and pixel shaders for rendering
		pDeviceContext->VSSetShader(vertexShader_.GetShader(), nullptr, 0);
		pDeviceContext->PSSetShader(pPixelShader_->GetShader(), nullptr, 0);

		// set the sampler state for the pixel shader
		pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

		// we have the same texture transformation for all the current geometry objects
		constBuffPerObj_.data.texTransform = DirectX::XMMatrixTranspose(texTransform);

		// set constant buffer for rendering
		pDeviceContext->VSSetConstantBuffers(0, 1, constBuffPerObj_.GetAddressOf());
		pDeviceContext->PSSetConstantBuffers(0, 1, constBuffPerObj_.GetAddressOf());
		pDeviceContext->PSSetConstantBuffers(1, 1, constBuffPerFrame_.GetAddressOf());
		pDeviceContext->PSSetConstantBuffers(2, 1, constBuffRareChanged_.GetAddressOf());

		// -------------------------------------------------------------------------
		//                     PIXEL SHADER: SET TEXTURES
		// -------------------------------------------------------------------------
		pDeviceContext->PSSetShaderResources(0, 1, textures[aiTextureType_DIFFUSE]);
		pDeviceContext->PSSetShaderResources(1, 1, textures[aiTextureType_LIGHTMAP]);
		pDeviceContext->PSSetShaderResources(2, 1, textures[aiTextureType_HEIGHT]);
		pDeviceContext->PSSetShaderResources(3, 1, textures[aiTextureType_SPECULAR]);

		// -------------------------------------------------------------------------
		// SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL AND RENDER MODELS
		// -------------------------------------------------------------------------
		
		constBuffPerObj_.data.material = material;

		// go through each model, prepare its data for rendering using HLSL shaders
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{
			// set new data for the constant buffer per object
			constBuffPerObj_.data.world             = DirectX::XMMatrixTranspose(worldMatrices[idx]);
			constBuffPerObj_.data.worldInvTranspose = MathHelper::InverseTranspose(worldMatrices[idx]);
			constBuffPerObj_.data.worldViewProj     = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);

			// load new data into GPU
			constBuffPerObj_.ApplyChanges(pDeviceContext);

			// render geometry
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("went out of range for some mesh");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't render using the light shader");
	}
}

///////////////////////////////////////////////////////////

const std::string & LightShaderClass::GetShaderName() const
{
	return className_;
}



// **********************************************************************************
//                               DEBUG CONTROL
// **********************************************************************************

PixelShader* LightShaderClass::CreatePS_ForDebug(
	ID3D11DeviceContext* pDeviceContext,
	const std::string& funcName)
{
	// create a pixel shader for debugging by input funcName

	try
	{
		const WCHAR* psFilename = L"shaders/LightPS.hlsl";
		ID3D11Device* pDevice = nullptr;
		pDeviceContext->GetDevice(&pDevice);

		// if there was some another debug PS we delete it
		_DELETE(pPixelShaderForDebug_);

		pPixelShaderForDebug_ = new PixelShader();

		const bool result = pPixelShaderForDebug_->Initialize(pDevice, psFilename, funcName);
		ASSERT_TRUE(result, "can't initialize the pixel shader for debug: " + funcName);

		return pPixelShaderForDebug_;
	}
	catch (const std::bad_alloc& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't create a pixel shader for debug: " + funcName);
	}
}

void LightShaderClass::EnableDisableDebugNormals(ID3D11DeviceContext* pDeviceContext)
{
	// enable/disable using a normal vector values as color for the vertex?

	// if we used the debugging before
	if (constBuffRareChanged_.data.debugNormals)
	{
		TurnOffDebug();  // reset all the debug flags
		SetDefaultPS();
	}
	// we want to turn on normals debugging
	else
	{
		TurnOffDebug();
		constBuffRareChanged_.data.debugNormals = true;
		pPixelShader_ = CreatePS_ForDebug(pDeviceContext, "PS_DebugNormals");
	}
}

///////////////////////////////////////////////////////////

void LightShaderClass::EnableDisableDebugTangents(ID3D11DeviceContext* pDeviceContext)
{
	// enable/disable using a tangent vector values as color for the vertex?
	
	// if we used the debugging before
	if (constBuffRareChanged_.data.debugTangents)
	{
		TurnOffDebug();  // reset all the debug flags
		SetDefaultPS();
	}
	// we want to turn on tangents debugging
	else
	{
		TurnOffDebug();
		constBuffRareChanged_.data.debugTangents = true;
		pPixelShader_ = CreatePS_ForDebug(pDeviceContext, "PS_DebugTangents");
	}
}

///////////////////////////////////////////////////////////

void LightShaderClass::EnableDisableDebugBinormals(ID3D11DeviceContext* pDeviceContext)
{
	// enable/disable using a binormal vector values as color for the vertex?
	
	// if we used the debugging before
	if (constBuffRareChanged_.data.debugBinormals)
	{
		TurnOffDebug();  // reset all the debug flags
		SetDefaultPS();
	}
	// we want to turn on normals debugging
	else
	{
		TurnOffDebug();
		constBuffRareChanged_.data.debugBinormals = true;
		pPixelShader_ = CreatePS_ForDebug(pDeviceContext, "PS_DebugBinormals");
	}
}



// **********************************************************************************
//                               EFFECTS CONTROL
// **********************************************************************************


void LightShaderClass::EnableDisableFogEffect(ID3D11DeviceContext* pDeviceContext)
{
	// do we use or not a fog effect?
	constBuffRareChanged_.data.fogEnabled = !(bool)constBuffRareChanged_.data.fogEnabled;
	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::SetFogParams(
	const float fogStart,
	const float fogRange,
	const DirectX::XMFLOAT3& fogColor)
{
	// since fog is changed very rarely we use this separate function to 
	// control various fog params
	assert(0 && "TODO: implement it!");
}

///////////////////////////////////////////////////////////

void LightShaderClass::ChangeFlashLightState(ID3D11DeviceContext* pDeviceContext)
{
	// switch state of using the flashlight (so we turn it on or turn it off)
	constBuffRareChanged_.data.turnOnFlashLight = !(bool)constBuffRareChanged_.data.turnOnFlashLight;
	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::SetNumberOfDirectionalLights_ForRendering(
	ID3D11DeviceContext* pDeviceContext,
	const UINT numOfLights)
{
	// set how many directional lights we used for lighting of the scene
	// NOTICE: the maximal number of directional light sources is 3

	ASSERT_TRUE(numOfLights < 4, "you can't use more than 3 directional light sources");

	constBuffRareChanged_.data.numOfDirLights = (float)numOfLights;
	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}



// *********************************************************************************
//
//                           PRIVATE FUNCTIONS                                       
//
// *********************************************************************************

void LightShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	//
	// helps to initialize the HLSL shaders, layout, sampler state, and buffers
	//

	bool result = false;
	const UINT layoutElemNum = 5;                       // the number of the input layout elements
	D3D11_INPUT_ELEMENT_DESC layoutDesc[layoutElemNum]; // description for the vertex input layout
	HRESULT hr = S_OK;

	// set the description for the input layout
	layoutDesc[0].SemanticName = "POSITION";
	layoutDesc[0].SemanticIndex = 0;
	layoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[0].InputSlot = 0;
	layoutDesc[0].AlignedByteOffset = 0;
	layoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[0].InstanceDataStepRate = 0;

#if 0
	layoutDesc[1].SemanticName = "NORMAL";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT2);
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;
#endif

	layoutDesc[1].SemanticName = "TEXCOORD";
	layoutDesc[1].SemanticIndex = 0;
	layoutDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layoutDesc[1].InputSlot = 0;
	layoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[1].InstanceDataStepRate = 0;

	layoutDesc[2].SemanticName = "NORMAL";
	layoutDesc[2].SemanticIndex = 0;
	layoutDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[2].InputSlot = 0;
	layoutDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[2].InstanceDataStepRate = 0;

	layoutDesc[3].SemanticName = "TANGENT";
	layoutDesc[3].SemanticIndex = 0;
	layoutDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[3].InputSlot = 0;
	layoutDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[3].InstanceDataStepRate = 0;

	layoutDesc[4].SemanticName = "BINORMAL";
	layoutDesc[4].SemanticIndex = 0;
	layoutDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layoutDesc[4].InputSlot = 0;
	layoutDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layoutDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layoutDesc[4].InstanceDataStepRate = 0;

	


	// --------------------- SHADERS / SAMPLER STATE -------------------------- //

	// initialize the vertex shader
	result = vertexShader_.Initialize(pDevice, vsFilename, layoutDesc, layoutElemNum);
	ASSERT_TRUE(result, "can't initialize the light vertex shader");

	// initialize the DEFAULT pixel shader
	result = psDefault_.Initialize(pDevice, psFilename);
	ASSERT_TRUE(result, "can't initialize the light pixel shader");

	// initialize the sampler state
	result = samplerState_.Initialize(pDevice);
	ASSERT_TRUE(result, "can't initialize the sampler state");

	// setup the current pixel shader
	pPixelShader_ = &psDefault_;


	// ------------------------ CONSTANT BUFFERS ------------------------------ //

	// initialize the constant buffer for data which is changed per object
	hr = constBuffPerObj_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the constant per object buffer");

	// initialize the constant buffer for data which is changed each frame
	hr = constBuffPerFrame_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the constant per frame buffer");

	// initialize the constant buffer for data which is changed each frame
	hr = constBuffRareChanged_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the constant buffer for rarely changed data");

	// ------------------------------------------------------------------------ //

	// setup fog params with default params
	const float fogStart = 5;
	const float fogRange = 100;
	const DirectX::XMFLOAT3 fogColor{ 0.5f, 0.5f, 0.5f };

	//this->SetFogParams(fogStart, fogRange, fogColor);

	constBuffRareChanged_.data.debugMode = 0;
	constBuffRareChanged_.data.debugNormals = 0;
	constBuffRareChanged_.data.debugTangents = 0;
	constBuffRareChanged_.data.debugBinormals = 0;
	constBuffRareChanged_.data.fogEnabled = 1;
	constBuffRareChanged_.data.turnOnFlashLight = 1;

	// load rare changed data into GPU
	constBuffRareChanged_.ApplyChanges(pDeviceContext);

	return;
}



