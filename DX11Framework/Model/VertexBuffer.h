////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     VertexBuffer.h
// Description:  this class is needed for easier using of 
//               vertex buffers for models and GUI text;
// Revising:     12.12.22
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "VertexBufferInterface.h"

//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
class VertexBuffer : public VertexBufferInterface
{
private:
	// helps to initialize a buffer
	HRESULT InitializeHelper(const D3D11_BUFFER_DESC & buffDesc, 
		const VERTEX* pData,
		const UINT numVertices);  

	// buffers copying helper
	void CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst);
};


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////






template<class T>
VertexBuffer<T>::VertexBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;
}

///////////////////////////////////////////////////////////

template<class T>
VertexBuffer<T>::VertexBuffer(const VertexBuffer<T> & another)
{
	// check input params
	assert(another.pBuffer_ != nullptr);

	// check if we allocated memory for the current vertex buffer
	COM_ERROR_IF_FALSE(this, "this == nullptr");

	// copy the origin buffer into the current one
	*this = another;
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

	// check if we allocated memory for the current vertex buffer
	COM_ERROR_IF_FALSE(this, "this == nullptr");

	this->bufferSize_ = another.bufferSize_;
	this->stride_ = another.stride_;

	// another buffer -> current buffer
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
	D3D11_SUBRESOURCE_DATA bufferInitData;
	D3D11_BUFFER_DESC dstBufferDesc;

	// first of all create a destination buffer (or recreate it)
	// basis on the source buffer description
	
	pSrc->GetDesc(&dstBufferDesc);

	// if the buffer has already been initialized before
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// fill in initial vertices data
	// (at first the buffer will be filled in with default vertices)
	ZeroMemory(&bufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferInitData.pSysMem = new T[this->bufferSize_];
	bufferInitData.SysMemPitch = 0;
	bufferInitData.SysMemSlicePitch = 0;

	// create a vertex buffer
	hr = this->pDevice_->CreateBuffer(&dstBufferDesc, &bufferInitData, &pBuffer_);
	COM_ERROR_IF_FAILED(hr, "can't create a vertex buffer");

	pDeviceContext_->CopyResource(this->pBuffer_, pSrc);

	// first of all map the src buffer to get its data
	//D3D11_MAPPED_SUBRESOURCE mappedResourceOfSrcBuffer;
	//hr = pDeviceContext_->Map(pSrc, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceOfSrcBuffer);
	//COM_ERROR_IF_FAILED(hr, "failed to map the source vertex buffer");

	// create and initialize the desctination buffer with the source buffer's data
	/*
	
	hr = this->InitializeHelper(this->pDevice_,
	dstBufferDesc,
	static_cast<T*>(mappedResourceOfSrcBuffer.pData),
	this->bufferSize_);
	COM_ERROR_IF_FAILED(hr, "can't create a copy of vertex buffer");
	
	*/

	// upmap the source vertex buffer
	//pDeviceContext_->Unmap(pSrc, 0);
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
bool VertexBuffer<T>::UpdateDynamic(ID3D11DeviceContext* pDeviceContext, const T* data)
{                            
	// update the DYNAMIC vertex buffer with new vertices data

	// check input params
	assert(data != nullptr);
	assert(this->GetUsage() == D3D11_USAGE::D3D11_USAGE_DYNAMIC);

	// map the buffer
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

template<class T>
D3D11_USAGE VertexBuffer<T>::GetUsage() const
{
	D3D11_BUFFER_DESC bufferDesc;
	this->pBuffer_->GetDesc(&bufferDesc);

	return bufferDesc.Usage;
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