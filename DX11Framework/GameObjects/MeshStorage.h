// *********************************************************************************
// Filename:      MeshStorage.h
// Description:   a storage for meshes:
//                vertex and index buffers; 
//                also contains different params for these meshes 
//                (for instance: rendering params or textures for mesh)
//
// Created:       16.05.24
// *********************************************************************************
#pragma once

#include <vector>
#include <map>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "textureclass.h"
#include "../Light/LightHelper.h"
#include "MeshHelperTypes.h"


class MeshStorage
{
public:
	

public:
	MeshStorage();
	MeshStorage(const MeshStorage&) = delete;
	MeshStorage(MeshStorage&&) = delete;
	MeshStorage& operator=(const MeshStorage&) = delete;
	MeshStorage& operator=(MeshStorage&&) = delete;
	~MeshStorage();

	inline static MeshStorage* Get() 
	{ 
		assert((pInstance_ != nullptr) && "you have to create an instance of the MeshStorage");
		return pInstance_;
	}


	// *****************************************************************************
	//                         Public creation API
	// *****************************************************************************
		
	// create a mesh using raw vertices/indices/textures data
	const std::string CreateMeshWithRawData(
		ID3D11Device* pDevice,
		const std::string & meshName,
		const std::vector<VERTEX>& verticesArr,
		const std::vector<UINT>& indicesArr,
		const std::map<aiTextureType, TextureClass*>& textures);

	// create a new mesh using some vertex/index buffer
	const std::string CreateMeshWithBuffers(
		ID3D11Device* pDevice,
		const std::string& meshName,
		VertexBuffer<VERTEX>& vertexBuffer,
		IndexBuffer& indexBuffer,
		const std::map<aiTextureType, TextureClass*>& textures);


	// *****************************************************************************
	//                         Public copying API
	// *****************************************************************************

	const std::vector<UINT> CreateCopiesOfMeshByIndex(
		const UINT indexOfOrigin, 
		const UINT numOfCopies);


	// *****************************************************************************
	//                        Public getters API
	// *****************************************************************************

	Mesh::MeshDataForRendering GetMeshDataForRendering(const std::string& meshID);

	// *****************************************************************************
	//                        Public setters API
	// *****************************************************************************

	void SetRenderingShaderForMeshByID(
		const std::string& meshID,
		const Mesh::RENDERING_SHADERS shaderType);

	void SetPrimitiveTopologyForMeshByID(
		const std::string& meshID,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType);

	void SetTexturesForMeshByID(
		const std::string& meshID,
		const std::map<aiTextureType, TextureClass*>& textures);  // pairs: ['texture_type' => 'ptr_to_texture']

	void SetMaterialForMeshByID(
		const std::string& meshID,
		const Material& material);

private:
	const UINT CreateMeshHelper(
		ID3D11Device* pDevice,
		const std::vector<VERTEX>& verticesArr,
		const std::vector<UINT>& indicesArr,
		const std::map<aiTextureType, TextureClass*>& textures);

public:
	static MeshStorage* pInstance_;      

	using MeshID = std::string;
	using DataIdx = UINT;

	std::map<MeshID, DataIdx>             meshIdToDataIdx_;
	std::vector<VertexBuffer<VERTEX>>     vertexBuffers_;
	std::vector<IndexBuffer>              indexBuffers_;
	std::vector<Mesh::RENDERING_SHADERS>        useShaderForRendering_;  // [index: vertex_buff_idx => value: EntityStore::RENDERING_SHADERS] (what kind of rendering shader will we use for this vertex buffer)
	std::vector<D3D11_PRIMITIVE_TOPOLOGY> usePrimTopology_;        // [index: vertex_buff_idx => value: primitive_topology] (what kind of primitive topology will we use for this vertex buffer)
	std::vector<std::map<aiTextureType, TextureClass*>> textures_; // textures set for each vertex buffer
	std::vector<Material>                 materials_;
};
