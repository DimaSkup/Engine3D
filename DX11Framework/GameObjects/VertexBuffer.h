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

//////////////////////////////////
// Class name: VertexBuffer
//////////////////////////////////
class VertexBuffer final
{
private:
	struct VertexBufferData
	{
		ID3D11Buffer* pBuffer_ = nullptr;          // a pointer to the vertex buffer
		UINT stride_ = 0;                          // a stride size
		UINT vertexCount_ = 0;                     // a number of vertices
		D3D11_USAGE usageType_ = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	};

public:
	VertexBuffer();
	~VertexBuffer();

	// initialize a new vertex buffer with data
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::vector<VERTEX> & verticesArr, const bool isDynamic = false);

	// update a DYNAMIC vertex buffer with new vertices
	bool UpdateDynamic(ID3D11DeviceContext* pDeviceContext, const std::vector<VERTEX> & verticesArr);

	// copy data from the anotherBuffer into the current one
	void CopyBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const VertexBuffer & anotherBuffer);

	// GETTERS
	const VertexBufferData & GetData() const;
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
		const std::vector<VERTEX> & verticesArr);

private:
	VertexBufferData data_;
};
