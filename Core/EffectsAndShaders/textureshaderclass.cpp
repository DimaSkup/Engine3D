////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"

using namespace DirectX;


TextureShaderClass::TextureShaderClass() 
	: className_{__func__}
{
	Log::Debug(LOG_MACRO);
}

TextureShaderClass::~TextureShaderClass() 
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
	const DirectX::XMFLOAT3 & cameraPosition,
	ID3D11Buffer* pMeshVB,
	ID3D11Buffer* pMeshIB,
	D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	// prepare the shaders for rendering; setup some data for it;
	// 
	// Note: YOU MUST CALL THIS FUNC right before the Render function

	// prepare input assembler (IA) stage before the rendering process
	const UINT stride[2] = { sizeof(Basic32), sizeof(InstancedData) };
	const UINT offset[2] = { 0,0 };

	ID3D11Buffer* vbs[2] = { pMeshVB, pInstancedBuffer_ };

	pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);

	// set what primitive topology we want to use to render this vertex buffer
	pDeviceContext->IASetPrimitiveTopology(topologyType);

	pDeviceContext->IASetIndexBuffer(
		pMeshIB,                           // pIndexBuffer
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT, // format of the indices
		0);                                // offset, in bytes

	AddressesOfMembers addr = addresses_;

	// setup buffers for the HLSL shader
	pDeviceContext->VSSetConstantBuffers(0, 1, addr.constBuffPerObjAddr);
	pDeviceContext->PSSetConstantBuffers(0, 1, addr.constBuffPerFrameAddr);
	pDeviceContext->PSSetConstantBuffers(1, 1, addr.constBuffRareChangedAddr);

	// Set the vertex input layout
	pDeviceContext->IASetInputLayout(vertexShader_.GetInputLayout());

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
	const std::vector<DirectX::XMMATRIX> & worldMatrices,            // each model has its own world matrix
	const DirectX::XMMATRIX & viewProj,                              // common view_matrix * proj_matrix
	const std::vector<DirectX::XMMATRIX> & texTransforms,            // each geometry object has own transformation for its textures
	const std::vector<ID3D11ShaderResourceView* const*> & textures,
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
		

		// setup textures for the pixel shader
		pDeviceContext->PSSetShaderResources(0, 1, textures[aiTextureType_DIFFUSE]);
		pDeviceContext->PSSetShaderResources(1, 1, textures[aiTextureType_LIGHTMAP]);
		//pDeviceContext->PSSetShaderResources(1, 1, textures[aiTextureType_LIGHTMAP]);
		
		
		// -------------------------------------------------------------------------
		//          SETUP SHADER PARAMS WHICH ARE DIFFERENT FOR EACH MODEL
		// -------------------------------------------------------------------------

#if 0
		for (UINT idx = 0; idx < worldMatrices.size(); ++idx)
		{
			// update data of the matrix const buffer
			//constBuffPerObj_.data.world = DirectX::XMMatrixTranspose(worldMatrices[idx]);
			//constBuffPerObj_.data.worldViewProj = DirectX::XMMatrixTranspose(worldMatrices[idx] * viewProj);
			constBuffPerObj_.data.texTransform = texTransforms[idx]; // NOTE: the texture transform must be already transposed

			// load matrices data into GPU
			constBuffPerObj_.ApplyChanges(pDeviceContext);

			// render the geometry
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
#endif 


		pDeviceContext->DrawIndexedInstanced(
			indexCount,             // number of indices in the mesh
			worldMatrices.size(),   // number of instances to draw
			0, 0, 0);
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

///////////////////////////////////////////////////////////

void TextureShaderClass::SwitchAlphaClipping(ID3D11DeviceContext* pDeviceContext)
{
	// enable / disable alpha clipping
	constBuffRareChanged_.data.useAlphaClip = !(bool)constBuffRareChanged_.data.useAlphaClip;
	constBuffRareChanged_.ApplyChanges(pDeviceContext);
}

///////////////////////////////////////////////////////////

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
	const UINT layoutElemNum = 6;
	const D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[layoutElemNum] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	// ------------------------  SHADERS / SAMPLER STATE  --------------------------

	result = vertexShader_.Initialize(pDevice, vsFilename, inputLayoutDesc, layoutElemNum);
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

///////////////////////////////////////////////////////////

void TextureShaderClass::BuildInstancedBuffer(ID3D11Device* pDevice)
{
	const int n = 5;
	instancedData_.resize(n * n * n);

	const float width = 200.0f;
	const float height = 200.0f;
	const float depth = 200.0f;

	const float x = -0.5f * width;
	const float y = -0.5f * height;
	const float z = -0.5f * depth;

	const float inv_n_sub_1 = 1.0f / (n - 1);
	const float dx = width * inv_n_sub_1;
	const float dy = height * inv_n_sub_1;
	const float dz = depth * inv_n_sub_1;

	for (int k = 0; k < n; ++k)
	{
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				const UINT idx = (k*n*n) + (i*n) + j;

				// position instanced along a 3D grid
				instancedData_[idx].world = XMFLOAT4X4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					x+j*dx, y+i*dy, z+k*dz, 1.0f); 
			}
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstancedData) * std::ssize(instancedData_);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	const HRESULT hr = pDevice->CreateBuffer(&vbd, nullptr, &pInstancedBuffer_);
}