////////////////////////////////////////////////////////////////////
// Filename:     IndexBuffer.cpp
// Description:  an implementation of the IndexBuffer class;
// Revising:     14.12.22
////////////////////////////////////////////////////////////////////
#include "IndexBuffer.h"




IndexBuffer::IndexBuffer()
{
}

IndexBuffer::IndexBuffer(const IndexBuffer & obj)
{
	this->data_.indexCount_ = obj.data_.indexCount_;
	this->data_.pBuffer_ = obj.data_.pBuffer_;
}

IndexBuffer::~IndexBuffer()
{
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void IndexBuffer::Initialize(ID3D11Device* pDevice, 
	const std::vector<UINT> & indicesArr)
{
	// initialize this index buffer with indices data

	// check input params
	ASSERT_NOT_ZERO(indicesArr.size(), "the input indices array is empty");

	IndexBufferStorage::IndexBufferData initData;   // local data struct with params of the index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// initialize the number of indices
	initData.indexCount_ = (UINT)indicesArr.size();

	// setup the index buffer description
	indexBufferDesc.Usage          = D3D11_USAGE_IMMUTABLE; //D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth      = sizeof(UINT) * initData.indexCount_;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	// setup some properties of the index buffer
	data_ = initData;

	// create and initialize a buffer with data
	this->InitializeHelper(pDevice, indexBufferDesc, indicesArr);

	return;

}

///////////////////////////////////////////////////////////

void IndexBuffer::CopyBuffer(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext, 
	const IndexBuffer & inOriginBuffer)
{
	// this function copies data from the inOriginBuffer into the current one
	// and creates a new index buffer using this data;

	// copy the main data from the origin buffer
	IndexBufferStorage::IndexBufferData bufferData = inOriginBuffer.GetData();

	// check input params
	ASSERT_NOT_ZERO(bufferData.indexCount_, "there is no indices in the inOriginBuffer");

	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	D3D11_BUFFER_DESC dstBufferDesc;
	ID3D11Buffer* pStagingBuffer = nullptr;
	std::vector<UINT> indicesArr;             // will be filled with indices for a destination buffer

	try
	{
		/////////////////  CREATE A STAGING BUFFER AND COPY DATA INTO IT  /////////////////

		// setup the staging buffer description
		D3D11_BUFFER_DESC stagingBufferDesc;
		ZeroMemory(&stagingBufferDesc, sizeof(D3D11_BUFFER_DESC));

		stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
		stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingBufferDesc.ByteWidth = sizeof(UINT) * bufferData.indexCount_;

		// create a staging buffer for reading data from the anotherBuffer
		hr = pDevice->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
		ASSERT_NOT_FAILED(hr, "can't create a staging buffer");

		// copy the entire contents of the source resource to the destination 
		// resource using the GPU (from the anotherBuffer into the statingBuffer)
		pDeviceContext->CopyResource(pStagingBuffer, bufferData.pBuffer_);

		// map the staging buffer
		hr = pDeviceContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
		ASSERT_NOT_FAILED(hr, "can't map the staging buffer");

		// in the end we unmap the staging buffer and release it
		pDeviceContext->Unmap(pStagingBuffer, 0);
		_RELEASE(pStagingBuffer);


		/////////////////////  CREATE A DESTINATION INDEX BUFFER  //////////////////////

		// get the description of the anotherBuffer
		bufferData.pBuffer_->GetDesc(&dstBufferDesc);

		// allocate memory for indices of the destination buffer and fill it with data
		indicesArr.resize(bufferData.indexCount_);
		CopyMemory(indicesArr.data(), mappedSubresource.pData, sizeof(UINT) * bufferData.indexCount_);

		// create and initialize a buffer with data
		this->InitializeHelper(pDevice, dstBufferDesc, indicesArr);

		// update the data of this vertex buffer
		this->data_.indexCount_ = bufferData.indexCount_;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't allocate memory for indices of buffer");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't copy an index buffer");
	}

	return;

} // end CopyBuffer





////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC GETTERS
//
////////////////////////////////////////////////////////////////////////////////////////////

const IndexBufferStorage::IndexBufferData & IndexBuffer::GetData() const
{
	return data_;
}


ID3D11Buffer* IndexBuffer::Get() const
{
	// return a pointer the index buffer
	return data_.pBuffer_;
}

// return a double pointer to the index buffer
ID3D11Buffer* const* IndexBuffer::GetAddressOf() const
{
	return &(data_.pBuffer_);
}

UINT IndexBuffer::GetIndexCount() const
{
	// return a number of the indices
	return data_.indexCount_;
}




////////////////////////////////////////////////////////////////////////////////////////////
//                                 PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void IndexBuffer::InitializeHelper(ID3D11Device* pDevice,
	const D3D11_BUFFER_DESC & buffDesc,
	const std::vector<UINT> & indicesArr)
{
	// this function helps to initialize an INDEX buffer

	D3D11_SUBRESOURCE_DATA indexBufferData;

	// if we already have some data by the buffer pointer we need first of all to release it
	_RELEASE(data_.pBuffer_);

	// fill in initial indices data 
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = indicesArr.data();

	// create an index buffer
	const HRESULT hr = pDevice->CreateBuffer(&buffDesc, &indexBufferData, &data_.pBuffer_);
	ASSERT_NOT_FAILED(hr, "can't create an index buffer");

	return;

} // end InitializeHelper

///////////////////////////////////////////////////////////