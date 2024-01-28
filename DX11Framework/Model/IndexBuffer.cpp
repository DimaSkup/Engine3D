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
	this->indexCount_ = another.indexCount_;

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

HRESULT IndexBuffer::Initialize(const std::vector<UINT> & indicesArr)
{
	// create and initialize an index buffer with indices data

	// check input params
	COM_ERROR_IF_FALSE(indicesArr.empty() == false, "the input indices array is empty");

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC indexBufferDesc;


	// if we already have some data by the buffer pointer we need first of all to release it
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// initialize the number of indices
	this->indexCount_ = (UINT)indicesArr.size();

	// set up the index buffer description
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth      = sizeof(UINT) * this->indexCount_;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags      = 0;

	// create and initialize a buffer with data
	this->InitializeHelper(indexBufferDesc, indicesArr);

	return hr;
} // end Initialize

///////////////////////////////////////////////////////////

bool IndexBuffer::CopyBuffer(const IndexBuffer & anotherBuffer)
{
	// this function copies data from the anotherBuffer into the current one;

	// check input params
	COM_ERROR_IF_FALSE(anotherBuffer.GetIndexCount(), "there is no indices in the anotherBuffer");

	HRESULT hr = S_OK;
	std::vector<UINT> indicesArr;             // indices for a destination buffer
	D3D11_SUBRESOURCE_DATA initBufferData;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	D3D11_BUFFER_DESC dstBufferDesc;
	ID3D11Buffer* pStagingBuffer = nullptr;

	try
	{
		// setup the number of indices
		this->indexCount_ = anotherBuffer.GetIndexCount();


		/////////////////  CREATE A STAGING BUFFER AND COPY DATA INTO IT  /////////////////

		// setup the staging buffer description
		D3D11_BUFFER_DESC stagingBufferDesc;
		ZeroMemory(&stagingBufferDesc, sizeof(D3D11_BUFFER_DESC));

		stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
		stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingBufferDesc.ByteWidth = sizeof(UINT) * this->indexCount_;

		// create a staging buffer for reading data from the anotherBuffer
		hr = pDevice_->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
		COM_ERROR_IF_FAILED(hr, "can't create a staging buffer");

		// copy the entire contents of the source resource to the destination 
		// resource using the GPU (from the anotherBuffer into the statingBuffer)
		pDeviceContext_->CopyResource(pStagingBuffer, anotherBuffer.Get());

		// map the staging buffer
		hr = pDeviceContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
		COM_ERROR_IF_FAILED(hr, "can't map the staging buffer");



		/////////////////////  CREATE A DESTINATION INDEX BUFFER  //////////////////////

		// if the buffer has already been initialized before
		if (this->pBuffer_ != nullptr)
			_RELEASE(this->pBuffer_);

		// get the description of the anotherBuffer
		anotherBuffer.Get()->GetDesc(&dstBufferDesc);

		// allocate memory for indices of the destination buffer and fill it with data
		indicesArr.resize(this->indexCount_);
		CopyMemory(indicesArr.data(), mappedSubresource.pData, sizeof(UINT) * this->indexCount_);

		// fill in initial indices data
		ZeroMemory(&initBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		initBufferData.pSysMem = indicesArr.data();

		// create an index buffer
		hr = this->pDevice_->CreateBuffer(&dstBufferDesc, &initBufferData, &pBuffer_);
		COM_ERROR_IF_FAILED(hr, "can't create an index buffer");

		// in the end we unmap the staging buffer and release it
		pDeviceContext_->Unmap(pStagingBuffer, 0);
		_RELEASE(pStagingBuffer);

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for indices of buffer");
	}
	catch (COMException & e)
	{
		// in case of any error we unmap the staging buffer and release it
		pDeviceContext_->Unmap(pStagingBuffer, 0);
		_RELEASE(pStagingBuffer);

		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't copy an index buffer buffer");
		return false;
	}

	return true;

} // end CopyBuffer





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

UINT IndexBuffer::GetIndexCount() const
{
	// return a number of the indices
	return this->indexCount_;
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                 PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IndexBuffer::InitializeHelper(const D3D11_BUFFER_DESC & buffDesc,
	const std::vector<UINT> & indicesArr)
{
	// this function helps to initialize an INDEX buffer

	D3D11_SUBRESOURCE_DATA indexBufferData;

	// if the index buffer has already been initialized before
	if (pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// fill in initial indices data 
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = indicesArr.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	// create an index buffer
	return this->pDevice_->CreateBuffer(&buffDesc, &indexBufferData, &pBuffer_);

} // end InitializeHelper

///////////////////////////////////////////////////////////