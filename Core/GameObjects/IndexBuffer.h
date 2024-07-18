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


public:
	IndexBuffer();
	IndexBuffer(ID3D11Device* pDevice, const std::vector<UINT>& indices);
	IndexBuffer(IndexBuffer&& rhs);
	~IndexBuffer();

	// restrict a copying of this class instance 
	//(you have to execute copyting using the CopyBuffer() function)
	IndexBuffer& operator=(const IndexBuffer& obj) = delete;
	IndexBuffer(const IndexBuffer& rhs) = delete;


	// Public modification API 
	void Initialize(ID3D11Device* pDevice, const std::vector<UINT> & indicesArr);
	void CopyBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const IndexBuffer& srcBuffer);

	// Public query API  
	inline ID3D11Buffer*        Get()           const { return pBuffer_; }
	inline ID3D11Buffer* const* GetAddressOf()  const { return &pBuffer_; }
	inline UINT                 GetIndexCount() const { return indexCount_; }

private:
	
	

private:
	void InitializeHelper(
		ID3D11Device* pDevice,
		const D3D11_BUFFER_DESC& buffDesc,
		const std::vector<UINT>& indicesArr);

private:
	ID3D11Buffer* pBuffer_ = nullptr;
	UINT          indexCount_ = 0;
};

