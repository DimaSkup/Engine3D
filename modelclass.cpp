/////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass(void)
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass(void)
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
	// Initialize the vertex and index buffer that hold the geometry for the triangle
	if (!InitializeBuffers(device));
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the vertex and index buffer");
		return false;
	}

	return true;
}

void ModelClass::Shutdown(void)
{
	// Release the vertex and index buffers
	ShutdownBuffers();
	
	return;
}

// This function calls RenderBuffers to put the vertex and index buffers on the 
// graphics pipeline so the color shader will be able to render them
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);

	return;
}

// Returns the number of indices in the model
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

// Handles creating of the vertex and index buffers.
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	Log::Get()->Debug(THIS_FUNC, "");	// print that we are in this function

	VERTEX* vertices = nullptr;
	unsigned long* indices = 0;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr = S_OK;

	// Set the number of vertices in the vertex array
	m_vertexCount = 3;

	// Set the number of indices in the index array
	m_indexCount = 3;

	// Create the vertex array
	vertices = new(std::nothrow) VERTEX[m_vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertex array");
		return false;
	}

	// Create the index array
	indices = new(std::nothrow) unsigned long[m_indexCount];
	if (!indices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the index array");
		return false;
	}


	// Load the vertex array with data
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);		// Bottom left
	vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);	// green

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// Top middle
	vertices[1].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);	// red

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);		// Bottom right
	vertices[2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);	// blue

	// Load the index array with data
	indices[0] = 0;	// bottom left
	indices[1] = 1;	// top left
	indices[2] = 2;	// bottom right

	

	// ---------------------------------------------------------------------------- //
	//				CREATE THE VERTEX AND INDEX BUFFERS                             //
	// ---------------------------------------------------------------------------- //

	// Set up the description of the static vertex buffer
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * m_vertexCount;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vetex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
		return false;
	}

	// Set up the description of the static index buffer
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the index buffer");
		return false;
	}

	// Release the arrays because the vertex and index buffers have been created and filled
	_DELETE(vertices);
	_DELETE(indices);

	return true;
}

// this function just releases the vertex and index buffers
void ModelClass::ShutdownBuffers(void)
{
	_RELEASE(m_pIndexBuffer);
	_RELEASE(m_pVertexBuffer);

	return;
}

// this function sets the vertex and index buffers as active on the input assembler
// in the GPU and tells DirectX that we want to draw buffers as triangles
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive 
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}