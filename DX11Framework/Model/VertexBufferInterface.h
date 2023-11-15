///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     VertexBufferInterface.h
// Description:  this class is a bunch of virtual functionals 
//               for easier using of the vertex buffers;
//
//               another models classes have to use vertex buffers
//               ONLY THROUGH this virtual interface;         
//
// Revising:     12.12.22
///////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
//#include <memory>
#include <vector>

#include "../Engine/log.h"
#include "../Engine/macros.h"
#include "../Engine/COMException.h"
#include "Vertex.h"

class VertexBufferInterface
{
public:
	VertexBufferInterface(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~VertexBufferInterface();

	// initialize a new vertex buffer of numVertices with data
	virtual HRESULT Initialize(const std::vector<VERTEX> & verticesArr, const bool isDynamic = false) = 0;

	// update a DYNAMIC vertex buffer
	virtual bool UpdateDynamic(const std::vector<VERTEX> & verticesArr) = 0;

	// copy data from the anotherBuffer into the current one
	virtual bool CopyBuffer(const VertexBufferInterface & anotherBuffer) = 0;

	// GETTERS
	virtual ID3D11Buffer* Get() const = 0;                  // get a pointer to the vertex buffer
	virtual ID3D11Buffer* const* GetAddressOf() const = 0;  // get a double pointer to the vertex buffer
	virtual const UINT  GetBufferSize() const = 0;          // get a number of vertices
	virtual const UINT  GetStride() const = 0;              // get the stride size
	virtual const UINT* GetAddressOfStride() const = 0;     // get a pointer to the stride variable
	virtual D3D11_USAGE GetUsage() const = 0;

private:  
	// restrict a copying of this class instance 
	//(you have to do it through the virtual CopyBuffer() function)
	VertexBufferInterface(const VertexBufferInterface & obj);
	//VertexBufferInterface & operator=(const VertexBufferInterface & obj);


protected:
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
	UINT stride_ = 0;                          // a stride size
	UINT bufferSize_ = 0;                      // a number of vertices
};