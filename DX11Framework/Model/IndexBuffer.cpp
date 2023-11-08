////////////////////////////////////////////////////////////////////
// Filename:     IndexBuffer.cpp
// Description:  an implementation of the IndexBuffer class;
// Revising:     14.12.22
////////////////////////////////////////////////////////////////////
#include "IndexBuffer.h"


IndexBuffer::IndexBuffer(ID3D11DeviceContext* pDeviceContext)
{
	this->pDeviceContext_ = pDeviceContext;
}

IndexBuffer::IndexBuffer(const IndexBuffer & another)
{
	// check input params
	assert(another.pBuffer_ != nullptr);

	bufferSize_ = another.bufferSize_;
	CopyBufferFromTo(another.pBuffer_, this->pBuffer_);
}

IndexBuffer::~IndexBuffer()
{
	_RELEASE(pBuffer_);
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

// initialize the index buffer with indices data
HRESULT IndexBuffer::Initialize(ID3D11Device* pDevice, 
	const UINT* data, 
	const UINT numIndices)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexBufferData;

	// if we already have some data by the buffer pointer we need first of all to release it
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	this->bufferSize_ = numIndices;

	// set up the index buffer description
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(UINT) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;

	// fill in the initial indices data
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = data;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &pBuffer_);
	COM_ERROR_IF_FAILED(hr, "can't create an index buffer");

	return hr;
} // Initialize()


IndexBuffer & IndexBuffer::operator=(const IndexBuffer & another)
{
	// guard self assignment
	if (this == &another)
		return *this;

	// check input params 
	assert(another.pBuffer_ != nullptr);

	this->bufferSize_ = another.bufferSize_;
	CopyBufferFromTo(another.pBuffer_, this->pBuffer_);

	return *this;
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////////////////

// return a pointer the index buffer
ID3D11Buffer* IndexBuffer::Get() const
{
	return pBuffer_;
}

// return a double pointer to the index buffer
ID3D11Buffer* const* IndexBuffer::GetAddressOf() const
{
	return &pBuffer_;
}

// return a number of the indices
UINT IndexBuffer::GetBufferSize() const
{
	return this->bufferSize_;
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                 PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////



void IndexBuffer::CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst)
{
	HRESULT hr = S_OK;

	// map the src buffer
	D3D11_MAPPED_SUBRESOURCE mappedResourceOfSrcBuffer;
	hr = pDeviceContext_->Map(pSrc, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceOfSrcBuffer);
	COM_ERROR_IF_FAILED(hr, "failed to map the source index buffer");

	// map the dst buffer
	D3D11_MAPPED_SUBRESOURCE mappedResourceOfDstBuffer;
	hr = pDeviceContext_->Map(pDst, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceOfDstBuffer);
	COM_ERROR_IF_FAILED(hr, "failed to map the destination index buffer");

	// copy a new data into the index buffer
	CopyMemory(mappedResourceOfDstBuffer.pData, mappedResourceOfSrcBuffer.pData, sizeof(UINT) * bufferSize_);

	// upmap both vertex buffers
	pDeviceContext_->Unmap(pSrc, 0);
	pDeviceContext_->Unmap(pDst, 0);

	return;

} // end CopyBufferFromTo
