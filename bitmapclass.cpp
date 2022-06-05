////////////////////////////////////////////////////////////////////
// Filename:    bitmapclass.cpp
// Revising:    27.05.22
////////////////////////////////////////////////////////////////////
#include "bitmapclass.h"


// ---------------------------------------------------------------------------------- //
//                    CREATION OF THE VERTEX AND PIXEL SHADERS                        //
// ---------------------------------------------------------------------------------- //


BitmapClass::BitmapClass(void)
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTexture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass& obj)
{
}

BitmapClass::~BitmapClass(void)
{
}


/////////////////////////////////////////////////////////////////////
//
//                   PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////

// 
bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
	                         WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result = false;

	// in this function both the screen size and image size are stored. These will be
	// required for generating exact vertex locations during rendering.

	// store the screen size
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the size in pixels that this bitmap should be rendered at
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// the previous rendering location is first initialized to negative one. These variables
	// are responsible for location where 2D model was drawn
	m_previousPosX = -1;
	m_previousPosY = -1;


	// initialize the vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize buffers for a 2D model");
		return false;
	}

	// load the texture for this model
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't load the texture for a 2D model");
		return false;
	}
	
	return true;
}

// this function will release the vertex and index buffers as the texture that
// was used for the bitmap image
void BitmapClass::Shutdown(void)
{
	// Release the model texture
	ReleaseTexture();

	// Shutdown the vertex and index buffers
	ShutdownBuffers();

	return;
}


// puts the buffers of the 2D image on the video card
bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result = false;

	// Re-build the dynamic vertex buffer for rendering to possibly a different
	// location on the screen
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update buffers");
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare the for drawing
	RenderBuffers(deviceContext);

	return true;
}

// returns the number of indexes for the 2D image
int BitmapClass::GetIndexCount(void)
{
	return m_indexCount;
}

// return a pointer to the texture resource for this 2D image
ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_pTexture->GetTexture();
}

int BitmapClass::GetBitmapWidth(void)
{
	return m_bitmapWidth;
}

int BitmapClass::GetBitmapHeight(void)
{
	return m_bitmapHeight;
}





/////////////////////////////////////////////////////////////////////
//
//                   PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////

// this function is used to build the vertex and index buffer that will 
// be used to draw the 2D image
bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	VertexType* vertices = nullptr;
	ULONG* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	int i = 0;

	// set the number of vertices in the vertex array 
	m_vertexCount = 6;

	// set the number of indices in the index array
	m_indexCount = m_vertexCount;

	// create the vertex array
	vertices = new(std::nothrow) VertexType[m_vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}

	// create the index array
	indices = new(std::nothrow) ULONG[m_indexCount];
	if (!indices)
	{
		_DELETE(vertices);  // if we have an error about indices array we release the memory from vertices array
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the indexes count");
		return false;
	}

	// initialize vertex array to zeros at first
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// load the index array with data
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}


	// set up the description of the vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// now create the vertex buffer
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
		return false;
	}


	// Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.MiscFlags = 0;

	// give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create the index buffer
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the index buffer");
		return false;
	}


	// Release the arrays now that the vertex and index buffers have been created and loaded
	_DELETE(vertices);
	_DELETE(indices);

	return true;
}


// releases the vertex and index buffers
void BitmapClass::ShutdownBuffers(void)
{
	_RELEASE(m_pIndexBuffer);  // release the index buffer
	_RELEASE(m_pVertexBuffer); // release the vertex buffer

	return;
}


// this function is called each frame to update to contents of the dynamic vertex buffer
// to re-position the 2D bitmap image on the screen if need be
bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	HRESULT hr = S_OK;
	float left, right, top, bottom = 0.0f;
	//VertexType* vertices = nullptr;
	VertexType* verticesPtr = nullptr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	// if the position we are rendering this bitmap to has not changed then don't update
	// the vertex buffer since it currently has the correct parameters
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// if it has change then update the position it is being rendered to
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// calculate the screen cooridates of the left side of the bitmap
	left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);

	// calculate the screen coordiates of the right side of the bitmap
	right = left + static_cast<float>(m_bitmapWidth);

	// calculate the screen coordinates of the top of the bitmap
	top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);

	// calculate the screen coordinates of the bottom of the bitmap
	bottom = top - static_cast<float>(m_bitmapHeight);


	// lock the vertex buffer so it can be written to
	hr = deviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the vertex buffer");
		return false;
	}

	// get a pointer to the data in the vertex buffer
	verticesPtr = (VertexType*)mappedResource.pData;
	
	// write the data into the vertex buffer
	// first triangle
	verticesPtr[0].position = D3DXVECTOR3(left, top, 0.0f); // top left
	verticesPtr[0].texture  = D3DXVECTOR2(0.0f, 0.0f);

	verticesPtr[1].position = D3DXVECTOR3(right, bottom, 0.0f); // bottom right
	verticesPtr[1].texture  = D3DXVECTOR2(1.0f, 1.0f);

	verticesPtr[2].position = D3DXVECTOR3(left, bottom, 0.0f); // bottom left
	verticesPtr[2].texture  = D3DXVECTOR2(0.0f, 1.0f);

	// second triangle
	verticesPtr[3].position = D3DXVECTOR3(left, top, 0.0f); // top left
	verticesPtr[3].texture  = D3DXVECTOR2(0.0f, 0.0f);

	verticesPtr[4].position = D3DXVECTOR3(right, top, 0.0f); // top right
	verticesPtr[4].texture  = D3DXVECTOR2(1.0f, 0.0f);

	verticesPtr[5].position = D3DXVECTOR3(right, bottom, 0.0f); // bottom right
	verticesPtr[5].texture  = D3DXVECTOR2(1.0f, 1.0f);


	// unlock the vertex buffer
	deviceContext->Unmap(m_pVertexBuffer, 0);

	return true;
}


// sets up the vertex and index buffers on the GPU to be drawn by the shader
void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set the index buffer to active in the input assmbler so it can be rendered
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


// loads the texture that will be used for drawing the 2D image
bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result = false;

	// create the texture object
	m_pTexture = new(std::nothrow) TextureClass;
	if (!m_pTexture)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the texture object");
		return false;
	}

	// initialize the texture object
	result = m_pTexture->Initialize(device, filename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture object");
		return false;
	}

	return true;
}


// releases the texture that was loaded
void BitmapClass::ReleaseTexture(void)
{
	_SHUTDOWN(m_pTexture);

	return;
}