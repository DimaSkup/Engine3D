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

using namespace DirectX;
using namespace Mesh;

MeshStorage* MeshStorage::pInstance_ = nullptr;

MeshStorage::MeshStorage()
{
	if (pInstance_ == nullptr)
		pInstance_ = this;
	else
		throw EngineException("You can't create more than only one instance of this class");

	Log::Print("is created");
}

MeshStorage::~MeshStorage()
{
	Log::Print("is destroyed");
}



// **********************************************************************************
//                         Public creation API
// **********************************************************************************

MeshID MeshStorage::CreateMeshWithRawData(
	ID3D11Device* pDevice,
	const Mesh::MeshData& data)
{
	// create a mesh using raw vertices/indices/textures/etc. data;

	bool meshDataIsValid = (!data.vertices.empty()) && (!data.indices.empty()) && (!data.texIDs.empty());
	Assert::True(meshDataIsValid, "the arr of vertices/indices/texturesIDs is empty");
	Assert::True((u32)data.texIDs.size() == TEXTURE_TYPE_COUNT, "the input textures arr must have size == " + std::to_string(TEXTURE_TYPE_COUNT));
	Assert::NotEmpty(data.name.empty(), "the mesh name is empty");
	Assert::NotEmpty(data.path.empty(), "the mesh path is empty");

	MeshID id;   // generated mesh ID

	try
	{
		id = GenerateID();
		
		// create and initialize vertex and index buffers, set textures for this model;
		// and get an index of the created vertex buffer
		const UINT dataIdx = CreateMeshHelper(pDevice, data);

		// relate a mesh with such a name to the data by index
		meshIdToDataIdx_.insert({id, dataIdx});
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		throw EngineException("can't initialize a new model");
	}

	// return a generated ID of this mesh
	return id;
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
		throw EngineException("can't initialize a new mesh; it's name: " + meshName);
	}
}

#endif


// *****************************************************************************
//                        Public getters API
// *****************************************************************************

bool MeshStorage::GetMeshesIDsByNames(
	const std::vector<MeshName>& namesArr,
	std::vector<MeshID>& outMeshesIDs)
{
	// find meshes IDs by input names;
	// 
	// in:  array of meshes names
	// out: array of meshes IDs


	// first of all make a map of pairs: ['mesh_data_idx' => 'mesh_id'] 
	std::map<DataIdx, MeshID> idxToID;

	for (const auto& it : meshIdToDataIdx_)
		idxToID.insert({ it.second, it.first });

	// prepare memory for meshes IDs
	outMeshesIDs.reserve(std::ssize(namesArr));

	// go through each name and find responsible mesh ID
	for (const MeshName& name : namesArr)
	{
		const auto nameIter = std::find(names_.begin(), names_.end(), name);

		if (nameIter != names_.end())
		{
			// compute data idx by name and get an ID by this idx
			const ptrdiff_t idx = std::distance(names_.begin(), nameIter) - 1;
			outMeshesIDs.push_back(idxToID[(UINT)idx]);
		}
		else
		{
			Log::Error("there is no mesh with such name: " + name);
			return false;   // we didn't manage to find an ID
		}
	}

	// we've successfully found meshes IDs by each input name
	return true;
}

///////////////////////////////////////////////////////////

MeshID MeshStorage::GetMeshIDByName(const MeshName& name)
{
	// find and return a mesh ID by input mesh name

	std::vector<MeshID> meshesIDs;
	GetMeshesIDsByNames({ name }, meshesIDs);
	return meshesIDs.front();
}

///////////////////////////////////////////////////////////

void MeshStorage::GetMeshesDataForRendering(
	const std::vector<MeshID>& meshesIDs,
	Mesh::DataForRendering& outData)
{
	// go thought each mesh from the input arr and get its data which will
	// be used for rendering

	const size meshesCount = std::ssize(meshesIDs);
	std::vector<ptrdiff_t> idxs;

	outData.Reserve((u32)meshesCount);
	idxs.reserve(meshesCount);


	try
	{
		// get data idx of each input mesh
		for (const MeshID& meshID : meshesIDs)
			idxs.push_back(meshIdToDataIdx_.at(meshID));

		for (const ptrdiff_t idx : idxs)
			outData.names_.push_back(names_[idx]);

		for (const ptrdiff_t idx : idxs)
			outData.pVBs_.push_back(vertexBuffers_[idx].Get());

		for (const ptrdiff_t idx : idxs)
		{
			// get index buff related data		
			IndexBuffer& ib = indexBuffers_[idx];
			outData.pIBs_.push_back(ib.Get());
			outData.indexCount_.push_back(ib.GetIndexCount());
		}

		// get arr of textures shader resource views for each mesh
		for (const ptrdiff_t idx : idxs)
			outData.texIDs_.push_back(textures_[idx]);

		// get AABB of each mesh
		for (const ptrdiff_t idx : idxs)
			outData.boundBoxes_.push_back(aabb_[idx]);
		
		for (const ptrdiff_t idx : idxs)
			outData.materials_.push_back(materials_[idx]);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("something went out of range");
	}
}


// *****************************************************************************
//                        Public setters API
// *****************************************************************************

void MeshStorage::SetTextureForMeshByID(
	const MeshID meshID,
	const aiTextureType type,
	const TexID texID)
{
	// set new texture ID by type for the mesh by meshID

	try
	{
		const DataIdx meshIdx = meshIdToDataIdx_.at(meshID);
		textures_[meshIdx][type] = texID;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("can't set texture for mesh by ID: " + std::to_string(meshID) + "; texture_type: " + std::to_string(type));
	}
}

///////////////////////////////////////////////////////////

void MeshStorage::SetTexturesForMeshByID(
	const MeshID& meshID,
	const std::unordered_map<aiTextureType, TexID>& texturesToSet)  // pairs: ['texture_type' => 'ptr_to_texture']
{
	// set a batch of textures by types for the mesh by ID

	try
	{
		const DataIdx meshIdx = meshIdToDataIdx_.at(meshID);
		
		// setup ptr to texture objects by each aiTextureType
		for (const auto& it : texturesToSet)
		{
			// it.first  - texture type
			// it.second - texture ID
			textures_[meshIdx][it.first] = it.second;
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("something went out of range for mesh by ID: " + std::to_string(meshID));
	}
}

///////////////////////////////////////////////////////////

void MeshStorage::SetMaterialForMeshByID(
	const MeshID& meshID,
	const Mesh::Material& material) 
{
	try
	{
		const UINT idx = meshIdToDataIdx_.at(meshID);
		materials_[idx] = material;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("something went out of range for mesh by ID: " + std::to_string(meshID));
	}
}


// *********************************************************************************
//                           PRIVATE MODIFICATION API
// *********************************************************************************

#pragma region PrivateModificationAPI

MeshID MeshStorage::GenerateID()
{
	// generate one unique ID for the mesh

	std::vector<MeshID> ids;
	GenerateIDs(1, ids);
	return ids[0];
}

///////////////////////////////////////////////////////////

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

const UINT MeshStorage::CreateMeshHelper(
	ID3D11Device* pDevice,
	const Mesh::MeshData& data)
{
	// THIS FUNCTION helps to create a new model;
	// it creates and initializes vertex and index buffers, setups textures,
	// and does some configuration for rendering of this model

	
	try
	{
		names_.push_back(data.name);

		// create and init vertex and index buffers for new model
		vertexBuffers_.emplace_back(pDevice, data.vertices, false);
		indexBuffers_.emplace_back(pDevice, data.indices);

		textures_.push_back(data.texIDs);
		aabb_.push_back(data.AABB);
		materials_.push_back(data.material);

		// return data index of the last added mesh
		return static_cast<UINT>(vertexBuffers_.size() - 1);
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		throw EngineException("can't initialize a new model");
	}
}