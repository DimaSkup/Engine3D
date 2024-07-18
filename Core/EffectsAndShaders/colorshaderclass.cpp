/////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
// Revising: 06.04.22
/////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"
#include "../GameObjects/Vertex.h"
#include "../Common/MathHelper.h"

#include "../Engine/macros.h"
#include "../Engine/Log.h"


ColorShaderClass::ColorShaderClass() : className_ { __func__ }
{
	Log::Debug(LOG_MACRO);
}

ColorShaderClass::~ColorShaderClass()
{
	_RELEASE(pInstancedBuffer_);
	instancedData_.clear();
}


// ------------------------------------------------------------------------------ //
//
//                         PUBLIC FUNCTIONS
//
// ------------------------------------------------------------------------------ //

bool ColorShaderClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	// THIS FUNCTION initializes the ColorShaderClass; 
	// creates a vertex and pixel shader, input layout, and const buffer 

	try
	{
		const WCHAR* vsFilename = L"shaders/colorVS.hlsl";
		const WCHAR* psFilename = L"shaders/colorPS.hlsl";

		InitializeShaders(pDevice, pDeviceContext, vsFilename, psFilename);
		BuildInstancedBuffer(pDevice);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the color shader class");
		return false;
	}

	Log::Debug(LOG_MACRO, "is initialized");

	return true;
}

///////////////////////////////////////////////////////////

void ColorShaderClass::Prepare(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& viewProj,
	const float totalGameTime)           // time passed since the start of the application
{
	// prepare the shader for rendering this frame

	pDeviceContext->IASetInputLayout(vs_.GetInputLayout());
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeviceContext->VSSetShader(vs_.GetShader(), nullptr, 0U);
	pDeviceContext->PSSetShader(pixelShader_.GetShader(), nullptr, 0U);

	// update constant buffers 
	constBufferPerFrame_.data.viewProj = DirectX::XMMatrixTranspose(viewProj);
	constBufferPerFrame_.ApplyChanges(pDeviceContext);

	// setup constant buffer for shaders
	pDeviceContext->VSSetConstantBuffers(0, 1, constBufferPerFrame_.GetAddressOf());
}

///////////////////////////////////////////////////////////

void ColorShaderClass::Render(
	ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* pMeshVB,
	ID3D11Buffer* pMeshIB,
	const UINT indexCount)            // time passed since the start of the application
{
	// THIS FUNCTION renders the input vertex buffer with some color

	assert((pMeshVB != nullptr) && "ptr to the mesh vertex buffer == nullptr");
	assert((pMeshIB != nullptr) && "ptr to the mesh index buffer == nullptr");
	
	try
	{
		const UINT stride[2] = { sizeof(VERTEX), sizeof(InstancedData) };
		const UINT offset[2] = {0,0};
	
		ID3D11Buffer* vbs[2] = { pMeshVB, pInstancedBuffer_ };

		pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		pDeviceContext->IASetIndexBuffer(pMeshIB, DXGI_FORMAT_R32_UINT, 0);

		pDeviceContext->DrawIndexedInstanced(
			indexCount,
			(UINT)instancedData_.size(),
			0, 0, 0);
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't render geometry");
	}
}



// ------------------------------------------------------------------------------ //
//
//                         PRIVATE FUNCTIONS
//
// ------------------------------------------------------------------------------ //

void ColorShaderClass::InitializeShaders(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	// Initializes the shaders, input vertex layout and constant matrix buffer.
	// This function is called from the Initialize() function

	HRESULT hr = S_OK;
	bool result = false;

	// sum of the structures sizes of:
	// position (float3) + 
	// texture (float2) + 
	// normal (float3) +
	// tangent (float3) + 
	// binormal (float3);
	// (look at the the VERTEX structure)
	
	//const UINT colorOffset = (4 * sizeof(DirectX::XMFLOAT3)) + sizeof(DirectX::XMFLOAT2);

	
	const UINT layoutElemNum = 6;
	const D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[layoutElemNum] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};


	// --------------------- SHADERS / SAMPLER STATE -------------------------- //

	// initialize the vertex shader
	result = vs_.Initialize(pDevice, vsFilename, inputLayoutDesc, layoutElemNum);
	ASSERT_TRUE(result, "can't initialize the sky dome vertex shader");

	// initialize the pixel shader
	result = pixelShader_.Initialize(pDevice, psFilename);
	ASSERT_TRUE(result, "can't initialize the sky dome pixel shader");

	// ------------------------ CONSTANT BUFFERS ------------------------------ //

	hr = constBufferPerFrame_.Initialize(pDevice, pDeviceContext);
	ASSERT_NOT_FAILED(hr, "can't initialize the constant buffer");


	// --------------------- SETUP CONSTANT BUFFERS --------------------------- //

	constBufferPerFrame_.data.viewProj = DirectX::XMMatrixIdentity();
}

///////////////////////////////////////////////////////////

void ColorShaderClass::BuildInstancedBuffer(ID3D11Device* pDevice)
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
	const float dx = width * inv_n_sub_1;
	const float dy = height * inv_n_sub_1;
	const float dz = depth * inv_n_sub_1;

	for (int k = 0; k < n; ++k)
	{
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				const UINT idx = (k * n * n) + (i * n) + j;

				// position instanced along a 3D grid
				instancedData_[idx].world = XMFLOAT4X4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					x + j * dx, y + i * dy, z + k * dz, 1.0f);

				// random color
				instancedData_[idx].color = MathHelper::RandColorRGBA();
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

	const HRESULT hr = pDevice->CreateBuffer(&vbd, &vbData, &pInstancedBuffer_);
	ASSERT_NOT_FAILED(hr, "can't create an instanced buffer");
}
