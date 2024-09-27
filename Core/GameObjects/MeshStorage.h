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
#include <assimp/material.h>       // for using aiTextureType

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MeshHelperTypes.h"

#include "../Common/Types.h"


class MeshStorage
{
public:
	
	using DataIdx = u32;

public:
	MeshStorage();
	MeshStorage(const MeshStorage&) = delete;
	MeshStorage(MeshStorage&&) = delete;
	MeshStorage& operator=(const MeshStorage&) = delete;
	MeshStorage& operator=(MeshStorage&&) = delete;
	~MeshStorage();

	inline static MeshStorage* Get() 
	{ 
		//assert((pInstance_ != nullptr) && "you have to create an instance of the MeshStorage");
		return pInstance_;
	}


	// *****************************************************************************
	//                         Public creation API
	// *****************************************************************************

	// create a mesh using raw vertices/indices/textures/etc. data
	MeshID CreateMeshWithRawData(ID3D11Device* pDevice,	const Mesh::MeshData& data);


	// *****************************************************************************
	//                         Public copying API
	// *****************************************************************************

	// create a new mesh using some another vertex/index buffer
	const std::string CopyMeshFromBuffers(
		ID3D11Device* pDevice,
		const MeshName& name,
		VertexBuffer<VERTEX>& vertexBuffer,
		IndexBuffer& indexBuffer,
		const std::unordered_map<aiTextureType, TexID>& textures);


	const std::vector<UINT> CreateCopiesOfMeshByIndex(
		const UINT indexOfOrigin, 
		const UINT numOfCopies);


	// *****************************************************************************
	//                        Public getters API
	// *****************************************************************************

	inline const std::vector<MeshName>& GetAllMeshesNames() const { return names_; }

	bool GetMeshesIDsByNames(
		const std::vector<MeshName>& namesArr,
		std::vector<MeshID>& outMeshesIDs);

	MeshID GetMeshIDByName(const MeshName& name);

	void GetBoundingDataByID(const MeshID id, DirectX::BoundingBox& outAABB);

	void GetBoundingDataByIDs(
		const std::vector<MeshID>& ids,
		std::vector<DirectX::BoundingBox>& outBoundingData);

	void GetCommonBoundingBoxByIDs(const std::vector<MeshID>& ids, DirectX::BoundingBox& outAABB);

	void GetMeshesDataForRendering(
		const std::vector<MeshID>& meshesIDs, 
		Mesh::DataForRendering& outData);

	// *****************************************************************************
	//                        Public setters API
	// *****************************************************************************

	void SetTextureForMeshByID(
		const MeshID meshID,
		const aiTextureType type,
		const TexID pTexture);

	void SetTexturesForMeshByID(
		const MeshID& meshID,
		const std::unordered_map<aiTextureType, TexID>& textures);  // pairs: ['texture_type' => 'ptr_to_texture']

	void SetMaterialForMeshByID(
		const MeshID& meshID,
		const Mesh::Material& material);

private:
	MeshID GenerateID();

	void GenerateIDs(
		const size_t newMeshesCount,
		std::vector<MeshID>& outGeneratedIDs);

	const UINT CreateMeshHelper(ID3D11Device* pDevice, const Mesh::MeshData& data);

	bool CheckIDsExist(const std::vector<MeshID>& ids);

public:

	static MeshStorage* pInstance_;      

	std::map<MeshID, DataIdx>         meshIdToDataIdx_;

	//std::vector<MeshPath>             srcDataFilepaths_;   // from where was the mesh loaded (or where to store the mesh if it was dynamically generated)
	std::vector<MeshName>             names_;             // name of the mesh
	std::vector<VertexBuffer<VERTEX>> vertexBuffers_;
	std::vector<IndexBuffer>          indexBuffers_;	
	std::vector<TexIDsArr>            textures_;          // each mesh has its ows set of textures
	std::vector<DirectX::BoundingBox> aabb_;
	std::vector<Mesh::Material>       materials_;
};
