/////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
// Last revising: 29.03.22
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass(void)
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

ModelClass::ModelClass(const ModelClass& another)
{
}

ModelClass::~ModelClass(void)
{
}


// Initialization of the model
bool ModelClass::Initialize(ID3D11Device* device)
{
	if (!InitializeBuffers(device))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the buffers");
		return false;
	}

	return true;
}


// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	ShutdownBuffers();	// release the memory from the buffers

	return;
}

// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

// Get the number of indices
int ModelClass::GetIndexCount(void)
{
	return m_indexCount;
}

// Initialization of the vertex and index buffers
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	VERTEX* vertices = nullptr;
	ULONG* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// setup the number of vertices and indices
	m_vertexCount = 3;
	m_indexCount = 3;

	// allocate the memory for the vertices and indices
	vertices = new(std::nothrow) VERTEX[m_vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}

	indices = new(std::nothrow) ULONG[m_indexCount];
	if (!indices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the indices array");
		return false;
	}

	// create vertices and indices
	vertices[0].position = { -1.0, -1.0f, 0.0f };   // bottom left
	vertices[0].color = { 1.0f, 0.0f, 0.0f, 1.0f }; // red

	vertices[1].position = { 0.0f, 1.0f, 0.0f };    // middle top
	vertices[1].color = { 0.0f, 1.0f, 0.0f, 1.0f }; // green

	vertices[2].position = { 1.0f, -1.0f, 0.0f };	// bottom right
	vertices[2].color = { 0.0f, 0.0f, 1.0f, 1.0f }; // blue


	indices[0] = 0;	// bottom left
	indices[1] = 1; // middle top
	indices[2] = 2; // bottom right

	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //

					// Setup the vertex buffer description
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Fill in initial vertices data 
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create a vertex buffer using the vertex buffer description
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
		return false;
	}

	// Setup the index buffer description
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Fill in initial indices data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create an index buffer using the index buffer description
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the index buffer");
		return false;
	}


	// release the vertex and index arrays because we already have buffers are initialized
	_DELETE(vertices);
	_DELETE(indices);


	Log::Get()->Debug(THIS_FUNC, "model is initialized successfully");

	return true;
}

// Releasing of the allocated memory from the vertex and index buffers
void ModelClass::ShutdownBuffers(void)
{
	_SHUTDOWN(m_pIndexBuffer);
	_SHUTDOWN(m_pVertexBuffer);

	return;
}


// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	// set the vertex buffer as active
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set the index buffer as active
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// set which type of primitive topology we want to use
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}