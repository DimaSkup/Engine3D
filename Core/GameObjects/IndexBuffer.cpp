////////////////////////////////////////////////////////////////////
// Filename:     IndexBuffer.cpp
// Description:  an implementation of the IndexBuffer class;
// Revising:     14.12.22
////////////////////////////////////////////////////////////////////
#include "IndexBuffer.h"
#include "../Common/MemHelpers.h"
#include "../Common/Assert.h"


IndexBuffer::IndexBuffer()
{
}

IndexBuffer::IndexBuffer(ID3D11Device* pDevice, const std::vector<UINT>& indices)
{
	Initialize(pDevice, indices);
}

IndexBuffer::IndexBuffer(IndexBuffer&& rhs)
{
	pBuffer_ = rhs.pBuffer_;
	indexCount_ = rhs.indexCount_;

	rhs.pBuffer_ = nullptr;
	rhs.indexCount_ = 0;
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(&pBuffer_);
	indexCount_ = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void IndexBuffer::Initialize(
	ID3D11Device* pDevice, 
	const std::vector<UINT> & indicesArr)
{
	// initialize this index buffer with indices data

	Assert::NotZero(indicesArr.size(), "the input indices array is empty");

	D3D11_BUFFER_DESC indexBufferDesc;

	// initialize the number of indices
	indexCount_ = (UINT)indicesArr.size();

	// setup the index buffer description
	indexBufferDesc.ByteWidth           = sizeof(UINT) * indexCount_;
	indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;   // D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	// create and initialize a buffer with data
	InitializeHelper(pDevice, indexBufferDesc, indicesArr);

	return;

}

///////////////////////////////////////////////////////////

void IndexBuffer::CopyBuffer(
	ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext, 
	const IndexBuffer & srcBuffer)
{
	// this function copies data from the inOriginBuffer into the current one
	// and creates a new index buffer using this data;

	// copy the main data from the origin buffer
	ID3D11Buffer* pOrigBuffer = srcBuffer.Get();
	const UINT origIndexCount = srcBuffer.GetIndexCount();
	
	// check input params
	Assert::NotZero(origIndexCount, "there is no indices in the inOriginBuffer");

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
		stagingBufferDesc.ByteWidth = sizeof(UINT) * origIndexCount;

		// create a staging buffer for reading data from the anotherBuffer
		hr = pDevice->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
		Assert::NotFailed(hr, "can't create a staging buffer");

		// copy the entire contents of the source resource to the destination 
		// resource using the GPU (from the anotherBuffer into the statingBuffer)
		pDeviceContext->CopyResource(pStagingBuffer, pOrigBuffer);

		// map the staging buffer
		hr = pDeviceContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
		Assert::NotFailed(hr, "can't map the staging buffer");

		// in the end we unmap the staging buffer and release it
		pDeviceContext->Unmap(pStagingBuffer, 0);
		SafeRelease(&pStagingBuffer);


		/////////////////////  CREATE A DESTINATION INDEX BUFFER  //////////////////////

		// get the description of the anotherBuffer
		pOrigBuffer->GetDesc(&dstBufferDesc);

		// allocate memory for indices of the destination buffer and fill it with data
		indicesArr.resize(origIndexCount);
		CopyMemory(indicesArr.data(), mappedSubresource.pData, sizeof(UINT) * origIndexCount);

		// create and initialize a buffer with data
		InitializeHelper(pDevice, dstBufferDesc, indicesArr);

		// update the data of this vertex buffer
		indexCount_ = origIndexCount;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for indices of buffer");
	}
	catch (EngineException & e)
	{
		Log::Error(e);
		throw EngineException("can't copy an index buffer");
	}

	return;

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
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	// if we already have some data by the buffer pointer we need first of all to release it
	SafeRelease(&pBuffer_);

	// fill in initial indices data 
	indexBufferData.pSysMem = indicesArr.data();

	// create an index buffer
	const HRESULT hr = pDevice->CreateBuffer(&buffDesc, &indexBufferData, &pBuffer_);
	Assert::NotFailed(hr, "can't create an index buffer");
}
