// *********************************************************************************
// Filename:     LightShaderClass.cpp
// Description:  this class is needed for rendering 3D models, 
//               its texture, simple PARRALEL light on it using HLSL shaders.
// Created:      09.04.23
// *********************************************************************************
#include "LightShaderClass.h"
#include "shaderclass.h"

#include "../Common/MathHelper.h"
#include "../Common/Assert.h"
#include "../Common/Log.h"

#include <stdexcept>


namespace Render
{

using namespace DirectX;


LightShaderClass::LightShaderClass() : className_{ __func__ }
{
	//Log::Debug();
}

LightShaderClass::~LightShaderClass()
{
}



// *********************************************************************************
//
//                             PUBLIC METHODS                                       
//
// *********************************************************************************

bool LightShaderClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// initializer the shader class: load and compile HLSL shaders,
	// create sampler state, create const buffers and 
	// initialize the instanced buffer with default values

	try
	{
		const std::wstring vsFilename = ShaderClass::pathToShadersDir_ + L"LightVS.hlsl";
		const std::wstring psFilename = ShaderClass::pathToShadersDir_ + L"LightPS.hlsl";

		InitializeShaders(
			pDevice, 
			pDeviceContext,
			vsFilename.c_str(),
			psFilename.c_str());

		BuildInstancedBuffer(pDevice);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the light shader class");
		return false;
	}

	Log::Debug("is initialized");

	return true;
}

///////////////////////////////////////////////////////////

void LightShaderClass::UpdatePerFrame(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& viewProj,
	const DirectX::XMFLOAT3& cameraPos,
	const std::vector<DirLight>& dirLights,
	const std::vector<PointLight>& pointLights,
	const std::vector<SpotLight>& spotLights,
	const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	// update constant buffers for this frame

	// view * proj matrix must be already transposed
	cbvsPerFrame_.data.viewProj = viewProj;

	// update the camera pos
	cbpsPerFrame_.data.cameraPos = cameraPos;
	
	// update directional light sources
	for (u32 idx = 0; idx < (u32)cbpsRareChanged_.data.numOfDirLights; ++idx)
		cbpsPerFrame_.data.dirLights[idx] = dirLights[idx];

	// we want to copy the proper number of point lights
	size pointLightsCountLimit = ARRAYSIZE(cbpsPerFrame_.data.pointLights);
	size inputPointLightsCount = std::ssize(pointLights);
	size pointLightsCount = (inputPointLightsCount >= pointLightsCountLimit) ? pointLightsCountLimit : inputPointLightsCount;

	// update point light sources
	// NOTICE: a size of the point lights buffer must be >= than the number of actual point light sources
	for (size idx = 0; idx < pointLightsCount; ++idx)
		cbpsPerFrame_.data.pointLights[idx] = pointLights[idx];

	cbpsPerFrame_.data.spotLights = spotLights[0];   
	
	cbvsPerFrame_.ApplyChanges(pDeviceContext);
	cbpsPerFrame_.ApplyChanges(pDeviceContext);

	// ---------------------------------------------

	pDeviceContext->IASetPrimitiveTopology(topologyType);
	pDeviceContext->IASetInputLayout(vs_.GetInputLayout());

	pDeviceContext->VSSetShader(vs_.GetShader(), nullptr, 0);
	pDeviceContext->PSSetShader(pPS_->GetShader(), nullptr, 0);
	pDeviceContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	// setup constant buffers for the HLSL shaders
	ID3D11Buffer* psCBs[2] = { cbpsPerFrame_.Get(), cbpsRareChanged_.Get() };

	pDeviceContext->VSSetConstantBuffers(0, 1, cbvsPerFrame_.GetAddressOf());
	pDeviceContext->PSSetConstantBuffers(0, 2, psCBs);
}

///////////////////////////////////////////////////////////

void LightShaderClass::UpdateInstancedBuffer(
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<DirectX::XMMATRIX>& worlds,
	const std::vector<DirectX::XMMATRIX>& texTransforms,
	const std::vector<Material>& materials)
{
	try
	{
		const size expectElemCount = std::ssize(worlds);

		Assert::True(expectElemCount == std::ssize(texTransforms), "the number of world matrices must be equal to the number of texture transformations");
		Assert::True(expectElemCount == std::ssize(materials), "the number of world matrices must be equal to the number of materials");
		//Assert::True(expectElemCount == std::ssize(texSetIdxs), "the number of idxs to textures set is wrong");

		// map the instanced buffer to write to it
		D3D11_MAPPED_SUBRESOURCE mappedData;
		HRESULT hr = pDeviceContext->Map(pInstancedBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		Assert::NotFailed(hr, "can't map the instanced buffer");

		buffTypes::InstancedData* dataView = (buffTypes::InstancedData*)mappedData.pData;

		// write data into the subresource
		for (ptrdiff_t idx = 0; idx < std::ssize(worlds); ++idx)
		{
			dataView[idx].world = worlds[idx];
			dataView[idx].worldInvTranspose = MathHelper::InverseTranspose(worlds[idx]);
			dataView[idx].texTransform = texTransforms[idx];
			dataView[idx].material = materials[idx];
			//dataView[idx].texSetIdx = texSetIdxs[idx];
		}

		pDeviceContext->Unmap(pInstancedBuffer_, 0);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
	}
}


///////////////////////////////////////////////////////////

void LightShaderClass::Render(
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<ID3D11Buffer*>& ptrsMeshVB,                     // arr of ptrs to meshes vertex buffers
	const std::vector<ID3D11Buffer*>& ptrsMeshIB,                     // arr of ptrs to meshes index buffers
	const std::vector<ID3D11ShaderResourceView*>& texturesSRVs,
	const std::vector<ptrdiff_t>& numInstancesPerMesh,
	const std::vector<uint32_t>& enttsMaterialTexIdxs,
	const std::vector<uint32_t>& enttsPerTexSet,
	const std::vector<uint32_t>& indexCounts,
	const uint32_t numOfTexSet,
	const uint32_t vertexSize)
{
	const UINT meshesCount = static_cast<UINT>(std::ssize(ptrsMeshVB));
	const UINT stride[2] = { vertexSize, sizeof(buffTypes::InstancedData) };
	const UINT offset[2] = { 0,0 };

	// go through each buffer and render it
	for (UINT meshIdx = 0, instancesSetIdx = 0, renderedForThisMesh = 0, startInstanceLocation = 0;
		meshIdx < meshesCount;
		++meshIdx, renderedForThisMesh = 0)
	{
		// prepare input assembler (IA) stage before the rendering process
		ID3D11Buffer* vbs[2] = { ptrsMeshVB[meshIdx], pInstancedBuffer_};
		pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		pDeviceContext->IASetIndexBuffer(ptrsMeshIB[meshIdx], DXGI_FORMAT_R32_UINT, 0);

		// go through each material/texture set for this mesh and render instances
		for (; renderedForThisMesh < (numInstancesPerMesh[meshIdx]); ++instancesSetIdx)
		{
			// update textures for the current instances
			pDeviceContext->PSSetShaderResources(0U, 2U,
				texturesSRVs.data() + enttsMaterialTexIdxs[instancesSetIdx] * 2U);

			const UINT instanceCount = static_cast<UINT>(enttsPerTexSet[instancesSetIdx]);

			pDeviceContext->DrawIndexedInstanced(
				indexCounts[meshIdx],
				instanceCount,
				0U, 0U,                               
				startInstanceLocation);

			startInstanceLocation += instanceCount;
			renderedForThisMesh += instanceCount;
		}
	}
}


	




// **********************************************************************************
// 
//                          DEBUG CONTROL METHODS
// 
// **********************************************************************************

PixelShader* LightShaderClass::CreatePS_ForDebug(
	ID3D11DeviceContext* pDeviceContext,
	const std::string& funcName)
{
	// create a pixel shader for debugging by input funcName

	try
	{
		const std::wstring psFilename = ShaderClass::pathToShadersDir_ + L"LightPS.hlsl";
		ID3D11Device* pDevice = nullptr;
		pDeviceContext->GetDevice(&pDevice);

		// if there was some another debug PS we delete it
		SafeDelete(pDebugPS_);

		pDebugPS_ = new PixelShader();

		const bool result = pDebugPS_->Initialize(pDevice, psFilename, funcName);
		Assert::True(result, "can't initialize the pixel shader for debug: " + funcName);

		return pDebugPS_;
	}
	catch (const std::bad_alloc& e)
	{
		Log::Error(e.what());
		throw LIB_Exception("can't create a pixel shader for debug: " + funcName);
	}
}

///////////////////////////////////////////////////////////

void LightShaderClass::EnableDisableDebugNormals(ID3D11DeviceContext* pDeviceContext)
{
	// enable/disable using a normal vector values as color for the vertex?

	// if we used the debugging before
	if (cbpsRareChanged_.data.debugNormals)
	{
		TurnOffDebug();  // reset all the debug flags
		SetDefaultPS();
	}
	// we want to turn on normals debugging
	else
	{
		TurnOffDebug();
		cbpsRareChanged_.data.debugNormals = true;
		pPS_ = CreatePS_ForDebug(pDeviceContext, "PS_DebugNormals");
	}
}

///////////////////////////////////////////////////////////

void LightShaderClass::EnableDisableDebugTangents(ID3D11DeviceContext* pDeviceContext)
{
	// enable/disable using a tangent vector values as color for the vertex?
	
	// if we used the debugging before
	if (cbpsRareChanged_.data.debugTangents)
	{
		TurnOffDebug();  // reset all the debug flags
		SetDefaultPS();
	}
	// we want to turn on tangents debugging
	else
	{
		TurnOffDebug();
		cbpsRareChanged_.data.debugTangents = true;
		pPS_ = CreatePS_ForDebug(pDeviceContext, "PS_DebugTangents");
	}
}

///////////////////////////////////////////////////////////

void LightShaderClass::EnableDisableDebugBinormals(ID3D11DeviceContext* pDeviceContext)
{
	// enable/disable using a binormal vector values as color for the vertex?
	
	// if we used the debugging before
	if (cbpsRareChanged_.data.debugBinormals)
	{
		TurnOffDebug();  // reset all the debug flags
		SetDefaultPS();
	}
	// we want to turn on normals debugging
	else
	{
		TurnOffDebug();
		cbpsRareChanged_.data.debugBinormals = true;
		pPS_ = CreatePS_ForDebug(pDeviceContext, "PS_DebugBinormals");
	}
}



// **********************************************************************************
//                              EFFECTS CONTROL
// **********************************************************************************

void LightShaderClass::SwitchFogEffect(ID3D11DeviceContext* pDeviceContext)
{
	// do we use or not a fog effect?
	cbpsRareChanged_.data.fogEnabled = !(bool)cbpsRareChanged_.data.fogEnabled;
	cbpsRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::SetFogParams(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3& fogColor,
	const float fogStart,
	const float fogRange)
{
	// since fog is changed very rarely we use this separate function to 
	// control various fog params
	cbpsRareChanged_.data.fogColor = fogColor;
	cbpsRareChanged_.data.fogStart = fogStart;
	cbpsRareChanged_.data.fogRange = fogRange;

	cbpsRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::ChangeFlashLightState(ID3D11DeviceContext* pDeviceContext)
{
	// switch state of using the flashlight (so we turn it on or turn it off)
	cbpsRareChanged_.data.turnOnFlashLight = !(bool)cbpsRareChanged_.data.turnOnFlashLight;
	cbpsRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::SetDirLights(
	ID3D11DeviceContext* pDeviceContext, 
	const std::vector<DirLight>& dirLights)
{
	// update the const buffer with new directional lights data

	for (u32 idx = 0; idx < (u32)cbpsRareChanged_.data.numOfDirLights; ++idx)
		cbpsPerFrame_.data.dirLights[idx] = dirLights[idx];

	cbpsPerFrame_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void LightShaderClass::SetDirLightsCount(
	ID3D11DeviceContext* pDeviceContext,
	const UINT numOfLights)
{
	// set how many directional lights we used for lighting of the scene
	// NOTICE: the maximal number of directional light sources is 3

	Assert::True(numOfLights < 4, "you can't use more than 3 directional light sources");

	cbpsRareChanged_.data.numOfDirLights = (float)numOfLights;
	cbpsRareChanged_.ApplyChanges(pDeviceContext);
}



// *********************************************************************************
//
//                           PRIVATE FUNCTIONS                                       
//
// *********************************************************************************

void LightShaderClass::InitializeShaders(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	//
	// helps to initialize the HLSL shaders, layout, sampler state, and buffers
	//

	HRESULT hr = S_OK;
	bool result = false;

	const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
	{
		// per vertex data
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

		// per instance data
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"WORLD_INV_TRANSPOSE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD_INV_TRANSPOSE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD_INV_TRANSPOSE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD_INV_TRANSPOSE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"TEX_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEX_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 144, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEX_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 160, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEX_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 176, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"MATERIAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"MATERIAL", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"MATERIAL", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"MATERIAL", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

		{"TEX_SET_IDX", 0, DXGI_FORMAT_R8_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};


	// --------------------- SHADERS / SAMPLER STATE -------------------------- //

	result = vs_.Initialize(
		pDevice, 
		vsFilename, 
		inputLayoutDesc.data(), 
		(UINT)inputLayoutDesc.size());

	Assert::True(result, "can't initialize the vertex shader");

	// initialize the DEFAULT pixel shader
	result = psDefault_.Initialize(pDevice, psFilename);
	Assert::True(result, "can't initialize the pixel shader");

	result = samplerState_.Initialize(pDevice);
	Assert::True(result, "can't initialize the sampler state");

	// setup the current pixel shader
	pPS_ = &psDefault_;


	// ------------------------ CONSTANT BUFFERS ------------------------------ 

	hr = cbvsPerFrame_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't init a const buffer per frame for the vertex shader");

	hr = cbpsPerFrame_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't init a const buffer buffer per frame for the pixel shader");

	hr = cbpsRareChanged_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't init a const buffer for rarely changed data for the pixel shader");


	// -------------  SETUP CONST BUFFERS WITH DEFAULT PARAMS  ----------------

	// pixel shader: setup fog params with default params
	cbpsRareChanged_.data.fogColor = { 0.5f, 0.5f, 0.5f };
	cbpsRareChanged_.data.fogStart = 15.0f;
	cbpsRareChanged_.data.fogRange = 250.0f;

	// pixel shader: setup controlling flags
	cbpsRareChanged_.data.debugMode = 0;
	cbpsRareChanged_.data.debugNormals = 0;
	cbpsRareChanged_.data.debugTangents = 0;
	cbpsRareChanged_.data.debugBinormals = 0;
	cbpsRareChanged_.data.fogEnabled = 1;
	cbpsRareChanged_.data.turnOnFlashLight = 1;

	// load rare changed data into GPU
	cbpsRareChanged_.ApplyChanges(pDeviceContext);

	return;
}

///////////////////////////////////////////////////////////

void LightShaderClass::BuildInstancedBuffer(ID3D11Device* pDevice)
{
	// setup the volume of the buffer
	const int n = 5;
	instancedData_.resize(n * n * n);

	D3D11_BUFFER_DESC vbd;

	// setup buffer's description
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = static_cast<UINT>(sizeof(buffTypes::InstancedData) * std::ssize(instancedData_));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	const HRESULT hr = pDevice->CreateBuffer(&vbd, nullptr, &pInstancedBuffer_);
	Assert::NotFailed(hr, "can't create an instanced buffer");
}


} // namespace Render