#include "EntityManager.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Log.h"

#include "EntityManagerSerializer.h"
#include "EntityManagerDeserializer.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <cctype>
#include <random>

using namespace DirectX;
using namespace ECS;


EntityManager::EntityManager() :
	nameSystem_ {&names_},
	transformSystem_{ &transform_, &world_ },
	moveSystem_{ &transform_, &world_, &movement_ },
	meshSystem_{ &meshComponent_ },
	renderSystem_{ &renderComponent_, &transform_, &world_, &meshComponent_ },
	texturesSystem_{ &textureComponent_ }
{
	const size_t reserveMemForEnttsCount = 100;

	ids_.reserve(reserveMemForEnttsCount);
	componentFlags_.reserve(reserveMemForEnttsCount);

	// make pairs ['component_type' => 'component_name']
	componentTypeToName_ =
	{
		{ ComponentType::TransformComponent, "Transform" },
		{ ComponentType::NameComponent, "Name" },
		{ ComponentType::MoveComponent, "Movement" },
		{ ComponentType::MeshComp, "MeshComponent" },
		{ ComponentType::RenderedComponent, "Rendered" },
		{ ComponentType::WorldMatrixComponent, "WorldMatrix" }
	};
}

EntityManager::~EntityManager()
{
	//Log::Debug(LOG_MACRO);
}


// ************************************************************************************
//                 PUBLIC SERIALIZATION / DESERIALIZATION API
// ************************************************************************************

bool EntityManager::Serialize(const std::string& dataFilepath)
{
	try
	{
		EntityManagerSerializer serializer;
		serializer.Serialize(*this, dataFilepath);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't serialize data into a file: " + dataFilepath);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool EntityManager::Deserialize(const std::string& dataFilepath)
{
	try
	{
		EntityManagerDeserializer deserializer;
		deserializer.Deserialize(*this, dataFilepath);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't deserialize data from the file: " + dataFilepath);
		return false;
	}

	return true;
}



// ************************************************************************************
//                     PUBLIC CREATION/DESTROYMENT API
// ************************************************************************************

#pragma region PublicCreationDestroymentAPI

EntityID EntityManager::CreateEntity()
{
	// create a new entity;
	// return: a generated ID of this new entity

	return CreateEntities(1).front();
}

///////////////////////////////////////////////////////////

std::vector<EntityID> EntityManager::CreateEntities(const size_t newEnttsCount)
{
	// create batch of new empty entities, generate for each entity 
	// unique ID and set that it hasn't any component by default;
	//
	// return: SORTED array of IDs of just created entities;

	ASSERT_NOT_ZERO(newEnttsCount, "new entitites count == 0");

	std::vector<EntityID> generatedIDs;
	GenerateIDs(newEnttsCount, generatedIDs);

	for (const EntityID& ID : generatedIDs)
	{
		const ptrdiff_t insertAtPos = Utils::GetPosForID(ids_, ID);

		// add new ID into the sorted array of IDs
		Utils::InsertAtPos<EntityID>(ids_, insertAtPos, ID);

		// set that each new entity by default doesn't have any component
		Utils::InsertAtPos<uint32_t>(componentFlags_, insertAtPos, 0);
	}

	return generatedIDs;
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
	moveSystem_.UpdateAllMoves(deltaTime, transformSystem_);
}


// ************************************************************************************
//                          PUBLIC RENDERING FUNCTIONS
// ************************************************************************************

void EntityManager::GetRenderingDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<XMMATRIX>& outWorldMatrices,
	std::vector<RENDERING_SHADERS>& outShaderTypes,
	std::vector<MeshID>& outMeshesIDs,
	std::vector<std::set<EntityID>>& outEnttsByMeshes)
{
	// get data which will be used for rendering of the entities;
	// in:   array of entities IDs
	// out:  rendering data of each input entity by its ID

	try
	{
		transformSystem_.GetWorldMatricesByIDs(enttsIDs, outWorldMatrices);
		renderSystem_.GetRenderingDataOfEntts(enttsIDs, outShaderTypes);

		meshSystem_.GetMeshesIDsRelatedToEntts(
			enttsIDs,
			outMeshesIDs,
			outEnttsByMeshes);
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

void EntityManager::SetEnttsHaveComponent(
	const std::vector<EntityID>& enttsIDs,
	const ComponentType compType)
{
	// go through each entity and set that this entity has the component by type;
	// input: 1. array of entities IDs
	//        2. type of the component (numeral value)

	std::vector<ptrdiff_t> dataIdxs;

	GetDataIdxsByIDs(enttsIDs, dataIdxs);
	SetEnttsHaveComponent(dataIdxs, compType);
}

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

void EntityManager::AddNameComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<EntityName>& enttsNames)
{	
	// add the Name component to all the input entities
	// so each entity will have its own name
	try
	{
		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
		ASSERT_TRUE(enttsCount == enttsNames.size(), "count of entities IDs and names must be equal");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::NameComponent);

		nameSystem_.AddRecords(enttsIDs, enttsNames);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

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

	try
	{
		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
		ASSERT_TRUE(enttsCount == positions.size(), "count of entities and positions must be equal");
		ASSERT_TRUE(enttsCount == directions.size(), "count of entities and directions must be equal");
		ASSERT_TRUE(enttsCount == scales.size(), "count of entities and scales must be equal");


		std::vector<ptrdiff_t> enttsDataIdxs;
		
		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::TransformComponent);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::WorldMatrixComponent);

		transformSystem_.AddRecords(enttsIDs, positions, directions, scales);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
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

	try
	{
		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
		ASSERT_TRUE(enttsCount == translations.size(), "count of entities and translations must be equal");
		ASSERT_TRUE(enttsCount == rotationQuats.size(), "count of entities and rotationQuats must be equal");
		ASSERT_TRUE(enttsCount == scaleFactors.size(), "count of entities and scaleFactors must be equal");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MoveComponent);

		moveSystem_.AddRecords(enttsIDs, translations, rotationQuats, scaleFactors);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponent(
	const EntityID& enttID,
	const std::vector<size_t>& meshesIDs)
{
	// add the Mesh component to a single entity by ID in terms of arrays
	AddMeshComponent(
		std::vector<EntityID>{enttID},
		meshesIDs);
}

void EntityManager::AddMeshComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<size_t>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to each input entity all the meshes IDs from the input array

	try
	{
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_NOT_EMPTY(meshesIDs.empty(), "the array of meshes IDs is empty");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MeshComp);

		meshSystem_.AddRecords(enttsIDs, meshesIDs);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponent(
	const EntityID& enttID,
	const ECS::RENDERING_SHADERS renderShaderType,
	const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	// add the Rendered component to a single entity by ID in terms of arrays
	AddRenderingComponent(
		std::vector<EntityID>{enttID},
		std::vector<ECS::RENDERING_SHADERS>{renderShaderType},
		std::vector<D3D11_PRIMITIVE_TOPOLOGY>{topologyType});
}

void EntityManager::AddRenderingComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<ECS::RENDERING_SHADERS>& shadersTypes,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	try
	{
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_TRUE(std::ssize(enttsIDs) == shadersTypes.size(), "entities count != count of the input shaders types");
		ASSERT_TRUE(std::ssize(enttsIDs) == topologyTypes.size(), "entities count != count of the input primitive topoECS::Logy types");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::RenderedComponent);

		renderSystem_.AddRecords(enttsIDs, shadersTypes, topologyTypes);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTextureComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<std::vector<TextureID>>& textures)
{
	// add Texture component to each input entity by its ID
	// and set that this entity has such textures

	try
	{
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_TRUE(std::ssize(enttsIDs) == std::ssize(textures), "entities count != textures arrays count");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::TexturedComponent);

		texturesSystem_.AddRecords(enttsIDs, textures);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
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
	//         false -- if some entity from the input arr doesn't exist

	bool allExist = true;

	for (const EntityID& id : enttsIDs)
		allExist &= std::binary_search(ids_.begin(), ids_.end(), id);
	
	return allExist;
}

///////////////////////////////////////////////////////////

void EntityManager::GetComponentFlagsByIDs(
	const std::vector<EntityID>& ids,
	std::vector<ComponentFlagsType>& outFlags)
{
	// get component bitmasks of entities by ID

	std::vector<ptrdiff_t> dataIdxs;
	GetDataIdxsByIDs(ids, dataIdxs);

	outFlags.reserve(ids.size());

	for (const ptrdiff_t idx : dataIdxs)
		outFlags.push_back(componentFlags_[idx]);
}

///////////////////////////////////////////////////////////

void EntityManager::GetEnttsByComponent(
	const ComponentType componentType,
	std::vector<EntityID>& outIDs)
{
	// get IDs of entities which have such component;

	switch (componentType)
	{
		case ComponentType::TransformComponent:
		{
			outIDs = transform_.ids_;
			return;
		}
		case ComponentType::WorldMatrixComponent:
		{
			transformSystem_.GetAllEnttsIDsFromWorldMatrixComponent(outIDs);
			return;
		}
		case ComponentType::MoveComponent:
		{
			outIDs = movement_.ids_;
			return;
		}
		case ComponentType::MeshComp:
		{
			meshSystem_.GetEnttsIDsFromMeshComponent(outIDs);
			return;
		}
		case ComponentType::RenderedComponent:
		{
			outIDs = renderComponent_.ids_;
			return;
		}
		default:
		{
			Log::Error(LOG_MACRO, "Unknown component type: " + std::to_string(componentType));
			THROW_ERROR("can't get IDs of entities which have such component: " + std::to_string(componentType));
		}
	}
}

#pragma endregion




// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************


void EntityManager::GenerateIDs(
	const size_t newEnttsCount,
	std::vector<EntityID>& outGeneratedIDs)
{
	// generate unique IDs in quantity newEnttsCount
	// 
	// in:  how many entities we will create
	// out: SORTED array of generated entities IDs

	using u32 = uint_least32_t;
	using engine = std::mt19937;

	std::random_device os_seed;
	const u32 seed = os_seed();
	engine generator(seed);
	std::uniform_int_distribution<u32> distribute(0, UINT_MAX);

	outGeneratedIDs.reserve(newEnttsCount);

	// generate an ID for each new entity
	for (size_t idx = 0; idx < newEnttsCount; ++idx)
	{
		u32 id = distribute(generator);

		// if such ID already exists we generate new id value
		while (std::binary_search(ids_.begin(), ids_.end(), id))
		{
			id = distribute(generator);
		}

		outGeneratedIDs.push_back(id);
	}

	// after generation we sort IDs so then it will be faster to store them
	std::sort(outGeneratedIDs.begin(), outGeneratedIDs.end());
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
