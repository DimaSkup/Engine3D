////////////////////////////////////////////////////////////////////
// Filename:     VertexBuffer.h
// Description:  this class is needed for easier using of 
//               vertex buffers for models;
// Revising:     12.12.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>


//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
template<class T>
class VertexBuffer
{
public:
	VertexBuffer() {}
	~VertexBuffer();

	// initialize the vertex buffer with vertices data
	HRESULT Initialize(ID3D11Device* pDevice, T* data, UINT numVertices);
	

	ID3D11Buffer* Get() const;                 // get a pointer to the vertex buffer
	ID3D11Buffer* const* GetAddressOf() const; // get a double pointer to the vertex buffer
	UINT GetBufferSize() const;                // get a number of vertices
	const UINT GetStride() const;              // get the stride size
	const UINT* GetAddressOfStride() const;    // get a pointer to the stride variable

private:
	VertexBuffer(const VertexBuffer<T>& rhs);

private:
	ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
	std::unique_ptr<UINT> pStride_ = nullptr;  // a stride size
	UINT bufferSize_ = 0;                      // a number of vertices
};

// a destructor
template<class T>
VertexBuffer<T>::~VertexBuffer()
{
	_RELEASE(pBuffer_);
	Log::Debug(THIS_FUNC_EMPTY);
}

// initialize the vertex buffer with vertices data
template<class T>
HRESULT VertexBuffer<T>::Initialize(ID3D11Device* pDevice, T* data, UINT numVertices)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexBufferData;

	// define the number of vertices and stride size
	this->bufferSize_ = numVertices;
	this->pStride_ = std::make_unique<UINT>(static_cast<UINT>(sizeof(T)));

	// setup the vertex buffer description
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// fill in initial vertices data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = data;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pBuffer_);
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't create a vertex buffer");
	}

	return hr;
} // Initialize()


  // get a pointer to the vertex buffer
template<class T>
ID3D11Buffer* VertexBuffer<T>::Get() const
{
	return pBuffer_;
}


// get a double pointer to the vertex buffer
template<class T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const
{
	return &pBuffer_;
}


// get a number of vertices
template<class T>
UINT VertexBuffer<T>::GetBufferSize() const
{
	return this->bufferSize_;
}


// get the stride size
template<class T>
const UINT VertexBuffer<T>::GetStride() const
{
	return *this->pStride_;
}


// get a pointer to the stride variable
template<class T>
const UINT* VertexBuffer<T>::GetAddressOfStride() const
{
	return this->pStride_.get();
}