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
	IndexBuffer() {}

	// initialize the index buffer with indices data
	HRESULT Initialize(ID3D11Device* pDevice, UINT* data, UINT numIndices);

	ID3D11Buffer* Get() const;                   // return a pointer the index buffer
	ID3D11Buffer* const* GetAddressOf() const;   // return a double pointer to the index buffer
	UINT GetBufferSize() const;                  // return a number of the indices

private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	ID3D11Buffer* pBuffer_ = nullptr;            // a pointer to the index buffer
	UINT bufferSize_ = 0;                        // a number of indices
};

