////////////////////////////////////////////////////////////////////
// Filename:     VertexBuffer.h
// Description:  this class is needed for easier using of 
//               vertex buffers for models and GUI text;
// Revising:     12.12.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>

#include "../Engine/macros.h"
#include "../Engine/COMException.h"

//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
template<class T>
class VertexBuffer
{
public:
	VertexBuffer(ID3D11DeviceContext* pDeviceContext);
	VertexBuffer(const VertexBuffer<T> & another);
	~VertexBuffer();

	VertexBuffer<T> & operator=(const VertexBuffer<T> & another);

	// INITIALIZE / UPDATE
	HRESULT Initialize(ID3D11Device* pDevice, const T* data, const UINT numVertices);   // initialize a DEFAULT vertex buffer with vertices data
//	HRESULT InitializeDynamic(ID3D11Device* pDevice, const T* data, const UINT numVertices);   // initialize a DYNAMIC vertex buffer with vertices data
	bool UpdateDynamic(ID3D11DeviceContext* pDeviceContext, T* data);                          // update a DYNAMIC vertex buffer
	
	// GETTERS
	ID3D11Buffer* Get() const;                 // get a pointer to the vertex buffer
	ID3D11Buffer* const* GetAddressOf() const; // get a double pointer to the vertex buffer
	const UINT GetBufferSize() const;          // get a number of vertices
	const UINT GetStride() const;              // get the stride size
	const UINT* GetAddressOfStride() const;    // get a pointer to the stride variable

private:
	// helps to initialize a vertex buffer of ANY type (usage)
	HRESULT InitializeHelper(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& buffDesc, const T* data, const UINT numVertices);  

	// buffers copying helper
	void CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst);

private:
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
	UINT stride_ = 0;                          // a stride size
	UINT bufferSize_ = 0;                      // a number of vertices
};


///////////////////////////////////////////////////////////


template<class T>
VertexBuffer<T>::VertexBuffer(ID3D11DeviceContext* pDeviceContext)
{
	this->pDeviceContext_ = pDeviceContext;
}

///////////////////////////////////////////////////////////

template<class T>
VertexBuffer<T>::VertexBuffer(const VertexBuffer<T> & another)
{
	// check input params
	assert(another.pBuffer_ != nullptr);

	stride_ = another.stride_;
	bufferSize_ = another.bufferSize_;
	CopyBufferFromTo(another.pBuffer_, this->pBuffer_);
}

///////////////////////////////////////////////////////////

// copying operation
template<class T>
VertexBuffer<T> & VertexBuffer<T>::operator=(const VertexBuffer<T> & another)
{
	// guard self assignment
	if (this == &another)
		return *this;

	// check input params 
	assert(another.pBuffer_ != nullptr);

	this->bufferSize_ = another.bufferSize_;
	this->stride_ = another.stride_;
	CopyBufferFromTo(another.pBuffer_, this->pBuffer_);

	return *this;
}
///////////////////////////////////////////////////////////

// a destructor
template<class T>
VertexBuffer<T>::~VertexBuffer()
{
	_RELEASE(pBuffer_);
}

///////////////////////////////////////////////////////////

template<class T>
void VertexBuffer<T>::CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst)
{
	HRESULT hr = S_OK;

	// map the src buffer
	D3D11_MAPPED_SUBRESOURCE mappedResourceOfSrcBuffer;
	hr = pDeviceContext_->Map(pSrc, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceOfSrcBuffer);
	COM_ERROR_IF_FAILED(hr, "failed to map the source vertex buffer");


	// map the dst buffer
	D3D11_MAPPED_SUBRESOURCE mappedResourceOfDstBuffer;
	hr = pDeviceContext_->Map(pDst, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceOfDstBuffer);
	COM_ERROR_IF_FAILED(hr, "failed to map the destination vertex buffer");

	// copy a new data into the vertex buffer
	CopyMemory(mappedResourceOfDstBuffer.pData, mappedResourceOfSrcBuffer.pData, sizeof(T) * bufferSize_);

	// upmap both input vertex buffers
	pDeviceContext_->Unmap(pSrc, 0);
	pDeviceContext_->Unmap(pDst, 0);
}

///////////////////////////////////////////////////////////

template<class T>
HRESULT VertexBuffer<T>::Initialize(ID3D11Device* pDevice,
	const T* data,
	const UINT numVertices)
{
	// initialize a vertex buffer with vertices data

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC vertexBufferDesc;

	// setup the vertex buffer description
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = InitializeHelper(pDevice, vertexBufferDesc, data, numVertices);
	COM_ERROR_IF_FAILED(hr, "can't create a vertex buffer");


	return hr;
} // Initialize()

///////////////////////////////////////////////////////////

template<class T>
HRESULT VertexBuffer<T>::InitializeHelper(ID3D11Device* pDevice,
	const D3D11_BUFFER_DESC & buffDesc,
	const T* data,
	const UINT numVertices)
{
	// this function helps to initialize a vertex buffer of ANY type (usage)

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	// if the vertex buffer has already been initialized before
	if (pBuffer_ != nullptr)  
		_RELEASE(pBuffer_);

	// define the number of vertices and stride size
	this->bufferSize_ = numVertices;
	
	stride_ = sizeof(T);
	assert(stride_ > 0);


	// fill in initial vertices data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = data;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// try to create a vertex buffer
	return pDevice->CreateBuffer(&buffDesc, &vertexBufferData, &pBuffer_);
}

///////////////////////////////////////////////////////////
 
template<class T>
bool VertexBuffer<T>::UpdateDynamic(ID3D11DeviceContext* pDeviceContext, T* data)
{                            
	// update the DYNAMIC vertex buffer with new vertices data

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	COM_ERROR_IF_FAILED(hr, "failed to map the vertex buffer");

	CopyMemory(mappedResource.pData, data, sizeof(T) * bufferSize_);
	pDeviceContext->Unmap(pBuffer_, 0);

	return true;

} // end UpdateDynamic










///////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC GETTERS
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
ID3D11Buffer* VertexBuffer<T>::Get() const
{
	// get a pointer to the vertex buffer
	return pBuffer_;
}

///////////////////////////////////////////////////////////

template<class T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const
{
	// get a double pointer to the vertex buffer
	return &pBuffer_;
}

///////////////////////////////////////////////////////////

template<class T>
const UINT VertexBuffer<T>::GetBufferSize() const
{
	// get a number of vertices
	return this->bufferSize_;
}

///////////////////////////////////////////////////////////

template<class T>
const UINT VertexBuffer<T>::GetStride() const
{
	// get the stride size
	return this->stride_;
}

///////////////////////////////////////////////////////////

template<class T>
const UINT* VertexBuffer<T>::GetAddressOfStride() const
{
	// get a pointer to the stride variable
	return &stride_;
}