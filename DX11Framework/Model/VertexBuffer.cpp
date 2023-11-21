///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      VertexBuffer.cpp
// Description:   this file contains implementations of virtual functionals 
//                of the VertexBufferInterface
//
// Created:       13.11.22
///////////////////////////////////////////////////////////////////////////////////////////////
#include "VertexBuffer.h"







VertexBuffer::VertexBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: VertexBufferInterface(pDevice, pDeviceContext)
{
}





///////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////


HRESULT VertexBuffer::Initialize(const std::vector<VERTEX> & verticesArr,
	const bool isDynamic)  // this flag defines either a buffer must be dynamic or not
{
	// initialize a vertex buffer with vertices data

	// check input params
	COM_ERROR_IF_FALSE(verticesArr.empty() == false, "the input vertices array is empty");

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// setup the number of vertices and stride size
	this->vertexCount_ = (UINT)verticesArr.size();
	this->stride_ = sizeof(VERTEX);

	
	// accordingly to the isDynamic flag we setup the vertex buffer description
	vertexBufferDesc.Usage          = (isDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = (isDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
	vertexBufferDesc.ByteWidth      = this->stride_ * this->vertexCount_;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.MiscFlags      = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// create a buffer using the description and initial vertices data
	hr = InitializeHelper(vertexBufferDesc, verticesArr);
	COM_ERROR_IF_FAILED(hr, "can't create a vertex buffer");


	return hr;
} // end Initialize

///////////////////////////////////////////////////////////

bool VertexBuffer::UpdateDynamic(const std::vector<VERTEX> & verticesArr)
{
	// update the DYNAMIC vertex buffer with new vertices data

	try
	{
		// check input params
		bool isBufferDynamic = this->GetUsage() == D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		COM_ERROR_IF_FALSE(isBufferDynamic, "the buffer you want to update isn't dynamic");
		COM_ERROR_IF_FALSE(verticesArr.empty() == false, "the input vertices array is empty");
	
		// map the buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->pDeviceContext_->Map(pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		COM_ERROR_IF_FAILED(hr, "failed to map the vertex buffer");

		// copy new data into the buffer
		CopyMemory(mappedResource.pData, verticesArr.data(), this->stride_ * this->vertexCount_);

		// unmap the buffer
		this->pDeviceContext_->Unmap(this->pBuffer_, 0);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

} // end UpdateDynamic

///////////////////////////////////////////////////////////

bool VertexBuffer::CopyBuffer(const VertexBufferInterface & anotherBuffer)
{
	// this function copies data from the anotherBuffer into the current one;

	// check input params
	COM_ERROR_IF_FALSE(anotherBuffer.GetVertexCount(), "there is no vertices in the anotherBuffer");

	HRESULT hr = S_OK;

	std::vector<VERTEX> verticesArr;               // vertices for a destination buffer
	D3D11_SUBRESOURCE_DATA initBufferData;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	D3D11_BUFFER_DESC dstBufferDesc;
	ID3D11Buffer* pStagingBuffer = nullptr;

	try
	{
		// setup the number of vertices and stride size
		this->vertexCount_ = anotherBuffer.GetVertexCount();
		this->stride_ = anotherBuffer.GetStride();


		////////////////  CREATE A STAGING BUFFER AND COPY DATA INTO IT  ////////////////

		// setup the staging buffer description
		D3D11_BUFFER_DESC stagingBufferDesc;
		ZeroMemory(&stagingBufferDesc, sizeof(D3D11_BUFFER_DESC));

		stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
		stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingBufferDesc.ByteWidth = this->stride_ * this->vertexCount_;

		// create a staging buffer for reading data from the anotherBuffer
		hr = pDevice_->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
		COM_ERROR_IF_FAILED(hr, "can't create a staging buffer");

		// copy the entire contents of the source resource to the destination 
		// resource using the GPU (from the anotherBuffer into the statingBuffer)
		pDeviceContext_->CopyResource(pStagingBuffer, anotherBuffer.Get());

		// map the staging buffer
		hr = pDeviceContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
		COM_ERROR_IF_FAILED(hr, "can't map the staging buffer");



		//////////////////////  CREATE A DESTINATION VERTEX BUFFER  //////////////////////

		// if the buffer has already been initialized before
		if (this->pBuffer_ != nullptr)
			_RELEASE(this->pBuffer_);

		// get the description of the anotherBuffer
		anotherBuffer.Get()->GetDesc(&dstBufferDesc);

		// allocate memory for vertices of the destination buffer and fill it with data
		verticesArr.resize(this->vertexCount_);
		CopyMemory(verticesArr.data(), mappedSubresource.pData, this->stride_ * this->vertexCount_);

		// fill in initial vertices data for the buffer
		ZeroMemory(&initBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		initBufferData.pSysMem = verticesArr.data();

		// create a vertex buffer
		hr = this->pDevice_->CreateBuffer(&dstBufferDesc, &initBufferData, &pBuffer_);
		COM_ERROR_IF_FAILED(hr, "can't create a vertex buffer");


		// in the end we unmap the staging buffer and release it
		pDeviceContext_->Unmap(pStagingBuffer, 0);
		_RELEASE(pStagingBuffer);

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for vertices of buffer");
	}
	catch (COMException & e)
	{
		// in case of any error we unmap the staging buffer and release it
		pDeviceContext_->Unmap(pStagingBuffer, 0);
		_RELEASE(pStagingBuffer);

		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't copy a vertex buffer");
		return false;
	}

	return true;

} // end CopyBuffer



///////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC GETTERS
///////////////////////////////////////////////////////////////////////////////////////////////

ID3D11Buffer* VertexBuffer::Get() const
{
	// get a pointer to the vertex buffer
	return this->pBuffer_;
}

///////////////////////////////////////////////////////////

ID3D11Buffer* const* VertexBuffer::GetAddressOf() const
{
	// get a double pointer to the vertex buffer
	return &(this->pBuffer_);
}

///////////////////////////////////////////////////////////

const UINT VertexBuffer::GetVertexCount() const
{
	// get a number of vertices
	return this->vertexCount_;
}

///////////////////////////////////////////////////////////

const UINT VertexBuffer::GetStride() const
{
	// get the stride size
	return this->stride_;
}

///////////////////////////////////////////////////////////

const UINT* VertexBuffer::GetAddressOfStride() const
{
	// get a pointer to the stride variable
	return &stride_;
}

///////////////////////////////////////////////////////////

D3D11_USAGE VertexBuffer::GetUsage() const
{
	// get a type of buffer using
	D3D11_BUFFER_DESC bufferDesc;
	this->pBuffer_->GetDesc(&bufferDesc);

	return bufferDesc.Usage;
}















////////////////////////////////////////////////////////////////////////////////////////////////
//                           PRIVATE FUNCTIONS (HELPERS)
////////////////////////////////////////////////////////////////////////////////////////////////


HRESULT VertexBuffer::InitializeHelper(const D3D11_BUFFER_DESC & buffDesc,
	const std::vector<VERTEX> & verticesArr)
{
	// this function helps to initialize a vertex buffer of ANY type (usage)

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	// if the vertex buffer has already been initialized before
	if (this->pBuffer_ != nullptr)
		_RELEASE(pBuffer_);

	// fill in initial data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = verticesArr.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// try to create a vertex buffer
	return this->pDevice_->CreateBuffer(&buffDesc, &vertexBufferData, &pBuffer_);
}

///////////////////////////////////////////////////////////

