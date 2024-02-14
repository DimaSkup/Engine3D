#include "MeshObject.h"




#if 0
MeshObject::MeshObject()
{
}

MeshObject::MeshObject(ID3D11Device* pDevice,
	const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT> & indicesArr,
	std::vector<TextureClass> & texturesArr,
	const DirectX::XMMATRIX & transformMatrix,
	const bool isVertexBufferDynamic)
{
	this->Initialize(pDevice,
		verticesArr, 
		indicesArr,
		texturesArr,
		transformMatrix, 
		isVertexBufferDynamic);
}

///////////////////////////////////////////////////////////

MeshObject::MeshObject(const MeshObject & MeshObject)
{
	// copying constructor

	// copy the input MeshObject into the current one using an assignment operator
	//data_.vertexBuffer_. = MeshObject.data_;
}

///////////////////////////////////////////////////////////

MeshObject::~MeshObject()
{
}






////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

void MeshObject::Initialize(ID3D11Device* pDevice,
	const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT> & indicesArr,
	std::vector<TextureClass> & texturesArr,
	const DirectX::XMMATRIX & transformMatrix,
	const bool isVertexBufferDynamic)
{
	// check input params
	COM_ERROR_IF_NULLPTR(pDevice, "pDevice == nullptr");
	COM_ERROR_IF_ZERO(verticesArr.size(), "there are no vertices");
	COM_ERROR_IF_ZERO(indicesArr.size(), "there are no indices");
	COM_ERROR_IF_ZERO(texturesArr.size(), "there are no textures");

	try
	{
		// go through each texture and pass ownership about it into this MeshObject
		data_.texturesArr_.resize(texturesArr.size());
		std::move(texturesArr.begin(), texturesArr.end(), data_.texturesArr_.begin());

		// setup the transform matrix of this MeshObject
		//data_.transformMatrix_ = transformMatrix;

		// load vertex data into the buffer
		data_.vertexBuffer_.Initialize(pDevice, verticesArr, isVertexBufferDynamic);

		// load index data into the buffer
		data_.indexBuffer_.Initialize(pDevice, indicesArr);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some element of the class");
	}

	return;
}


void MeshObject::Draw(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// This function prepares the vertex and index buffers for rendering;
	// sets up of the input assembler (IA) state

	UINT offset = 0;
	
	pDeviceContext->IASetVertexBuffers(0, 1, data_.vertexBuffer_.GetAddressOf(), data_.vertexBuffer_.GetAddressOfStride(), &offset);
	pDeviceContext->IASetIndexBuffer(data_.indexBuffer_.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(topologyType);

	return;
}

///////////////////////////////////////////////////////////

void MeshObject::Copy(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext, 
	const MeshObject & inMesh)
{
	// copies all the data from the input MeshObject into the current one

	// guard self assignment
	//COM_ERROR_IF_FALSE(this != &inMesh, "you want to copy this MeshObject into this MeshObject :)");

	const MeshData & inMeshData = inMesh.data_;

	// check input MeshObject
	COM_ERROR_IF_ZERO(inMeshData.vertexBuffer_.GetVertexCount(), "the vertex buffer of input MeshObject is empty");
	COM_ERROR_IF_ZERO(inMeshData.indexBuffer_.GetIndexCount(), "the index buffer of input MeshObject is empty");

	std::vector<TextureClass> copiedTexturesArr;

	// copy buffers
	data_.vertexBuffer_.CopyBuffer(pDevice, pDeviceContext, inMeshData.vertexBuffer_);
	data_.indexBuffer_.CopyBuffer(pDevice, pDeviceContext, inMeshData.indexBuffer_);

	// copy the transform matrix of the origin MeshObject
	//this->transformMatrix_ = MeshObject.GetTransformMatrix();

	// go through each texture of the origin MeshObject and copy it into the current MeshObject
	for (const TextureClass & originTexture : inMeshData.texturesArr_)
	{
		// make a new texture and push it into the textures array of this MeshObject
		copiedTexturesArr.push_back(TextureClass(originTexture));
	}

	// ----------------------------------------------- //

	// move all the copied textures into this MeshObject
	std::move(copiedTexturesArr.begin(), copiedTexturesArr.end(), data_.texturesArr_.begin());

	return;
}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

void MeshObject::UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & newVerticesArr)
{
	// update the vertex buffer with new vertices data
	
	try
	{
		data_.vertexBuffer_.UpdateDynamic(pDeviceContext, newVerticesArr);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't update the vertex buffer of the MeshObject");
	}

	return;
}

///////////////////////////////////////////////////////////

const MeshObject::MeshData & MeshObject::GetMeshData()
{
	return data_;
}


const DirectX::XMMATRIX & MeshObject::GetTransformMatrix() const
{
	return transformMatrix_;
}

const std::vector<std::unique_ptr<TextureClass>> & MeshObject::GetTexturesArr() const
{
	// this functions returns a reference to the array of texture class objects
	// (an array of this meshes' textures)
	return texturesArr_;
}

ID3D11Buffer* const* MeshObject::GetAddressOfVertexBuffer() const
{
	return pVertexBuffer_->GetAddressOf();
}

const UINT* MeshObject::GetAddressOfVertexBufferStride() const
{
	return pVertexBuffer_->GetAddressOfStride();
}

ID3D11Buffer* MeshObject::GetIndexBuffer() const
{
	return pIndexBuffer_->Get();
}

///////////////////////////////////////////////////////////

const UINT MeshObject::GetVertexCount() const
{
	return pVertexBuffer_->GetVertexCount();
}

///////////////////////////////////////////////////////////

const UINT MeshObject::GetIndexCount() const
{
	return pIndexBuffer_->GetIndexCount();
}
#endif