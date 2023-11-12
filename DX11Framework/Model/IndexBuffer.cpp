////////////////////////////////////////////////////////////////////
// Filename:     IndexBuffer.cpp
// Description:  an implementation of the IndexBuffer class;
// Revising:     14.12.22
////////////////////////////////////////////////////////////////////
#include "IndexBuffer.h"




IndexBuffer::IndexBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;
}

///////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer(const IndexBuffer & another)
{
	// check input params
	assert(another.pBuffer_ != nullptr);

	// check if we allocated memory for the current index buffer
	COM_ERROR_IF_FALSE(this, "this == nullptr");

	// copy the origin buffer into the current one
	*this = another;
}

///////////////////////////////////////////////////////////

IndexBuffer & IndexBuffer::operator=(const IndexBuffer & another)
{
	// guard self assignment
	if (this == &another)
		return *this;

	// check input params 
	assert(another.pBuffer_ != nullptr);

	// check if we allocated memory for the current vertex buffer
	COM_ERROR_IF_FALSE(this, "this == nullptr");

	this->pDevice_ = another.pDevice_;
	this->pDeviceContext_ = another.pDeviceContext_;
	this->bufferSize_ = another.bufferSize_;

	// another buffer -> current buffer
	//CopyBufferFromTo(another.pBuffer_, this->pBuffer_);

	return *this;
}

///////////////////////////////////////////////////////////

IndexBuffer::~IndexBuffer()
{
	_RELEASE(pBuffer_);
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IndexBuffer::Initialize(const UINT* data, const UINT numIndices)
{
	// create and initialize an index buffer with indices data

	// check input params
	COM_ERROR_IF_FALSE(data, "input indices data == nullptr");
	COM_ERROR_IF_FALSE(numIndices, "the number of indices == 0");


	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC indexBufferDesc;

	// if we already have some data by the buffer pointer we need first of all to release it
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// initialize the number of indices
	this->bufferSize_ = numIndices;

	// set up the index buffer description
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(UINT) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;

	// create and initialize a buffer with data
	this->InitializeHelper(indexBufferDesc, data);

	return hr;
} // Initialize()





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

HRESULT IndexBuffer::InitializeHelper(const D3D11_BUFFER_DESC & buffDesc,
	const UINT* data)
{
	// this function helps to initialize an INDEX buffer

	// check input params
	COM_ERROR_IF_FALSE(data, "input data == nullptr");

	D3D11_SUBRESOURCE_DATA indexBufferData;

	// if the index buffer has already been initialized before
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// fill in initial indices data 
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = data;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	// create an index buffer
	return this->pDevice_->CreateBuffer(&buffDesc, &indexBufferData, &pBuffer_);
}

///////////////////////////////////////////////////////////

void IndexBuffer::CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst)
{
	// this function reinitializes a destination buffer basis on the source buffer

	D3D11_SUBRESOURCE_DATA indexBufferData;
	D3D11_BUFFER_DESC dstBufferDesc;

	// first of all create a destination buffer (or recreate it)
	// basis on the source buffer description
	pSrc->GetDesc(&dstBufferDesc);
	
	// if the index buffer has already been initialized before
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// fill in initial indices data (at first the buffer will be filled in with zeros)
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = new UINT[this->bufferSize_]{ 0 };
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	// create an index buffer
	HRESULT hr = this->pDevice_->CreateBuffer(&dstBufferDesc, &indexBufferData, &pBuffer_);

	// create and initialize the desctination buffer with data
	//hr = this->InitializeHelper(dstBufferDesc, pBufferData);
	COM_ERROR_IF_FAILED(hr, "can't create an index buffer");

	pDeviceContext_->CopyResource(this->pBuffer_, pSrc);

	// map the src buffer to get its data
	//D3D11_MAPPED_SUBRESOURCE subresource;
	//hr = pDeviceContext_->Map(pSrc, 0, D3D11_MAP::D3D11_MAP_READ, 0, &subresource);
	//COM_ERROR_IF_FAILED(hr, "failed to map the source index buffer");
/*

	HRESULT hr = S_OK;
	UINT* pBufferData = nullptr;


	// first of all map the src buffer to get its data
	D3D11_MAPPED_SUBRESOURCE subresource;
	hr = pDeviceContext_->Map(pSrc, 0, D3D11_MAP::D3D11_MAP_READ, 0, &subresource);
	COM_ERROR_IF_FAILED(hr, "failed to map the source index buffer");

	if (subresource.pData)
	{
		try
		{
			// if the index buffer has already been initialized before
			if (pBuffer_ != nullptr)
				_RELEASE(pBuffer_);

			// allocate memory for the buffer's data
			UINT* pBufferData = new UINT[this->bufferSize_]{ 0 };

			UINT bufferByteSize = sizeof(UINT) * bufferSize_;

			// copy data into the index buffer
			memcpy_s(pBufferData,
				bufferByteSize,
				subresource.pData,
				bufferByteSize);
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			Log::Error(THIS_FUNC, "can't allocate memory for the index buffer's data");
			return;
		}
	}




	// upmap the source index buffer
	pDeviceContext_->Unmap(pSrc, 0);

*/

	return;

} // end CopyBufferFromTo
