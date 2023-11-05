////////////////////////////////////////////////////////////////////
// Filename:     VertexBuffer.h
// Description:  this class is needed for easier using of 
//               vertex buffers for models and GUI text;
// Revising:     12.12.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>


//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
template<class T>
class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	
	HRESULT InitializeDefault(ID3D11Device* pDevice, T* data, UINT numVertices);   // initialize a DEFAULT vertex buffer with vertices data
	HRESULT InitializeDynamic(ID3D11Device* pDevice, T* data, UINT numVertices);   // initialize a DYNAMIC vertex buffer with vertices data
	bool UpdateDynamic(ID3D11DeviceContext* pDeviceContext, T* data);              // update a DYNAMIC vertex buffer
	

	ID3D11Buffer* Get() const;                 // get a pointer to the vertex buffer
	ID3D11Buffer* const* GetAddressOf() const; // get a double pointer to the vertex buffer
	UINT GetBufferSize() const;                // get a number of vertices
	const UINT GetStride() const;              // get the stride size
	const UINT* GetAddressOfStride() const;    // get a pointer to the stride variable

private:
	VertexBuffer(const VertexBuffer<T>& rhs);
	HRESULT InitializeHelper(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& buffDesc, T* data, UINT numVertices);   // helps to initialize a vertex buffer of ANY type (usage)

private:
	ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
	std::unique_ptr<UINT> pStride_ = nullptr;  // a stride size
	UINT bufferSize_ = 0;                      // a number of vertices
};



// a constructor
template<class T>
VertexBuffer<T>::VertexBuffer()
{
}


// a destructor
template<class T>
VertexBuffer<T>::~VertexBuffer()
{
	_RELEASE(pBuffer_);
}


// initialize a DEFAULT vertex buffer with vertices data
template<class T>
HRESULT VertexBuffer<T>::InitializeDefault(ID3D11Device* pDevice, T* data, UINT numVertices)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC vertexBufferDesc;

	// setup the vertex buffer description
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = InitializeHelper(pDevice, vertexBufferDesc, data, numVertices);
	COM_ERROR_IF_FAILED(hr, "can't create a DEFAULT vertex buffer");


	return hr;
} // InitializeDefault()


  // initialize a DYNAMIC vertex buffer with vertices data
template<class T>
HRESULT VertexBuffer<T>::InitializeDynamic(ID3D11Device* pDevice, T* data, UINT numVertices)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC vertexBufferDesc;

	// setup the vertex buffer description
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = this->InitializeHelper(pDevice, vertexBufferDesc, data, numVertices);
	COM_ERROR_IF_FAILED(hr, "can't create a DYNAMIC vertex buffer");


	return hr;
}  // InitializeDynamic()


// helps to initialize a vertex buffer of ANY type (usage)
template<class T>
HRESULT VertexBuffer<T>::InitializeHelper(ID3D11Device* pDevice,
										  const D3D11_BUFFER_DESC& buffDesc,
										  T* data,
										  UINT numVertices)
{
	D3D11_SUBRESOURCE_DATA vertexBufferData;

	// if the vertex buffer has already been initialized before
	if (pBuffer_ != nullptr)  
		_RELEASE(pBuffer_);

	// define the number of vertices and stride size
	this->bufferSize_ = numVertices;
	if (this->pStride_ == nullptr)  // if a stride hasn't been initialized once before
		this->pStride_ = std::make_unique<UINT>(static_cast<UINT>(sizeof(T)));


	// fill in initial vertices data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = data;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// try to create a vertex buffer
	return pDevice->CreateBuffer(&buffDesc, &vertexBufferData, &pBuffer_);
}


 // update the DYNAMIC vertex buffer with vertices data
template<class T>
bool VertexBuffer<T>::UpdateDynamic(ID3D11DeviceContext* pDeviceContext, T* data)
{                            

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	COM_ERROR_IF_FAILED(hr, "failed to map the vertex buffer");

	CopyMemory(mappedResource.pData, data, sizeof(T) * bufferSize_);
	pDeviceContext->Unmap(pBuffer_, 0);

	return true;
} // UpdateDynamic()




  // get a pointer to the vertex buffer
template<class T>
ID3D11Buffer* VertexBuffer<T>::Get() const
{
	return pBuffer_;
}


// get a double pointer to the vertex buffer
template<class T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const
{
	return &pBuffer_;
}


// get a number of vertices
template<class T>
UINT VertexBuffer<T>::GetBufferSize() const
{
	return this->bufferSize_;
}


// get the stride size
template<class T>
const UINT VertexBuffer<T>::GetStride() const
{
	return *this->pStride_;
}


// get a pointer to the stride variable
template<class T>
const UINT* VertexBuffer<T>::GetAddressOfStride() const
{

	return this->pStride_.get();
}