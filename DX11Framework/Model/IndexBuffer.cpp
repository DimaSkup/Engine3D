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
	this->bufferSize_ = (UINT)indicesArr.size();

	// set up the index buffer description
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth      = sizeof(UINT) * this->bufferSize_;
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

	HRESULT hr = S_OK;
	D3D11_SUBRESOURCE_DATA initBufferData;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	D3D11_BUFFER_DESC dstBufferDesc;

	// setup the number of indices
	this->bufferSize_ = anotherBuffer.GetBufferSize();


	// ----------------- CREATE A STAGING BUFFER AND COPY DATA INTO IT ----------------- //

	// setup the staging buffer description
	D3D11_BUFFER_DESC stagingBufferDesc;
	ZeroMemory(&stagingBufferDesc, sizeof(D3D11_BUFFER_DESC));

	stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
	stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingBufferDesc.ByteWidth = sizeof(UINT) * this->bufferSize_;
	stagingBufferDesc.BindFlags = 0;
	stagingBufferDesc.MiscFlags = 0;
	stagingBufferDesc.StructureByteStride = 0;

	// allocate memory for the staging buffer
	initBufferData.pSysMem = new UINT[this->bufferSize_];
	initBufferData.SysMemPitch = 0;
	initBufferData.SysMemSlicePitch = 0;

	// create a staging buffer for reading data from the anotherBuffer
	ID3D11Buffer* pStagingBuffer = nullptr;
	hr = pDevice_->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
	COM_ERROR_IF_FAILED(hr, "can't create a staging buffer");


	// copy the entire contents of the source resource to the destination 
	// resource using the GPU (from the anotherBuffer into the statingBuffer)
	pDeviceContext_->CopyResource(pStagingBuffer, anotherBuffer.Get());

	// map the staging buffer
	hr = pDeviceContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
	COM_ERROR_IF_FAILED(hr, "can't map the staging buffer");



	// ------------------- CREATE A DESTINATION INDEX BUFFER ------------------- //

	// if the buffer has already been initialized before
	if (this->pBuffer_ != nullptr)
		_RELEASE(this->pBuffer_);

	// get the description of the anotherBuffer
	anotherBuffer.Get()->GetDesc(&dstBufferDesc);

	UINT* pIndices = new UINT[this->bufferSize_];
	CopyMemory(pIndices, mappedSubresource.pData, sizeof(UINT) * this->bufferSize_);


	// fill in initial indices data
	ZeroMemory(&initBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	initBufferData.pSysMem = pIndices;
	initBufferData.SysMemPitch = 0;
	initBufferData.SysMemSlicePitch = 0;

	// create an index buffer
	hr = this->pDevice_->CreateBuffer(&dstBufferDesc, &initBufferData, &pBuffer_);
	COM_ERROR_IF_FAILED(hr, "can't create an index buffer");

	// unmap the staging buffer
	pDeviceContext_->Unmap(pStagingBuffer, 0);


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

// return a number of the indices
UINT IndexBuffer::GetBufferSize() const
{
	return this->bufferSize_;
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
