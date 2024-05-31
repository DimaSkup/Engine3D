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

using namespace Mesh;

MeshStorage* MeshStorage::pInstance_ = nullptr;

MeshStorage::MeshStorage()
{
	if (pInstance_ == nullptr)
	{
		pInstance_ = this;
	}
	else
	{
		THROW_ERROR("You can't create more than only one instance of this class");
	}

	Log::Print(LOG_MACRO, "is created");
}

MeshStorage::~MeshStorage()
{
	Log::Print(LOG_MACRO, "is destroyed");
}


// **********************************************************************************
//                         Public creation API
// **********************************************************************************

const std::string MeshStorage::CreateMeshWithRawData(
	ID3D11Device* pDevice,
	const std::string & meshName,
	const std::vector<VERTEX>& verticesArr,
	const std::vector<UINT>& indicesArr,
	const std::map<aiTextureType, TextureClass*>& textures)
{
	// create a mesh using raw vertices/indices/textures data;
	// input:
	// 1. vertices and indices arrays
	// 2. textures map with pairs ['texture_type' => 'ptr_to_texture']
	//
	// return: ID of the created mesh

	ASSERT_NOT_EMPTY(meshName.empty(), "the input mesh name is empty");
	ASSERT_NOT_ZERO(verticesArr.size(), "the input vertices array is empty");
	ASSERT_NOT_ZERO(indicesArr.size(), "the input indices array is empty");
	ASSERT_NOT_EMPTY(textures.empty(), "the input textures array is empty");

	try
	{
		// create and initialize vertex and index buffers, set textures for this model;
		// and get an index of the created vertex buffer
		const UINT dataIdx = CreateMeshHelper(
			pDevice,
			verticesArr,                                          // raw vertices data
			indicesArr,                                           // raw indices data
			textures);                                            // map of pairs: ['texture_type' => 'ptr_to_texture']

		// relate a mesh with such a name to the data by index
		meshIdToDataIdx_.insert({ meshName, dataIdx });
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a new model");
	}

	return meshName;
}

///////////////////////////////////////////////////////////

const std::string MeshStorage::CreateMeshWithBuffers(
	ID3D11Device* pDevice,
	const std::string & meshName,
	VertexBuffer<VERTEX>& vertexBuffer,
	IndexBuffer& indexBuffer,
	const std::map<aiTextureType, TextureClass*>& textures)
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

		// set default rendering shader / primitive topology / etc. for this new VB
		useShaderForRendering_.push_back(RENDERING_SHADERS::COLOR_SHADER);
		usePrimTopology_.push_back(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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




// *****************************************************************************
//                        Public getters API
// *****************************************************************************

MeshDataForRendering MeshStorage::GetMeshDataForRendering(const std::string& meshID)
{
	try
	{
		const UINT dataIdx = meshIdToDataIdx_.at(meshID);
		MeshDataForRendering data;
		VertexBuffer<VERTEX>& VB = vertexBuffers_.at(dataIdx);
		IndexBuffer& IB = indexBuffers_.at(dataIdx);

		VB.GetAddressOfBufferAndStride(data.ppVertexBuffer, data.pStride);
		IB.GetBufferAndIndexCount(data.pIndexBuffer, data.indexCount);
		data.dataIdx = dataIdx;
		data.renderingShaderType = useShaderForRendering_.at(dataIdx);
		data.topologyType = usePrimTopology_.at(dataIdx);
		data.material = materials_.at(dataIdx);

		return data;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range for mesh by ID: " + meshID);
	}
}



// *****************************************************************************
//                        Public setters API
// *****************************************************************************

void MeshStorage::SetRenderingShaderForMeshByID(
	const std::string& meshID,
	const RENDERING_SHADERS shaderType)
{
	try
	{
		const UINT dataIdx = meshIdToDataIdx_.at(meshID);
		useShaderForRendering_.at(dataIdx) = shaderType;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range for mesh by ID: " + meshID);
	}
}

///////////////////////////////////////////////////////////

void MeshStorage::SetPrimitiveTopologyForMeshByID(
	const std::string& meshID,
	const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

void MeshStorage::SetTexturesForMeshByID(
	const std::string& meshID,
	const std::map<aiTextureType, TextureClass*>& textures)  // pairs: ['texture_type' => 'ptr_to_texture']
{
	try
	{
		const UINT dataIdx = meshIdToDataIdx_.at(meshID);
		std::map<aiTextureType, TextureClass*>& texturesMap = textures_.at(dataIdx);

		// set a ptr to texture by its type
		for (const auto texture : textures)
			texturesMap.insert_or_assign(texture.first, texture.second);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range for mesh by ID: " + meshID);
	}
}

///////////////////////////////////////////////////////////

void MeshStorage::SetMaterialForMeshByID(
	const std::string& meshID,
	const Material& material) 
{
	try
	{
		const UINT dataIdx = meshIdToDataIdx_.at(meshID);
		materials_.at(dataIdx) = material;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range for mesh by ID: " + meshID);
	}
}

// *********************************************************************************
//                           PRIVATE MODIFICATION API
// *********************************************************************************

#pragma region PrivateModificationAPI

const UINT MeshStorage::CreateMeshHelper(ID3D11Device* pDevice,
	const std::vector<VERTEX>& verticesArr,
	const std::vector<UINT>& indicesArr,
	const std::map<aiTextureType, TextureClass*>& textures)
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
		// create and init vertex and index buffers for new model
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		// set default rendering shader / primitive topology / etc. for this new VB
		useShaderForRendering_.push_back(RENDERING_SHADERS::COLOR_SHADER);
		usePrimTopology_.push_back(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		textures_.push_back(textures);      // set related textures to the last added VB
		materials_.push_back(Material());   // default material

		// return index of the last added vertex buffer (VB)
		return static_cast<UINT>(vertexBuffers_.size() - 1);
	}

	catch (EngineException& e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a new model");
	}
}