////////////////////////////////////////////////////////////////////
// Filename:     IndexBuffer.cpp
// Description:  an implementation of the IndexBuffer class;
// Revising:     14.12.22
////////////////////////////////////////////////////////////////////
#include "IndexBuffer.h"


// initialize the index buffer with indices data
HRESULT IndexBuffer::Initialize(ID3D11Device* pDevice, UINT* data, UINT numIndices)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexBufferData;

	this->bufferSize_ = numIndices;

	// set up the index buffer description
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// fill in the initial indices data
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = data;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &pBuffer_);
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't create an index buffer");
	}

	return hr;
} // Initialize()

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
