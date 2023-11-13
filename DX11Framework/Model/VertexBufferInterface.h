#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>

#include "../Engine/macros.h"
#include "../Engine/COMException.h"
#include "Vertex.h"

class VertexBufferInterface
{
public:
	VertexBufferInterface(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	VertexBufferInterface(const VertexBufferInterface & anotherBuffer);
	virtual ~VertexBufferInterface();

	virtual VertexBufferInterface & operator=(const VertexBufferInterface & anotherBuffer) = 0;

	// initialize a new vertex buffer of numVertices with data
	virtual HRESULT Initialize(const VERTEX* pData, const UINT numVertices) = 0;

	// update a DYNAMIC vertex buffer
	virtual bool UpdateDynamic(const VERTEX* pData) = 0;

	// GETTERS
	virtual ID3D11Buffer* Get() const = 0;                  // get a pointer to the vertex buffer
	virtual ID3D11Buffer* const* GetAddressOf() const = 0;  // get a double pointer to the vertex buffer
	virtual D3D11_USAGE GetUsage() const = 0;
	virtual const UINT  GetBufferSize() const = 0;          // get a number of vertices
	virtual const UINT  GetStride() const = 0;              // get the stride size
	virtual const UINT* GetAddressOfStride() const = 0;     // get a pointer to the stride variable


private:
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
	UINT stride_ = 0;                          // a stride size
	UINT bufferSize_ = 0;                      // a number of vertices
};