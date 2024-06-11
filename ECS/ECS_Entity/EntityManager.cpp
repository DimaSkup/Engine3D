#include "EntityManager.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Log.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <sstream>
#include <unordered_map>

#include <cctype>
#include <random>

using namespace DirectX;
using namespace ECS;


EntityManager::EntityManager() :
	transformSystem_{ &transform_, &world_ },
	moveSystem_{ &transform_, &world_, &movement_ },
	meshSystem_{ &meshComponent_ },
	renderSystem_{ &renderComponent_, &transform_, &world_, &meshComponent_ }
{
	// make pairs ['component_type' => 'component_name']
	componentTypeToName_ =
	{
		{ ComponentType::TransformComponent, "Transform" },
		{ ComponentType::MoveComponent, "Movement" },
		{ ComponentType::MeshComp, "MeshComponent" },
		{ ComponentType::RenderedComponent, "Rendered" },
	};
}

EntityManager::~EntityManager()
{
	//Log::Debug(LOG_MACRO);
}

void EntityManager::Serialize()
{
#if 0
	const std::string dataFilepath = "transform.bin";
	std::ofstream fout(dataFilepath, std::ios::binary);
	if (!fout.is_open())
	{
		Log::Error(ECS::Log_MACRO, "can't open file for writing: " + dataFilepath);
		exit(-1);
	}

	std::stringstream ss;

	// SERIALIZE POSITIONS
	const std::vector<DirectX::XMFLOAT3>& positions = modelsTransformData.at("sphere").positions;
	const DirectX::XMFLOAT3* ptrPosRawData = positions.data();
	fout.write((const char*)(ptrPosRawData), positions.size() * sizeof(DirectX::XMFLOAT3));

	fout.close();

#endif
}

///////////////////////////////////////////////////////////

void EntityManager::Deserialize()
{
#if 0
	// DESERIALIZE POSITIONS
	std::ifstream fin(dataFilepath, std::ios::binary);
	if (!fin.is_open())
	{
		ECS::Log::Error(ECS::Log_MACRO, "can't open file for reading: " + dataFilepath);
		exit(-1);
	}

	std::vector<DirectX::XMFLOAT3> deserializedPositions(positions.size());

	fin.read((char*)(deserializedPositions.data()), positions.size() * sizeof(DirectX::XMFLOAT3));
	fin.close();
#endif
}



// ************************************************************************************
//                     PUBLIC CREATION/DESTROYMENT API
// ************************************************************************************

#pragma region PublicCreationDestroymentAPI

void EntityManager::CreateEntities(const size_t newEnttsCount)
{
	// create batch of new empty entities, generate for each entity 
	// unique ID and set that it hasn't any component by default

	ASSERT_NOT_ZERO(newEnttsCount, "new entitites count == 0");

	Utils::AppendArray(ids_, GenerateIDs(newEnttsCount));

	// set that each new entity by default doesn't have any component
	componentFlags_.insert(componentFlags_.end(), newEnttsCount, 0);
}

///////////////////////////////////////////////////////////


void EntityManager::DestroyEntities(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}


#pragma endregion

// ************************************************************************************
//                          PUBLIC UPDATING FUNCTIONS
// ************************************************************************************

void EntityManager::Update(const float deltaTime)
{
	moveSystem_.UpdateAllMoves(deltaTime);
}


// ************************************************************************************
//                          PUBLIC RENDERING FUNCTIONS
// ************************************************************************************

void EntityManager::GetRenderingDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<XMMATRIX>& outWorldMatrices,
	std::vector<RENDERING_SHADERS>& outShaderTypes)
{
	// get data which will be used for rendering of the entities;
	// in:   array of entities IDs
	// out:  array of world matrix of each entity from the input arr

	try
	{
		renderSystem_.GetRenderingDataOfEntts(
			enttsIDs,
			outWorldMatrices,
			outShaderTypes);
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, false);
		THROW_ERROR("can't get rendering data using the RenderSystem (ECS)");
	}
}

// *********************************************************************************
//                     ADD COMPONENTS PUBLIC FUNCTIONS
// *********************************************************************************

#pragma region AddComponentsAPI

void EntityManager::AddTransformComponent(
	const EntityID& enttID,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& scale)
{
	// add the Transform component to a single entity in terms of arrays
	AddTransformComponent(
		std::vector<EntityID>{enttID},
		std::vector<XMFLOAT3>{position},
		std::vector<XMFLOAT3>{direction},
		std::vector<XMFLOAT3>{scale});
}

///////////////////////////////////////////////////////////

void EntityManager::AddTransformComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// add transform component to all the input entities

	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
	ASSERT_TRUE(enttsCount == positions.size(), "count of entities and positions must be equal");
	ASSERT_TRUE(enttsCount == directions.size(), "count of entities and directions must be equal");
	ASSERT_TRUE(enttsCount == scales.size(), "count of entities and scales must be equal");

	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;
		
		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::TransformComponent);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::WorldMatrixComponent);

		transformSystem_.AddRecords(enttsIDs, positions, directions, scales);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const EntityID& enttID,
	const XMFLOAT3& translation,
	const XMFLOAT4& rotationAngles,
	const XMFLOAT3& scaleFactor)
{
	// add the Move component to a single entity in terms of arrays
	AddMoveComponent(
		std::vector<EntityID>{enttID},
		std::vector<XMFLOAT3>{translation},
		std::vector<XMFLOAT4>{rotationAngles},
		std::vector<XMFLOAT3>{scaleFactor});
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMFLOAT4>& rotationQuats,
	const std::vector<XMFLOAT3>& scaleFactors)
{
	// add the Move component to all the input entities;
	// and setup entities movement using input data arrays

	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
	ASSERT_TRUE(enttsCount == translations.size(), "count of entities and translations must be equal");
	ASSERT_TRUE(enttsCount == rotationQuats.size(), "count of entities and rotationQuats must be equal");
	ASSERT_TRUE(enttsCount == scaleFactors.size(), "count of entities and scaleFactors must be equal");

	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MoveComponent);

		moveSystem_.AddRecords(enttsIDs, translations, rotationQuats, scaleFactors);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponents(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<std::string>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to each input entity all the meshes IDs from the input array

	ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
	ASSERT_NOT_EMPTY(meshesIDs.empty(), "the array of meshes IDs is empty");

	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MeshComp);

		meshSystem_.AddRecords(enttsIDs, meshesIDs);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponents(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<RENDERING_SHADERS>& shadersTypes,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	ASSERT_NOT_ZERO(enttsCount, "the array of entities IDs is empty");
	ASSERT_TRUE(enttsCount == shadersTypes.size(), "entities count != count of the input shaders types");
	ASSERT_TRUE(enttsCount == topologyTypes.size(), "entities count != count of the input primitive topoECS::Logy types");
	
	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::RenderedComponent);

		renderSystem_.AddRecords(enttsIDs, shadersTypes, topologyTypes);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(StringHelper::ConvertNumbersIntoStrings<EntityID>(enttsIDs)));
	}
}

#pragma endregion


// ************************************************************************************
//                            PUBLIC QUERY API
// ************************************************************************************

#pragma region PublicQueryAPI

const std::vector<EntityID>& EntityManager::GetAllEnttsIDs() const
{
	// return IDs of all the existing entities
	return ids_;
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs)
{
	// check by ID if each entity from the input array is created;
	// return: true  -- if some entity from the input arr exists
	//         false -- if all the entities from the input arr don't exist

	for (const EntityID& id : enttsIDs)
	{
		if (std::binary_search(ids_.begin(), ids_.end(), id))
			continue;
		else
			return false;  // some entity by ID doesn't exist
	}
	
	return true;
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsByIDsHaveComponent(
	const std::vector<EntityID>& enttsIDs,
	const ComponentType componentType)
{
	std::vector<ptrdiff_t> enttsDataIdxs;
	GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);

	return CheckEnttsByDataIdxsHaveComponent(enttsDataIdxs, componentType);
}


///////////////////////////////////////////////////////////

#pragma endregion




// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************


std::vector<EntityID> EntityManager::GenerateIDs(const size_t newEnttsCount)
{
	// generate unique IDs in quantity newEnttsCount
	// 
	// return: SORTED array of generated entities IDs

	using u32 = uint_least32_t;
	using engine = std::mt19937;

	std::random_device os_seed;
	const u32 seed = os_seed();
	engine generator(seed);
	std::uniform_int_distribution<u32> distribute(0, UINT_MAX);

	std::vector<EntityID> generatedIDs(newEnttsCount, INVALID_ENTITY_ID);

	// generate a hash by entity name (this hash will be an ID)
	for (size_t idx = 0; idx < newEnttsCount; ++idx)
	{
		u32 id = distribute(generator);

		// if such ID already exists we generate new id value
		while (std::binary_search(ids_.begin(), ids_.end(), id))
		{
			id = distribute(generator);
		}

		generatedIDs[idx] = id;
	}

	std::sort(generatedIDs.begin(), generatedIDs.end());

	return generatedIDs;
}

///////////////////////////////////////////////////////////

void EntityManager::GetDataIdxsByIDs(
	const std::vector<EntityID>& enttsIDs,
	std::vector<ptrdiff_t>& outDataIdxs)
{
	// get an index into data arrays for each input entity name;
	// since all the IDs are sorted the output arr will have sorted idxs;
	// 
	// in:  array of entities IDs
	// out: array of data idxs

	const ptrdiff_t enttsIDsCount = std::ssize(enttsIDs);
	const auto beg = ids_.begin();
	const auto end = ids_.end();
	outDataIdxs.reserve(enttsIDsCount);

	// get index into array for each ID
	for (const EntityID& enttID : enttsIDs)
		outDataIdxs.push_back(std::distance(beg, std::upper_bound(beg, end, enttID)) - 1);
}

///////////////////////////////////////////////////////////

void EntityManager::GetEnttsIDsByDataIdxs(
	const std::vector<ptrdiff_t>& enttsDataIdxs,
	std::vector<EntityID>& outEnttsIDs)
{
	// get entity ID value from the array by data idx
	// 
	// in:   SORTED array of indices
	// out:  array of entities IDs

	outEnttsIDs.reserve(enttsDataIdxs.size());

	for (const ptrdiff_t idx : enttsDataIdxs)
		outEnttsIDs.push_back(ids_[idx]);
}

///////////////////////////////////////////////////////////

void EntityManager::SetEnttsHaveComponent(
	const std::vector<ptrdiff_t>& enttsDataIdxs,
	const ComponentType compType)
{
	// go through each entity and set that this entity has the component by type;
	// input: 1. enttsDataIdxs -- array of data indices 
	//           (you can receive it using the GetDataIdxsByNames function)
	//        2. type of the component (numeral value)

	uint32_t bitmask = (1 << compType);

	for (const ptrdiff_t idx : enttsDataIdxs)
		componentFlags_[idx] |= bitmask;
}


///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsByDataIdxsHaveComponent(
	const std::vector<ptrdiff_t>& enttsDataIdxs,
	const ComponentType componentType)
{
	const uint32_t bitmaskForComponent = (1 << componentType);

	// check if entity has component
	for (const ptrdiff_t idx : enttsDataIdxs)
	{
		if (componentFlags_[idx] & bitmaskForComponent)
			continue;
		else
			return false;   // not all the input entts have the component
	}

	// all the input entts have the component
	return true;
}