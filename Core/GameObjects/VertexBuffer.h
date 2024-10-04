// *********************************************************************************
// Filename:     VertexBuffer.h
// Description:  a functional wrapper for vertex buffers
//
//
// Revising:     12.12.22
// *********************************************************************************
#pragma once


#include <d3d11.h>
#include <vector>

#include "../Render/d3dclass.h"
#include "../Engine/log.h"
#include "../Common/Assert.h"
#include "../Common/MemHelpers.h"
#include "Vertex.h"


//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
template <typename T>
class VertexBuffer final
{
public:
	VertexBuffer();
	VertexBuffer(ID3D11Device* pDevice, const std::vector<T>& vertices, const bool isDynamic);
	VertexBuffer(VertexBuffer&& rhs);
	
	~VertexBuffer();

	// restrict a copying of this class instance using the asignment operator
	//(you have to do it through the CopyBuffer() function)
	VertexBuffer(const VertexBuffer& obj) = delete;
	VertexBuffer& operator=(const VertexBuffer& obj) = delete;


	// Public modification API
	void Initialize(
		ID3D11Device* pDevice,
		const std::vector<T> & verticesArr, 
		const bool isDynamic = false);

	void UpdateDynamic(
		ID3D11DeviceContext* pDeviceContext, 
		const std::vector<T> & verticesArr);

	void CopyBuffer(
		ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const VertexBuffer & anotherBuffer);


	// Public query API  
	inline void GetBufferAndStride(
		ID3D11Buffer* pBuffer,
		UINT*& pStride)
	{
		pBuffer = pBuffer_;
		pStride = &stride_;
	}

	inline UINT GetStride()                 const { return stride_; }
	inline const UINT* GetAddressOfStride() const { return &stride_; }

	inline UINT GetVertexCount()  const { return vertexCount_; }
	inline D3D11_USAGE GetUsage() const { return usageType_; }

	ID3D11Buffer* Get() const;                  // get a pointer to the vertex buffer
	ID3D11Buffer* const* GetAddressOf() const;  // get a double pointer to the vertex buffer
	
private:

	void InitializeHelper(ID3D11Device* pDevice, 
		const D3D11_BUFFER_DESC & buffDesc,
		const std::vector<T> & verticesArr);

private:
	ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
	UINT stride_ = 0;                          // a stride size
	UINT vertexCount_ = 0;                     // a number of vertices
	D3D11_USAGE usageType_ = D3D11_USAGE::D3D11_USAGE_DEFAULT;
};



// *********************************************************************************

template <typename T>
VertexBuffer<T>::VertexBuffer()
{
}

template<typename T>
VertexBuffer<T>::VertexBuffer(VertexBuffer&& rhs)
	:
	pBuffer_(rhs.pBuffer_),
	stride_(rhs.stride_),
	vertexCount_(rhs.vertexCount_),
	usageType_(rhs.usageType_)
{
	Assert::NotNullptr(rhs.pBuffer_, "ptr to the buffer == nullptr");

	rhs.pBuffer_ = nullptr;
	rhs.stride_ = 0;
	rhs.vertexCount_ = 0;
}

template<typename T>
VertexBuffer<T>::VertexBuffer(ID3D11Device* pDevice,
	const std::vector<T>& verticesArr,
	const bool isDynamic)                  // this flag defines either a buffer must be dynamic or not
{
	Initialize(pDevice, verticesArr, isDynamic);
}

template <typename T>
VertexBuffer<T>::~VertexBuffer()
{
	SafeRelease(&pBuffer_);
}






///////////////////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void VertexBuffer<T>::Initialize(
	ID3D11Device* pDevice,
	const std::vector<T>& verticesArr,
	const bool isDynamic)                  // this flag defines either a buffer must be dynamic or not
{
	// initialize a vertex buffer with vertices data

	// check input params
	Assert::NotZero(verticesArr.size(), "the input vertices array is empty");

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// accordingly to the isDynamic flag we setup the vertex buffer description
	if (isDynamic)
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                       // specify how the buffer will be used
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;           // specify how the CPU will access the buffer
	}
	else
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;                       // specify how the buffer will be used
		vertexBufferDesc.CPUAccessFlags = 0;                                // specify how the CPU will access the buffer
	}

	vertexBufferDesc.ByteWidth = sizeof(T) * (UINT)verticesArr.size();      // the size, in bytes, of the vertex buffer we are going to create
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                  
	vertexBufferDesc.StructureByteStride = 0;                               // the size, in bytes, of a single element stored in the structured buffer. A structure buffer is a buffer that stores elements of equal size
	vertexBufferDesc.MiscFlags = 0;

	// create a buffer using the description and initial vertices data
	InitializeHelper(pDevice, vertexBufferDesc, verticesArr);

	// setup the number of vertices, stride size, the usage type, etc.
	stride_      = sizeof(T);
	vertexCount_ = (UINT)verticesArr.size();
	usageType_   = vertexBufferDesc.Usage;
}

///////////////////////////////////////////////////////////

template <typename T>
void VertexBuffer<T>::UpdateDynamic(
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<T> & verticesArr)
{
	// update this DYNAMIC vertex buffer with new vertices

	try
	{
		Assert::True(usageType_ == D3D11_USAGE_DYNAMIC, "the usage type of this vertex buffer must be dynamic");

		// map the buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		const HRESULT hr = pDeviceContext->Map(pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		Assert::NotFailed(hr, "failed to map the vertex buffer");

		// copy new data into the buffer
		CopyMemory(mappedResource.pData, verticesArr.data(), stride_ * vertexCount_);

		pDeviceContext->Unmap(pBuffer_, 0);
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		throw EngineException("can't update the dynamic vertex buffer");
	}

	return;
} 

///////////////////////////////////////////////////////////

template <typename T>
void VertexBuffer<T>::CopyBuffer(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const VertexBuffer & inOriginBuffer)
{
	// this function copies data from the inOriginBuffer into the current one
	// and creates a new vertex buffer using this data;

	Assert::NotNullptr(pDevice, "a ptr to the device == nullptr");
	Assert::NotNullptr(pDeviceContext, "a ptr to the device context == nullptr");

	ID3D11Buffer* pBuffer = inOriginBuffer.Get();
	const UINT stride = inOriginBuffer.GetStride();
	const UINT vertexCount = inOriginBuffer.GetVertexCount();
	const D3D11_USAGE usageType = inOriginBuffer.GetUsage();

	// check input params
	Assert::NotNullptr(pBuffer, "ptr to buffer == nullptr");
	Assert::NotZero(vertexCount, "there is no vertices in the origin vertex buffer");

	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	D3D11_BUFFER_DESC dstBufferDesc;
	ID3D11Buffer* pStagingBuffer = nullptr;
	std::vector<T> verticesArr;               // vertices for a destination buffer

	try
	{
		// ------------  CREATE A STAGING BUFFER AND COPY DATA INTO IT  ------------

		// setup the staging buffer description
		D3D11_BUFFER_DESC stagingBufferDesc;
		ZeroMemory(&stagingBufferDesc, sizeof(D3D11_BUFFER_DESC));

		stagingBufferDesc.Usage          = D3D11_USAGE_STAGING;
		stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingBufferDesc.ByteWidth      = stride * vertexCount;

		// create a staging buffer for reading data from the anotherBuffer
		hr = pDevice->CreateBuffer(&stagingBufferDesc, nullptr, &pStagingBuffer);
		Assert::NotFailed(hr, "can't create a staging buffer");

		// copy the entire contents of the source resource to the destination 
		// resource using the GPU (from the origin buffer into the statingBuffer)
		pDeviceContext->CopyResource(pStagingBuffer, inOriginBuffer.Get());

		// map the staging buffer
		hr = pDeviceContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
		Assert::NotFailed(hr, "can't map the staging buffer");

		pDeviceContext->Unmap(pStagingBuffer, 0);
		SafeRelease(&pStagingBuffer);


		// ---------------  CREATE A DESTINATION Vertex3D BUFFER  ---------------

		// allocate memory for vertices of the destination buffer and fill it with data
		verticesArr.resize(vertexCount);
		CopyMemory(verticesArr.data(), mappedSubresource.pData, stride * vertexCount);

		pBuffer->GetDesc(&dstBufferDesc);

		// create and initialize a buffer with data
		InitializeHelper(pDevice, dstBufferDesc, verticesArr);

		// set params of this vertex buffer (DON'T COPY HERE A PTR TO THE ORIGIN BUFFER)
		stride_      = stride;
		vertexCount_ = vertexCount;
		usageType_   = usageType;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for vertices of buffer");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		throw EngineException("can't copy a vertex buffer");
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC QUERY API
///////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ID3D11Buffer* VertexBuffer<T>::Get() const
{
	// get a pointer to the vertex buffer
	return pBuffer_;
}

///////////////////////////////////////////////////////////

template <typename T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const
{
	// get a double pointer to the vertex buffer
	return &(pBuffer_);
}
#if 0
///////////////////////////////////////////////////////////

template <typename T>
const UINT VertexBuffer<T>::GetVertexCount() const
{
	// get a number of vertices
	return vertexCount_;
}

///////////////////////////////////////////////////////////

template <typename T>
const UINT VertexBuffer<T>::GetStride() const
{
	// get the stride size
	return stride_;
}

///////////////////////////////////////////////////////////

template <typename T>
const UINT* VertexBuffer<T>::GetAddressOfStride() const
{
	// get a pointer to the stride variable
	return &(stride_);
}

///////////////////////////////////////////////////////////

template <typename T>
D3D11_USAGE VertexBuffer<T>::GetUsage() const
{
	// get a type of buffer using
	return usageType_;
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////
//                           PRIVATE FUNCTIONS (HELPERS)
////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void VertexBuffer<T>::InitializeHelper(
	ID3D11Device* pDevice,
	const D3D11_BUFFER_DESC & buffDesc,
	const std::vector<T> & verticesArr)
{
	// this function helps to initialize a vertex buffer of ANY type (usage)

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	// fill in initial data 
	//ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = verticesArr.data();   // a pointer to a system memory array which contains the data to initialize the vertex buffer
	vertexBufferData.SysMemPitch = 0;                // not used for vertex buffers
	vertexBufferData.SysMemSlicePitch = 0;           // not used for vertex buffers

	// if the vertex buffer has already been initialized before
	SafeRelease(&pBuffer_);

	// try to create a vertex buffer
	const HRESULT hr = pDevice->CreateBuffer(&buffDesc, &vertexBufferData, &pBuffer_);
	Assert::NotFailed(hr, "can't create a vertex buffer");
}

///////////////////////////////////////////////////////////
