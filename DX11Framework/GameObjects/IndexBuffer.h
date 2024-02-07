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
class IndexBuffer final
{
private:
	struct IndexBufferData
	{
		ID3D11Buffer* pBuffer_ = nullptr;            // a pointer to the index buffer
		UINT indexCount_ = 0;                        // a number of indices
	};

public:
	IndexBuffer();
	~IndexBuffer();

	// initialize a new index buffer with indices data
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::vector<UINT> & indicesArr);

	// copy data from the anotherBuffer into the current one
	void CopyBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const IndexBuffer & originBuffer);

	const IndexBufferData & GetData() const;
	ID3D11Buffer* Get() const;                   // return a pointer the index buffer
	ID3D11Buffer* const* GetAddressOf() const;   // return a double pointer to the index buffer
	UINT GetIndexCount() const;                  // return a number of the indices

private:
	// restrict a copying of this class instance 
	//(you have to do it through the CopyBuffer() function)
	IndexBuffer(const IndexBuffer & obj);
	IndexBuffer & operator=(const IndexBuffer & obj);


private:
	void InitializeHelper(ID3D11Device* pDevice,
		const D3D11_BUFFER_DESC & buffDesc,
		const std::vector<UINT> & indicesArr);

private:
	IndexBufferData data_;
};

