#include "Mesh.h"




Mesh::Mesh(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & vertices,
	const std::vector<UINT> & indices)
{
	// check input params
	COM_ERROR_IF_NULLPTR(pDevice, "pDevice == nullptr");
	COM_ERROR_IF_NULLPTR(pDeviceContext, "pDeviceContext == nullptr");
	COM_ERROR_IF_ZERO(vertices.size(), "there are no vertices");
	COM_ERROR_IF_ZERO(indices.size(), "there are no indices");

	try
	{
		this->pDevice_ = pDevice;
		this->pDeviceContext_ = pDeviceContext;

		// allocate memory for the buffers
		this->pVertexBuffer_ = std::make_unique<VertexBuffer>(pDevice, pDeviceContext);
		this->pIndexBuffer_ = std::make_unique<IndexBuffer>(pDevice, pDeviceContext);


		// initialize the buffers
		// load vertex data into the buffer
		HRESULT hr = pVertexBuffer_->Initialize(vertices);
		COM_ERROR_IF_FAILED(hr, "can't initialize a vertex buffer of the mesh");

		// load index data into the buffer
		hr = pIndexBuffer_->Initialize(indices);
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
	// copying constructor

	// check if we allocated memory for the current mesh
	COM_ERROR_IF_FALSE(this, "memory for the obj isn't allocated: this == nullptr");

	// copy the input mesh into the current one
	*this = mesh;
}

///////////////////////////////////////////////////////////

Mesh & Mesh::operator=(const Mesh & mesh)
{
	// copying operator

	// guard self assignment
	if (this == &mesh)
		return *this;

	// check input mesh
	COM_ERROR_IF_FALSE(mesh.pDevice_, "pDevice == nullptr");
	COM_ERROR_IF_FALSE(mesh.pDeviceContext_, "pDeviceContext == nullptr");
	COM_ERROR_IF_FALSE(mesh.pVertexBuffer_, "vertex buffer == nullptr");
	COM_ERROR_IF_FALSE(mesh.pIndexBuffer_, "index buffer == nullptr");

	this->pDevice_ = mesh.pDevice_;
	this->pDeviceContext_ = mesh.pDeviceContext_;

	// allocate memory for the buffers
	this->pVertexBuffer_ = std::make_unique<VertexBuffer>(pDevice_, pDeviceContext_);
	this->pIndexBuffer_ = std::make_unique<IndexBuffer>(pDevice_, pDeviceContext_);

	// copy buffers
	this->pVertexBuffer_->CopyBuffer(*mesh.pVertexBuffer_);
	this->pIndexBuffer_->CopyBuffer(*mesh.pIndexBuffer_);

	return *this;
}

///////////////////////////////////////////////////////////

Mesh::~Mesh()
{
	this->pDeviceContext_ = nullptr;
}






////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


void Mesh::Draw(D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// This function prepares the vertex and index buffers for rendering;
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
	if (!this->pVertexBuffer_->UpdateDynamic(newVerticesArr))
	{
		Log::Error(THIS_FUNC, "can't update the vertex buffer of the mesh");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

const UINT Mesh::GetVertexCount() const
{
	return pVertexBuffer_->GetVertexCount();
}

///////////////////////////////////////////////////////////

const UINT Mesh::GetIndexCount() const
{
	return pIndexBuffer_->GetIndexCount();
}
