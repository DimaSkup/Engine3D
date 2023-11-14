///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     VertexBuffer.h
// Description:  implementations of virtual functionals of the VertexBufferInterface
//               for easier using of the vertex buffers
//
//
// Revising:     12.12.22
///////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "VertexBufferInterface.h"

//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
class VertexBuffer : public VertexBufferInterface
{
public:
	VertexBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	// initialize a new vertex buffer with data
	virtual HRESULT Initialize(const std::vector<VERTEX> & verticesArr, const bool isDynamic = false);

	// update a DYNAMIC vertex buffer with new vertices
	virtual bool UpdateDynamic(const std::vector<VERTEX> & verticesArr);

	// copy data from the anotherBuffer into the current one
	virtual bool CopyBuffer(const VertexBufferInterface & anotherBuffer);

	// GETTERS
	virtual ID3D11Buffer* Get() const;                  // get a pointer to the vertex buffer
	virtual ID3D11Buffer* const* GetAddressOf() const;  // get a double pointer to the vertex buffer
	virtual const UINT  GetBufferSize() const;          // get a number of vertices
	virtual const UINT  GetStride() const;              // get the stride size
	virtual const UINT* GetAddressOfStride() const;     // get a pointer to the stride variable
	virtual D3D11_USAGE GetUsage() const;               // get a type of buffer using

private:
	// restrict a copying of this class instance 
	//(you have to do it through the virtual CopyBuffer() function)
	VertexBuffer(const VertexBuffer & obj);
	VertexBuffer & operator=(const VertexBuffer & obj);


private:
	// helps to initialize a buffer
	HRESULT InitializeHelper(const D3D11_BUFFER_DESC & buffDesc,
		const std::vector<VERTEX> & verticesArr);

	// a buffers copying helper
	void CopyBufferFromTo(ID3D11Buffer* pSrc, ID3D11Buffer* pDst);
};
