////////////////////////////////////////////////////////////////////
// Filename:     IndexBuffer.h
// Description:  this class is needed for easier using of 
//               indices buffers for models;
// Revising:     12.12.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <vector>
#include <memory>

#include "../Engine/Log.h"


//////////////////////////////////
// Class name: IndexBuffer
//////////////////////////////////
class IndexBuffer
{
public:
	IndexBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	IndexBuffer(const IndexBuffer & another);
	~IndexBuffer();

	IndexBuffer & operator=(const IndexBuffer & another);

	// initialize the index buffer with indices data
	HRESULT Initialize(const UINT* data, const UINT numIndices);

	ID3D11Buffer* Get() const;                   // return a pointer the index buffer
	ID3D11Buffer* const* GetAddressOf() const;   // return a double pointer to the index buffer
	UINT GetBufferSize() const;                  // return a number of the indices

private:
	HRESULT InitializeHelper(const D3D11_BUFFER_DESC & buffDesc,
		const UINT* data);

	// buffers copying helper
	void IndexBuffer::CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst);

private:
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	ID3D11Buffer* pBuffer_ = nullptr;            // a pointer to the index buffer
	UINT bufferSize_ = 0;                        // a number of indices
};

