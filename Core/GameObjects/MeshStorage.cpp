// *********************************************************************************
// Filename:      MeshStorage.cpp
// Description:   implementation of the MeshStorage functional;
//
// Created:       16.05.24
// *********************************************************************************
#include "MeshStorage.h"
#include "MeshHelperTypes.h"
#include "../Engine/log.h"

#include <stdexcept>
#include <cctype>
#include <random>

using namespace Mesh;

MeshStorage* MeshStorage::pInstance_ = nullptr;

MeshStorage::MeshStorage()
{
	if (pInstance_ == nullptr)
		pInstance_ = this;
	else
		THROW_ERROR("You can't create more than only one instance of this class");

	Log::Print(LOG_MACRO, "is created");
}

MeshStorage::~MeshStorage()
{
	Log::Print(LOG_MACRO, "is destroyed");
}



// **********************************************************************************
//                         Public creation API
// **********************************************************************************

MeshID MeshStorage::CreateMeshWithRawData(
	ID3D11Device* pDevice,
	const MeshName& name,
	const MeshPath& srcDataFilepath,
	const std::vector<VERTEX>& vertices,
	const std::vector<UINT>& indices,
	const std::unordered_map<aiTextureType, TextureClass*>& textures)
{
	// create a mesh using raw vertices/indices/textures/etc. data;
	// input:
	// 1. name of the mesh
	// 2. filepath to the file from where was the mesh loaded (if the mesh was dynamically generated this path is empty)
	// 3. vertices and indices arrays
	// 4. textures map with pairs ['texture_type' => 'ptr_to_texture']

	ASSERT_NOT_EMPTY(name.empty(), "the input mesh name is empty");
	ASSERT_NOT_ZERO(vertices.size(), "the input vertices array is empty");
	ASSERT_NOT_ZERO(indices.size(), "the input indices array is empty");
	ASSERT_NOT_EMPTY(textures.empty(), "the input textures array is empty");

	std::vector<MeshID> generatedIDs;

	try
	{
		GenerateIDs(1, generatedIDs);

		// create and initialize vertex and index buffers, set textures for this model;
		// and get an index of the created vertex buffer
		const UINT dataIdx = CreateMeshHelper(
			pDevice,
			name,
			srcDataFilepath,
			vertices,                                          // raw vertices data
			indices,                                           // raw indices data
			textures);                                            // map of pairs: ['texture_type' => 'ptr_to_texture']

		// relate a mesh with such a name to the data by index
		meshIdToDataIdx_.insert({ generatedIDs[0], dataIdx});
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a new model");
	}

	// return a generated ID for this mesh
	return generatedIDs[0];
}

///////////////////////////////////////////////////////////
#if 0
const std::string MeshStorage::CopyMeshFromBuffers(
	ID3D11Device* pDevice,
	const std::string & meshName,
	VertexBuffer<VERTEX>& vertexBuffer,
	IndexBuffer& indexBuffer,
	const std::unordered_map<aiTextureType, TextureClass*>& textures)
{
	// create a new mesh using some vertex/index buffer, and textures map;
	// input: 
	// 1. vertex and index buffer from which we will copy geometry data;
	// 2. textures map from which we will copy textures data
	//          
	// return: ID of the created mesh

	// check input data
	assert(meshName.empty() == false);
	assert(vertexBuffer.GetVertexCount() > 0);
	assert(indexBuffer.GetIndexCount() > 0);
	assert(textures.empty() == false);

	// an index of a new model
	uint32_t model_idx = 0;

	try
	{
		// add new vertex/index buffer since this model is absolutely new
		vertexBuffers_.push_back(vertexBuffer);
		indexBuffers_.push_back(indexBuffer);

		// set related textures to the last added VB
		textures_.push_back(textures);

		const UINT dataIdx = static_cast<UINT>(vertexBuffers_.size() - 1);

		// relate a mesh with such a name to the data by index
		meshIdToDataIdx_.insert({ meshName, dataIdx });

		return meshName;
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a new mesh; it's name: " + meshName);
	}
}

#endif


// *****************************************************************************
//                        Public getters API
// *****************************************************************************

void MeshStorage::GetMeshesDataForRendering(
	const std::vector<MeshID>& meshesIDs,
	std::vector<Mesh::DataForRendering>& outData)
{
	// go thought each mesh from the input arr and get its data which will
	// be used for rendering

	const ptrdiff_t meshesCount = std::ssize(meshesIDs);
	outData.reserve(meshesCount);

	try
	{
		for (const MeshID& meshID : meshesIDs)
		{
			DataForRendering data;

			const UINT idx = meshIdToDataIdx_.at(meshID);
			VertexBuffer<VERTEX>& VB = vertexBuffers_[idx];
			IndexBuffer& IB = indexBuffers_[idx];

			VB.GetAddressOfBufferAndStride(data.ppVertexBuffer, data.pStride);
			IB.GetBufferAndIndexCount(data.pIndexBuffer, data.indexCount);
			data.dataIdx = idx;
			data.material = materials_[idx];
			data.textures = textures_[idx];

			data.name = names_[idx];
			data.path = srcDataFilepaths_[idx];

			// store data into the input array
			outData.emplace_back(data);
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range");
	}
}

// *****************************************************************************
//                        Public setters API
// *****************************************************************************

void MeshStorage::SetTextureForMeshByID(
	const MeshID& meshID,
	const aiTextureType type,
	TextureClass* pTexture)
{
	// set a single texture by type for the mesh by ID
	try
	{
		const UINT idx = meshIdToDataIdx_.at(meshID);
		textures_.at(idx).insert_or_assign(type, pTexture);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't set texture for mesh by ID: " + std::to_string(meshID) + "; texture_type: " + std::to_string(type));
	}
}

///////////////////////////////////////////////////////////

void MeshStorage::SetTexturesForMeshByID(
	const MeshID& meshID,
	const std::unordered_map<aiTextureType, TextureClass*>& textures)  // pairs: ['texture_type' => 'ptr_to_texture']
{
	// set a batch of textures by types for the mesh by ID
	try
	{
		const UINT idx = meshIdToDataIdx_.at(meshID);
		std::unordered_map<aiTextureType, TextureClass*>& texturesMap = textures_.at(idx);

		// set a ptr to texture by its type
		for (const auto texture : textures)
			texturesMap.insert_or_assign(texture.first, texture.second);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range for mesh by ID: " + std::to_string(meshID));
	}
}

///////////////////////////////////////////////////////////

void MeshStorage::SetMaterialForMeshByID(
	const MeshID& meshID,
	const Material& material) 
{
	try
	{
		const UINT idx = meshIdToDataIdx_.at(meshID);
		materials_.at(idx) = material;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range for mesh by ID: " + std::to_string(meshID));
	}
}


// *********************************************************************************
//                           PRIVATE MODIFICATION API
// *********************************************************************************

#pragma region PrivateModificationAPI

void MeshStorage::GenerateIDs(
	const size_t newMeshesCount,
	std::vector<MeshID>& outGeneratedIDs)
{
	// generate unique IDs in quantity newMeshesCount
	// 
	// in:  how many meshes we will create
	// out: SORTED array of generated entities IDs

	using u32 = uint_least32_t;
	using engine = std::mt19937;

	std::random_device os_seed;
	const u32 seed = os_seed();
	engine generator(seed);
	std::uniform_int_distribution<u32> distribute(0, UINT_MAX);

	outGeneratedIDs.reserve(newMeshesCount);

	// generate an ID for each new mesh
	for (size_t idx = 0; idx < newMeshesCount; ++idx)
	{
		u32 id = distribute(generator);

		// if such an ID already exists we generate new id value
		while (meshIdToDataIdx_.find(id) != meshIdToDataIdx_.end())
		{
			id = distribute(generator);
		}

		outGeneratedIDs.push_back(id);
	}

	// after generation we sort IDs so then it will be faster to store them
	std::sort(outGeneratedIDs.begin(), outGeneratedIDs.end());
}

///////////////////////////////////////////////////////////

const UINT MeshStorage::CreateMeshHelper(ID3D11Device* pDevice,
	const MeshName& name,
	const MeshPath& srcDataFilepath,
	const std::vector<VERTEX>& verticesArr,
	const std::vector<UINT>& indicesArr,
	const std::unordered_map<aiTextureType, TextureClass*>& textures)
{
	// THIS FUNCTION helps to create a new model;
	// it creates and initializes vertex and index buffers, setups textures,
	// and does some configuration for rendering of this model

	// check input params
	ASSERT_NOT_ZERO(verticesArr.size(), "the input vertices array is empty");
	ASSERT_NOT_ZERO(indicesArr.size(), "the input indices array is empty");
	ASSERT_TRUE(!textures.empty(), "the input textures array is empty");

	try
	{
		names_.push_back(name);
		srcDataFilepaths_.push_back(srcDataFilepath);

		// create and init vertex and index buffers for new model
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		textures_.push_back(textures);      // set related textures to the last added VB
		materials_.push_back(Material());   // default material

		// return data index of the last added mesh
		return static_cast<UINT>(vertexBuffers_.size() - 1);
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a new model");
	}
}