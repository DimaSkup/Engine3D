/////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
// Last revising: 29.03.22
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass(void)
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_texture = nullptr;
}

ModelClass::ModelClass(const ModelClass& another)
{
}

ModelClass::~ModelClass(void)
{
}

// ------------------------------------------------------------------------------ //
//
//                           PUBLIC METHODS
//
// ------------------------------------------------------------------------------ //

// Initialization of the model
bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	// Initialize the vertex and index buffer that hold the geometry for the model
	if (!InitializeBuffers(device))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the buffers");
		return false;
	}

	// Load the texture for this model
	if (!LoadTexture(device, textureFilename))
	{
		Log::Get()->Error(THIS_FUNC, "can't load texture for the model");
		return false;
	}

	return true;
}


// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	ReleaseTexture();   // Release the model texture
	ShutdownBuffers();  // release the memory from the vertex and index buffer

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

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

// ------------------------------------------------------------------------------ //
//
//                           PRIVATE METHODS
//
// ------------------------------------------------------------------------------ //

// Initialization of the vertex and index buffers
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	VERTEX* vertices = nullptr;
	unsigned long* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// setup the number of vertices and indices
	m_vertexCount = 4;
	m_indexCount = 6;

	// allocate the memory for the vertices and indices
	vertices = new(std::nothrow) VERTEX[m_vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}

	indices = new(std::nothrow) unsigned long[m_indexCount];
	if (!indices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the indices array");
		return false;
	}

	// create vertices and indices
	vertices[0].position = D3DXVECTOR3(-1.0, -1.0f, 0.0f);   // bottom left
	vertices[0].texture  = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);   // top left
	vertices[1].texture  = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);    // top right
	vertices[2].texture  = D3DXVECTOR2(1.0f, 0.0f);
	vertices[2].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);   // bottom right
	vertices[3].texture  = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].normal   = D3DXVECTOR3(0.0f, 0.0f, -1.0f);


	// load the index array with data
	indices[0] = 0;  // bottom left
	indices[1] = 1;  // top left
	indices[2] = 2;  // top right

	indices[3] = 0;  // bottom left
	indices[4] = 2;  // top right
	indices[5] = 3;  // bottom right

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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
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
	_RELEASE(m_pIndexBuffer);
	_RELEASE(m_pVertexBuffer);

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

// Creates the texture object and then initialize it with the input file name provided.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	// Create the texture object
	m_texture = new(std::nothrow) TextureClass;
	if (!m_texture)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the texture object");
		return false;
	}

	// Initialize the texture object
	if (!m_texture->Initialize(device, filename))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture object");
		return false;
	}

	return true;
}

// Releases the texture object that was created and loaded during the LoadTexture function
void ModelClass::ReleaseTexture(void)
{
	// Release the texture object
	_SHUTDOWN(m_texture);

	return;
}