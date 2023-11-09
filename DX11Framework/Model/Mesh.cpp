#include "Mesh.h"

Mesh::Mesh()
{
}

///////////////////////////////////////////////////////////

Mesh::Mesh(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & vertices,
	const std::vector<UINT> & indices)
{
	// check input params
	assert(vertices.size() > 0);
	assert(indices.size() > 0);

	try
	{
		this->pDeviceContext_ = pDeviceContext;

		// allocate memory for the buffers
		this->pVertexBuffer_ = new VertexBuffer<VERTEX>(pDeviceContext);
		this->pIndexBuffer_ = new IndexBuffer(pDeviceContext);


		// initialize the buffers
		// load vertex data into the buffer
		HRESULT hr = pVertexBuffer_->Initialize(pDevice,
			vertices.data(),
			static_cast<UINT>(vertices.size()));
		COM_ERROR_IF_FAILED(hr, "can't initialize a vertex buffer of the mesh");

		// load index data into the buffer
		hr = pIndexBuffer_->Initialize(pDevice,
			indices.data(),
			static_cast<UINT>(indices.size()));
		COM_ERROR_IF_FAILED(hr, "can't initialize an index buffer of the mesh");
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some element of the class");
	}
}

///////////////////////////////////////////////////////////

Mesh::Mesh(const Mesh & mesh)
{
	this->pDeviceContext_   = mesh.pDeviceContext_;
	*(this->pVertexBuffer_) = *(mesh.pVertexBuffer_);
	*(this->pIndexBuffer_)  = *(mesh.pIndexBuffer_);
}

///////////////////////////////////////////////////////////

Mesh::~Mesh()
{
	_DELETE(pVertexBuffer_);      // release the vertex/index buffers
	_DELETE(pIndexBuffer_);
	this->pDeviceContext_ = nullptr;
}

///////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


void Mesh::Draw(D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// This function prepares the vertex and index buffers for rendering
	// sets up of the input assembler (IA) state

	UINT offset = 0;

	this->pDeviceContext_->IASetVertexBuffers(0, 1, pVertexBuffer_->GetAddressOf(), pVertexBuffer_->GetAddressOfStride(), &offset);
	this->pDeviceContext_->IASetIndexBuffer(pIndexBuffer_->Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->pDeviceContext_->IASetPrimitiveTopology(topologyType);

	return;
}

///////////////////////////////////////////////////////////

bool Mesh::UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & newVerticesArr)
{
	// update the vertex buffer with new vertices data
	if (!this->pVertexBuffer_->UpdateDynamic(pDeviceContext, newVerticesArr.data()))
	{
		Log::Error(THIS_FUNC, "can't update the vertex buffer of the mesh");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

const UINT Mesh::GetVertexCount() const
{
	return pVertexBuffer_->GetBufferSize();
}

const UINT Mesh::GetIndexCount() const
{
	return pIndexBuffer_->GetBufferSize();
}
