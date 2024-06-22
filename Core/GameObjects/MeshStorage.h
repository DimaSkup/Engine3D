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
#include "MeshHelperTypes.h"
#include "../Light/LightHelper.h"  // for using Material type


class MeshStorage
{
public:
	
	using DataIdx = UINT;

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

	// create a mesh using raw vertices/indices/textures/etc. data
	MeshID CreateMeshWithRawData(
		ID3D11Device* pDevice,
		const MeshName & name,
		const MeshPath& srcDataFilepath,
		const std::vector<VERTEX>& vertices,
		const std::vector<UINT>& indices,
		const std::unordered_map<aiTextureType, TextureClass*>& textures);


	// *****************************************************************************
	//                         Public copying API
	// *****************************************************************************

	// create a new mesh using some another vertex/index buffer
	const std::string CopyMeshFromBuffers(
		ID3D11Device* pDevice,
		const MeshName& name,
		VertexBuffer<VERTEX>& vertexBuffer,
		IndexBuffer& indexBuffer,
		const std::unordered_map<aiTextureType, TextureClass*>& textures);


	const std::vector<UINT> CreateCopiesOfMeshByIndex(
		const UINT indexOfOrigin, 
		const UINT numOfCopies);


	// *****************************************************************************
	//                        Public getters API
	// *****************************************************************************

	MeshID GetMeshIDByName(const MeshName& name)
	{
		ASSERT_NOT_EMPTY(name.empty(), "input name is empty");

		const auto nameIter = std::find(names_.begin(), names_.end(), name);

		if (nameIter != names_.end())
		{
			const ptrdiff_t idx = std::distance(names_.begin(), nameIter) - 1;

			for (const auto& it : meshIdToDataIdx_)
			{
				if (it.second == idx)
					return it.first;
			}
		}
		else
		{
			THROW_ERROR("there is no mesh with such name: " + name);
		}
	}

	void GetMeshesDataForRendering(
		const std::vector<MeshID>& meshesIDs, 
		std::vector<Mesh::DataForRendering>& outData);

	// *****************************************************************************
	//                        Public setters API
	// *****************************************************************************

	void SetTextureForMeshByID(
		const MeshID& meshID,
		const aiTextureType type,
		TextureClass* pTexture);

	void SetTexturesForMeshByID(
		const MeshID& meshID,
		const std::unordered_map<aiTextureType, TextureClass*>& textures);  // pairs: ['texture_type' => 'ptr_to_texture']

	void SetMaterialForMeshByID(
		const MeshID& meshID,
		const Material& material);

private:
	void GenerateIDs(
		const size_t newMeshesCount,
		std::vector<MeshID>& outGeneratedIDs);

	const UINT CreateMeshHelper(
		ID3D11Device* pDevice,
		const MeshName& name,
		const MeshPath& srcDataFilepath,
		const std::vector<VERTEX>& verticesArr,
		const std::vector<UINT>& indicesArr,
		const std::unordered_map<aiTextureType, TextureClass*>& textures);

public:
	static MeshStorage* pInstance_;      

	std::map<MeshID, DataIdx>             meshIdToDataIdx_;

	std::vector<MeshPath>                 srcDataFilepaths_;                 // from where was the mesh loaded (or where to store the mesh if it was dynamically generated)
	std::vector<MeshName>                 names_;                            // name of the mesh
	std::vector<VertexBuffer<VERTEX>>     vertexBuffers_;
	std::vector<IndexBuffer>              indexBuffers_;	
	std::vector<std::unordered_map<aiTextureType, TextureClass*>> textures_; // textures set for each vertex buffer
	std::vector<Material>                 materials_;
};
