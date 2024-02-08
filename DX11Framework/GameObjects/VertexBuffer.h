///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     VertexBuffer.h
// Description:  implementations of virtual functionals of the VertexBufferInterface
//               for easier using of the vertex buffers
//
//
// Revising:     12.12.22
///////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


#include <d3d11.h>
#include <vector>

#include "../Engine/log.h"
#include "../Engine/macros.h"
#include "../Engine/COMException.h"
#include "Vertex.h"

namespace VertexBufferStorage
{
	struct VertexBufferData
	{
		ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
		UINT stride_ = 0;                          // a stride size
		UINT vertexCount_ = 0;                     // a number of vertices
		D3D11_USAGE usageType_ = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	};
};


//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
template <typename T>
class VertexBuffer final
{
public:
	VertexBuffer();
	~VertexBuffer();

	// initialize a new vertex buffer with data
	void Initialize(ID3D11Device* pDevice,
		const std::vector<T> & verticesArr, const bool isDynamic = false);

	// update a DYNAMIC vertex buffer with new vertices
	bool UpdateDynamic(ID3D11DeviceContext* pDeviceContext, 
		const std::vector<T> & verticesArr);

	// copy data from the anotherBuffer into the current one
	void CopyBuffer(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const VertexBuffer & anotherBuffer);

	// GETTERS
	const VertexBufferStorage::VertexBufferData & GetData() const;
	ID3D11Buffer* Get() const;                  // get a pointer to the vertex buffer
	ID3D11Buffer* const* GetAddressOf() const;  // get a double pointer to the vertex buffer
	const UINT  GetVertexCount() const;         // get a number of vertices
	const UINT  GetStride() const;              // get the stride size
	const UINT* GetAddressOfStride() const;     // get a pointer to the stride variable
	D3D11_USAGE GetUsage() const;               // get a type of buffer using

private:
	// restrict a copying of this class instance 
	//(you have to do it through the virtual CopyBuffer() function)
	VertexBuffer(const VertexBuffer & obj);
	VertexBuffer & operator=(const VertexBuffer & obj);


private:

	// helps to initialize a buffer
	void InitializeHelper(ID3D11Device* pDevice, 
		const D3D11_BUFFER_DESC & buffDesc,
		const std::vector<T> & verticesArr);

private:
	VertexBufferStorage::VertexBufferData data_;
};




///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



template <typename T>
VertexBuffer<T>::VertexBuffer()
{

}

template <typename T>
VertexBuffer<T>::~VertexBuffer()
{
}





///////////////////////////////////////////////////////////////////////////////////////////////
//                                 PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void VertexBuffer<T>::Initialize(ID3D11Device* pDevice,
	const std::vector<T> & verticesArr,
	const bool isDynamic)  // this flag defines either a buffer must be dynamic or not
{
	// initialize a vertex buffer with vertices data

	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty");


	VertexBufferStorage::VertexBufferData initData;       // local data struct with params of the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// setup the number of vertices and stride size
	initData.vertexCount_ = (UINT)verticesArr.size();
	initData.stride_ = sizeof(T);

	// accordingly to the isDynamic flag we setup the vertex buffer description
	if (isDynamic)
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		initData.usageType_ = vertexBufferDesc.Usage;
	}
	else
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = 0;

		initData.usageType_ = vertexBufferDesc.Usage;
	}

	vertexBufferDesc.ByteWidth = initData.stride_ * initData.vertexCount_;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// initialize the data of this vertex buffer
	this->data_ = initData;

	// create a buffer using the description and initial vertices data
	InitializeHelper(pDevice, vertexBufferDesc, verticesArr);



	return;

} // end Initialize

  ///////////////////////////////////////////////////////////

template <typename T>
bool VertexBuffer<T>::UpdateDynamic(ID3D11DeviceContext* pDeviceContext,
	const std::vector<T> & verticesArr)
{
	// update the DYNAMIC vertex buffer with new vertices data

	try
	{
		VertexBufferStorage::VertexBufferData & data = data_;

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

template <typename T>
void VertexBuffer<T>::CopyBuffer(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const VertexBuffer & inOriginBuffer)
{
	// this function copies data from the anotherBuffer into the current one;

	VertexBufferStorage::VertexBufferData bufferData = inOriginBuffer.GetData();

	// check input params
	COM_ERROR_IF_FALSE(bufferData.vertexCount_, "there is no vertices in the anotherBuffer");

	HRESULT hr = S_OK;

	std::vector<T> verticesArr;               // vertices for a destination buffer
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

template <typename T>
const VertexBufferStorage::VertexBufferData & VertexBuffer<T>::GetData() const
{
	return data_;
}

template <typename T>
ID3D11Buffer* VertexBuffer<T>::Get() const
{
	// get a pointer to the vertex buffer
	return data_.pBuffer_;
}

///////////////////////////////////////////////////////////

template <typename T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const
{
	// get a double pointer to the vertex buffer
	return &(data_.pBuffer_);
}

///////////////////////////////////////////////////////////

template <typename T>
const UINT VertexBuffer<T>::GetVertexCount() const
{
	// get a number of vertices
	return data_.vertexCount_;
}

///////////////////////////////////////////////////////////

template <typename T>
const UINT VertexBuffer<T>::GetStride() const
{
	// get the stride size
	return data_.stride_;
}

///////////////////////////////////////////////////////////

template <typename T>
const UINT* VertexBuffer<T>::GetAddressOfStride() const
{
	// get a pointer to the stride variable
	return &(data_.stride_);
}

///////////////////////////////////////////////////////////
















////////////////////////////////////////////////////////////////////////////////////////////////
//                           PRIVATE FUNCTIONS (HELPERS)
////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void VertexBuffer<T>::InitializeHelper(ID3D11Device* pDevice,
	const D3D11_BUFFER_DESC & buffDesc,
	const std::vector<T> & verticesArr)
{
	// this function helps to initialize a vertex buffer of ANY type (usage)

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	// if the vertex buffer has already been initialized before
	_RELEASE(data_.pBuffer_);

	// fill in initial data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = verticesArr.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// try to create a vertex buffer
	const HRESULT hr = pDevice->CreateBuffer(&buffDesc, &vertexBufferData, &data_.pBuffer_);
	COM_ERROR_IF_FAILED(hr, "can't create a vertex buffer");

	return;
}

///////////////////////////////////////////////////////////

