////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"
#include "shaderclass.h"

#include "../Common/Log.h"
#include "../Common/Types.h"
#include "../Common/MathHelper.h"
#include "../Common/Assert.h"


namespace Render
{

using namespace DirectX;

TextureShaderClass::TextureShaderClass() : className_{__func__}
{
	//Log::Debug();
}

TextureShaderClass::~TextureShaderClass() 
{
	SafeRelease(&pInstancedBuffer_);
	instancedData_.clear();
}


// ************************************************************************************
// 
//                           PUBLIC MODIFICATION API
// 
// ************************************************************************************

bool TextureShaderClass::Initialize(
	ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext)
{
	// initializer the shader class: load and compile HLSL shaders and 
	// initialize the instanced buffer with default values

	try
	{
		const std::wstring vsFilename = ShaderClass::pathToShadersDir_ + L"textureVS.hlsl";
		const std::wstring psFilename = ShaderClass::pathToShadersDir_ + L"texturePS.hlsl";

		InitializeShaders(pDevice, pDeviceContext, vsFilename.c_str(), psFilename.c_str());
		BuildInstancedBuffer(pDevice);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the texture shader class");
		return false;
	}

	Log::Debug("is initialized");

	return true;
}


// ************************************************************************************
// 
//                             PUBLIC RENDERING API
// 
// ************************************************************************************

void TextureShaderClass::UpdatePerFrame(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& viewProj,
	const DirectX::XMFLOAT3& cameraPosition,
	const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	// prepare the shaders for rendering; setup some data for it;
	// 
	// Note: YOU MUST CALL THIS FUNC right before the Render function

	AddressesOfMembers addr = addresses_;

	pDeviceContext->IASetPrimitiveTopology(topologyType);
	pDeviceContext->IASetInputLayout(vs_.GetInputLayout());

	pDeviceContext->VSSetShader(addr.pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(addr.pPixelShader, nullptr, 0);
	pDeviceContext->PSSetSamplers(0, 1, addr.ppSamplerState);

	// prepare data for the constant camera buffer and load this data into GPU
	cbPerFrame_.data.cameraPos = cameraPosition;
	cbPerFrame_.data.viewProj = DirectX::XMMatrixTranspose(viewProj);
	cbPerFrame_.ApplyChanges(pDeviceContext);

	// setup constant buffers for the HLSL shaders
	ID3D11Buffer* cbs[2] = { cbPerFrame_.Get(), cbRareChanged_.Get() };

	pDeviceContext->VSSetConstantBuffers(0, 1, cbPerFrame_.GetAddressOf());
	pDeviceContext->PSSetConstantBuffers(0, 2, cbs);

}

///////////////////////////////////////////////////////////

void TextureShaderClass::UpdateInstancedBuffer(
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<DirectX::XMMATRIX>& worlds,
	const std::vector<DirectX::XMMATRIX>& texTransforms)
{
	assert(std::ssize(worlds) == std::ssize(texTransforms) && "the number of world matrices and texture transformations must be equal");

	// map the instanced buffer to write to it
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = pDeviceContext->Map(pInstancedBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	Assert::NotFailed(hr, "can't map the instanced buffer");

	InstancedData* dataView = (InstancedData*)mappedData.pData;
	const size instancesCount = std::ssize(worlds);

	// write data into the subresource
	for (ptrdiff_t idx = 0; idx < instancesCount; ++idx)
		dataView[idx].world = worlds[idx];

	for (ptrdiff_t idx = 0; idx < instancesCount; ++idx)
		dataView[idx].texTransform = texTransforms[idx];

	pDeviceContext->Unmap(pInstancedBuffer_, 0);
}

///////////////////////////////////////////////////////////

void TextureShaderClass::Render(
	ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* pMeshVB,
	ID3D11Buffer* pMeshIB,
	const std::vector<ID3D11ShaderResourceView*>& texturesSRVs,
	const std::vector<UINT>& instancesCountsPerTexSet,
	const uint32_t indexCount,
	const uint32_t vertexSize)
{
	Assert::NotNullptr(pMeshVB, "a ptr to the mesh vertex buffer == nullptr");
	Assert::NotNullptr(pMeshIB, "a ptr to the mesh index buffer == nullptr");
		
	// prepare input assembler (IA) stage before the rendering process
	const UINT stride[2] = { vertexSize, sizeof(InstancedData) };
	const UINT offset[2] = { 0,0 };

	ID3D11Buffer* vbs[2] = { pMeshVB, pInstancedBuffer_ };

	pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
	pDeviceContext->IASetIndexBuffer(pMeshIB, DXGI_FORMAT_R32_UINT, 0);
		
		
	for (UINT idx = 0, startInstanceLocation = 0;
		const UINT instancesCount : instancesCountsPerTexSet)
	{
		// set textures for this batch of instances
		pDeviceContext->PSSetShaderResources(
			0,
			22U,
			texturesSRVs.data() + (idx * 22));

		pDeviceContext->DrawIndexedInstanced(
			indexCount,
			instancesCount,
			0,                               // start index location
			0,                               // base vertex location
			startInstanceLocation);

		startInstanceLocation += instancesCount;
		++idx;
	}
}



// ********************************************************************************
// 
//                            PUBLIC UPDATING API
// 
// ********************************************************************************

void TextureShaderClass::SwitchFog(ID3D11DeviceContext* pDeviceContext)
{
	// enable / disable rendering of the fog
	cbRareChanged_.data.fogEnabled = !(bool)cbRareChanged_.data.fogEnabled;
	cbRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void TextureShaderClass::SwitchAlphaClipping(ID3D11DeviceContext* pDeviceContext)
{
	// enable / disable alpha clipping
	cbRareChanged_.data.useAlphaClip = !(bool)cbRareChanged_.data.useAlphaClip;
	cbRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

void TextureShaderClass::SetFogParams(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMFLOAT3 & fogColor, 
	const float fogStart, 
	const float fogRange)
{
	// setup some for params
	cbRareChanged_.data.fogStart = fogStart;
	cbRareChanged_.data.fogRange = fogRange;
	cbRareChanged_.data.fogColor = fogColor;

	cbRareChanged_.ApplyChanges(pDeviceContext);
}



// *********************************************************************************
// 
//                         PRIVATE MODIFICATION API
// 
// *********************************************************************************

void TextureShaderClass::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename, 
	const WCHAR* psFilename)
{
	// initialized the vertex shader, pixel shader, input layout, 
	// sampler state, and different constant buffers

	HRESULT hr = S_OK;
	bool result = false;

	const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXTRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXTRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXTRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXTRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	// ------------------------  SHADERS / SAMPLER STATE  --------------------------

	result = vs_.Initialize(
		pDevice, 
		vsFilename, 
		inputLayoutDesc.data(), 
		(UINT)inputLayoutDesc.size());

	Assert::True(result, "can't initialize the vertex shader");

	result = pixelShader_.Initialize(pDevice, psFilename);
	Assert::True(result, "can't initialize the pixel shader");

	result = samplerState_.Initialize(pDevice);
	Assert::True(result, "can't initialize the sampler state");



	// ---------------------------  CONSTANT BUFFERS  ------------------------------

	hr = cbPerFrame_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't initialize the const buffer per frame");

	hr = cbRareChanged_.Initialize(pDevice, pDeviceContext);
	Assert::NotFailed(hr, "can't initialize the const buffer rare changed");


	// ----------------  SETUP CONST BUFFER WITH DEFAULT PARAMS  -------------------

	
	cbRareChanged_.data.fogEnabled = 1.0f;         // by default the fog is enabled
	cbRareChanged_.data.useAlphaClip = 1.0f;       // by default we use alpha clipping
	cbRareChanged_.data.fogStart = 5.0f;           // where the fog starts
	cbRareChanged_.data.fogRange = 100.0f;         // distance from camera where geometry is fully fogged
	cbRareChanged_.data.fogColor = { 1, 1, 1 };

	// load data into GPU
	cbRareChanged_.ApplyChanges(pDeviceContext);


	// ----------------------  GET ADDRESSES OF MEMBERS  ---------------------------

	// later we will use all these addresses during rendering
	addresses_.ppSamplerState = samplerState_.GetAddressOf();
	addresses_.pVertexShader = vs_.GetShader();
	addresses_.pPixelShader = pixelShader_.GetShader();
	addresses_.pVertexShaderInputLayout = vs_.GetInputLayout();
	
	//addresses_.constBuffPerObjAddr      = constBuffPerObj_.GetAddressOf();
	addresses_.constBuffPerFrameAddr    = cbPerFrame_.GetAddressOf();
	addresses_.constBuffRareChangedAddr = cbRareChanged_.GetAddressOf();

	return;
}

///////////////////////////////////////////////////////////

void TextureShaderClass::BuildInstancedBuffer(ID3D11Device* pDevice)
{
	const int n = 5;
	instancedData_.resize(n * n * n);

	const float width  = 50.0f;
	const float height = 50.0f;
	const float depth  = 50.0f;

	const float x = -0.5f * width;
	const float y = -0.5f * height;
	const float z = -0.5f * depth;

	const float inv_n_sub_1 = 1.0f / (n - 1);
	const float dx = width  * inv_n_sub_1;
	const float dy = height * inv_n_sub_1;
	const float dz = depth  * inv_n_sub_1;

	for (int k = 0; k < n; ++k)
	{
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				const UINT idx = (k*n*n) + (i*n) + j;

				// position instanced along a 3D grid
				instancedData_[idx].world = XMMATRIX(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					x+j*dx, y+i*dy, z+k*dz, 1.0f); 

				instancedData_[idx].texTransform = DirectX::XMMatrixScaling(MathHelper::RandF(), 1.0f, 1.0f);

				// random color
				//instancedData_[idx].color = MathHelper::RandColorRGBA();
			}
		}
	}

	D3D11_BUFFER_DESC vbd;
	D3D11_SUBRESOURCE_DATA vbData;

	// setup buffer's description
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = static_cast<UINT>(sizeof(InstancedData) * std::ssize(instancedData_));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// setup buffer's initial data
	vbData.pSysMem = instancedData_.data();
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	const HRESULT hr = pDevice->CreateBuffer(&vbd, nullptr, &pInstancedBuffer_);
	Assert::NotFailed(hr, "can't create an instanced buffer");
}

} // namespace Render