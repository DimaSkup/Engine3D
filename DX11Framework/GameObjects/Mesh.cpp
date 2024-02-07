#include "Mesh.h"

#include "TextureManagerClass.h"

Mesh::Mesh(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT> & indicesArr,
	std::vector<TextureClass> & texturesArr,
	const DirectX::XMMATRIX & transformMatrix,
	const bool isVertexBufferDynamic)
{
	// check input params
	COM_ERROR_IF_NULLPTR(pDevice, "pDevice == nullptr");
	COM_ERROR_IF_NULLPTR(pDeviceContext, "pDeviceContext == nullptr");
	COM_ERROR_IF_ZERO(verticesArr.size(), "there are no vertices");
	COM_ERROR_IF_ZERO(indicesArr.size(), "there are no indices");
	COM_ERROR_IF_ZERO(texturesArr.size(), "there are no textures");

	try
	{
		// go through each texture and pass ownership about it into this mesh
		data_.texturesArr_.resize(texturesArr.size());
		std::move(texturesArr.begin(), texturesArr.end(), data_.texturesArr_.begin());

		// setup the transform matrix of this mesh
		//data_.transformMatrix_ = transformMatrix;

		// load vertex data into the buffer
		data_.vertexBuffer_.Initialize(pDevice,
			pDeviceContext, 
			verticesArr, 
			isVertexBufferDynamic);

		// load index data into the buffer
		data_.indexBuffer_.Initialize(pDevice,
			pDeviceContext, 
			indicesArr);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some element of the class");
	}
}

///////////////////////////////////////////////////////////

Mesh::Mesh(const Mesh & mesh)
{
	// copying constructor

	// copy the input mesh into the current one using an assignment operator
	*this = mesh;
}

///////////////////////////////////////////////////////////

Mesh::~Mesh()
{
}






////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


void Mesh::Draw(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType)
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

void Mesh::Copy(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext, 
	const Mesh & inMesh)
{
	// copies all the data from the input mesh into the current one

	// guard self assignment
	//COM_ERROR_IF_FALSE(this != &inMesh, "you want to copy this mesh into this mesh :)");

	const MeshData & inMeshData = inMesh.data_;

	// check input mesh
	COM_ERROR_IF_ZERO(inMeshData.vertexBuffer_.GetVertexCount(), "the vertex buffer of input mesh is empty");
	COM_ERROR_IF_ZERO(inMeshData.indexBuffer_.GetIndexCount(), "the index buffer of input mesh is empty");

	std::vector<TextureClass> copiedTexturesArr;

	// copy buffers
	data_.vertexBuffer_.CopyBuffer(pDevice, pDeviceContext, inMeshData.vertexBuffer_);
	data_.indexBuffer_.CopyBuffer(pDevice, pDeviceContext, inMeshData.indexBuffer_);

	// copy the transform matrix of the origin mesh
	//this->transformMatrix_ = mesh.GetTransformMatrix();

	// go through each texture of the origin mesh and copy it into the current mesh
	for (const TextureClass & originTexture : inMeshData.texturesArr_)
	{
		// make a new texture and push it into the textures array of this mesh
		copiedTexturesArr.push_back(TextureClass(originTexture));
	}

	// ----------------------------------------------- //

	// move all the copied textures into this mesh
	std::move(copiedTexturesArr.begin(), copiedTexturesArr.end(), data_.texturesArr_.begin());

	return;
}

///////////////////////////////////////////////////////////

void Mesh::SetTextureByIndex(const UINT index, const std::string & texturePath, aiTextureType type)
{
	// set a new texture from the file into the textures array by particular index

	
	// create a new texture from the file or just get a ptr to a texture object by key (its path) if it is already exists 
	TextureClass* pOriginTexture = TextureManagerClass::Get()->GetTexturePtrByKey(texturePath);

	// check if the textures array size is less than the index if so we push this texture
	// at the end of the array;
	if (index >= data_.texturesArr_.size())
		data_.texturesArr_.push_back(TextureClass(*pOriginTexture));
	else
		data_.texturesArr_[index] = TextureClass(*pOriginTexture);   // set texture by index
	
}

///////////////////////////////////////////////////////////

void Mesh::UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext,
	const std::vector<VERTEX> & newVerticesArr)
{
	// update the vertex buffer with new vertices data
	const bool isUpdated = data_.vertexBuffer_.UpdateDynamic(pDeviceContext, newVerticesArr);
	COM_ERROR_IF_FALSE(isUpdated, "can't update the vertex buffer of the mesh");

	return;
}

///////////////////////////////////////////////////////////

const Mesh::MeshData & Mesh::GetMeshData() const
{
	return data_;
}






#if 0
const DirectX::XMMATRIX & Mesh::GetTransformMatrix() const
{
	return transformMatrix_;
}

const std::vector<std::unique_ptr<TextureClass>> & Mesh::GetTexturesArr() const
{
	// this functions returns a reference to the array of texture class objects
	// (an array of this meshes' textures)
	return texturesArr_;
}

ID3D11Buffer* const* Mesh::GetAddressOfVertexBuffer() const
{
	return pVertexBuffer_->GetAddressOf();
}

const UINT* Mesh::GetAddressOfVertexBufferStride() const
{
	return pVertexBuffer_->GetAddressOfStride();
}

ID3D11Buffer* Mesh::GetIndexBuffer() const
{
	return pIndexBuffer_->Get();
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
#endif