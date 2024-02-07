///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      VertexBuffer.cpp
// Description:   this file contains implementations of virtual functionals 
//                of the VertexBufferInterface
//
// Created:       13.11.22
///////////////////////////////////////////////////////////////////////////////////////////////
#include "VertexBuffer.h"


#include "../Engine/log.h"
#include "../Engine/macros.h"
#include "../Engine/COMException.h"
#include "Vertex.h"




VertexBuffer::VertexBuffer()
{

}

VertexBuffer::~VertexBuffer()
{
}





///////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////


void VertexBuffer::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & verticesArr,
	const bool isDynamic)  // this flag defines either a buffer must be dynamic or not
{
	// initialize a vertex buffer with vertices data

	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty");
	

	VertexBufferData initData;       // local data struct with params of the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// setup the number of vertices and stride size
	initData.vertexCount_ = (UINT)verticesArr.size();
	initData.stride_ = sizeof(VERTEX);
	
	// accordingly to the isDynamic flag we setup the vertex buffer description
	if (isDynamic)
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = 0;
	}

	vertexBufferDesc.ByteWidth      = initData.stride_ * initData.vertexCount_;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.MiscFlags      = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// create a buffer using the description and initial vertices data
	InitializeHelper(pDevice, vertexBufferDesc, verticesArr);
	
	// initialize the data of this vertex buffer
	this->data_ = initData;

	return;

} // end Initialize

///////////////////////////////////////////////////////////

bool VertexBuffer::UpdateDynamic(ID3D11DeviceContext* pDeviceContext, 
	const std::vector<VERTEX> & verticesArr)
{
	// update the DYNAMIC vertex buffer with new vertices data

	try
	{
		VertexBufferData & data = data_;

		// map the buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext->Map(data.pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		COM_ERROR_IF_FAILED(hr, "failed to map the vertex buffer");

		// copy new data into the buffer
		CopyMemory(mappedResource.pData, verticesArr.data(), data.stride_ * data.vertexCount_);

		// unmap the buffer
		pDeviceContext->Unmap(data.pBuffer_, 0);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

} // end UpdateDynamic

///////////////////////////////////////////////////////////

void VertexBuffer::CopyBuffer(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext, 
	const VertexBuffer & inOriginBuffer)
{
	// this function copies data from the anotherBuffer into the current one;

	VertexBufferData bufferData = inOriginBuffer.GetData();

	// check input params
	COM_ERROR_IF_FALSE(bufferData.vertexCount_, "there is no vertices in the anotherBuffer");

	HRESULT hr = S_OK;

	std::vector<VERTEX> verticesArr;               // vertices for a destination buffer
	D3D11_SUBRESOURCE_DATA initBufferData;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	D3D11_BUFFER_DESC dstBufferDesc;
	ID3D11Buffer* pStagingBuffer = nullptr;



	try
	{
		////////////////  CREATE A STAGING BUFFER AND COPY DATA INTO IT  ////////////////

		// setup the staging buffer description
		D3D11_BUFFER_DESC stagingBufferDesc;
		ZeroMemory(&stagingBufferDesc, sizeof(D3D11_BUFFER_DESC));

		stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
		stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingBufferDesc.ByteWidth = bufferData.stride_ * bufferData.vertexCount_;

		// create a staging buffer for reading data from the anotherBuffer
		hr = pDevice->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
		COM_ERROR_IF_FAILED(hr, "can't create a staging buffer");

		// copy the entire contents of the source resource to the destination 
		// resource using the GPU (from the anotherBuffer into the statingBuffer)
		pDeviceContext->CopyResource(pStagingBuffer, bufferData.pBuffer_);

		// map the staging buffer
		hr = pDeviceContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
		COM_ERROR_IF_FAILED(hr, "can't map the staging buffer");

		// in the end we unmap the staging buffer and release it
		pDeviceContext->Unmap(pStagingBuffer, 0);
		_RELEASE(pStagingBuffer);


		//////////////////////  CREATE A DESTINATION VERTEX BUFFER  //////////////////////

		// get the description of the originBuffer
		bufferData.pBuffer_->GetDesc(&dstBufferDesc);

		// allocate memory for vertices of the destination buffer and fill it with data
		verticesArr.resize(bufferData.vertexCount_);
		CopyMemory(verticesArr.data(), mappedSubresource.pData, bufferData.stride_ * bufferData.vertexCount_);

		// create and initialize a buffer with data
		this->InitializeHelper(pDevice, dstBufferDesc, verticesArr);

		// update the data of this vertex buffer
		data_ = bufferData;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for vertices of buffer");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't copy a vertex buffer");
	}

	return;

} // end CopyBuffer



///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC GETTERS
//
///////////////////////////////////////////////////////////////////////////////////////////////

const VertexBuffer::VertexBufferData & VertexBuffer::GetData() const
{
	return data_;
}

ID3D11Buffer* VertexBuffer::Get() const
{
	// get a pointer to the vertex buffer
	return data_.pBuffer_;
}

///////////////////////////////////////////////////////////

ID3D11Buffer* const* VertexBuffer::GetAddressOf() const
{
	// get a double pointer to the vertex buffer
	return &(data_.pBuffer_);
}

///////////////////////////////////////////////////////////

const UINT VertexBuffer::GetVertexCount() const
{
	// get a number of vertices
	return data_.vertexCount_;
}

///////////////////////////////////////////////////////////

const UINT VertexBuffer::GetStride() const
{
	// get the stride size
	return data_.stride_;
}

///////////////////////////////////////////////////////////

const UINT* VertexBuffer::GetAddressOfStride() const
{
	// get a pointer to the stride variable
	return &(data_.stride_);
}

///////////////////////////////////////////////////////////
















////////////////////////////////////////////////////////////////////////////////////////////////
//                           PRIVATE FUNCTIONS (HELPERS)
////////////////////////////////////////////////////////////////////////////////////////////////

void VertexBuffer::InitializeHelper(ID3D11Device* pDevice,
	const D3D11_BUFFER_DESC & buffDesc,
	const std::vector<VERTEX> & verticesArr)
{
	// this function helps to initialize a vertex buffer of ANY type (usage)

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ID3D11Buffer* pBuffer = data_.pBuffer_;

	// if the vertex buffer has already been initialized before
	_RELEASE(pBuffer);

	// fill in initial data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = verticesArr.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// try to create a vertex buffer
	const HRESULT hr = pDevice->CreateBuffer(&buffDesc, &vertexBufferData, &pBuffer);
	COM_ERROR_IF_FAILED(hr, "can't create a vertex buffer");

	return;
}

///////////////////////////////////////////////////////////

