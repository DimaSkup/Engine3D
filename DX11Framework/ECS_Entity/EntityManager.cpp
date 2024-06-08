#include "EntityManager.h"
#include "Utils.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace DirectX;

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
		Log::Error(LOG_MACRO, "can't open file for writing: " + dataFilepath);
		exit(-1);
	}

	std::stringstream ss;

	//const DirectX::XMFLOAT3& pos = modelsTransformData.at("sphere").positions[0];
	//ss << pos.x << " " << pos.y << " " << pos.z;
	//fout.write(ss.str().c_str(), ss.str().size());

	// SERIALIZE POSITIONS
	const std::vector<DirectX::XMFLOAT3>& positions = modelsTransformData.at("sphere").positions;
	const DirectX::XMFLOAT3* ptrPosRawData = positions.data();
	fout.write((const char*)(ptrPosRawData), positions.size() * sizeof(DirectX::XMFLOAT3));


	fout.close();

	// DESERIALIZE POSITIONS
	std::ifstream fin(dataFilepath, std::ios::binary);
	if (!fin.is_open())
	{
		Log::Error(LOG_MACRO, "can't open file for reading: " + dataFilepath);
		exit(-1);
	}

	std::vector<DirectX::XMFLOAT3> deserializedPositions(positions.size());

	fin.read((char*)(deserializedPositions.data()), positions.size() * sizeof(DirectX::XMFLOAT3));
	fin.close();

	//const UINT spheresCount = modelsTransformData.at("sphere").positions.size();
	//GenerateEntitiesIDs(spheresCount, "sphere", generatedEntituiesIDs);
#endif
}

///////////////////////////////////////////////////////////

void EntityManager::Deserialize()
{
}



// ************************************************************************************
//                     PUBLIC CREATION/DESTROYMENT API
// ************************************************************************************

#pragma region PublicCreationDestroymentAPI

void EntityManager::CreateEntities(const std::vector<EntityName>& enttsNames)
{
	// create batch of new empty entities with names from the input arr;
	// generate for each entity unique ID and set that it hasn't any component by default
	ASSERT_NOT_EMPTY(enttsNames.empty(), "array of entities names is empty");
	ASSERT_TRUE(!CheckEnttsByNamesExist(enttsNames), "these is already some entity with the same name");

	// set that each new entity by default doesn't have any component
	componentFlags_.insert(componentFlags_.end(), enttsNames.size(), 0);  

	Utils::AppendArray(ids_, GenerateIDsByNames(enttsNames));
	Utils::AppendArray(names_, enttsNames);
}

///////////////////////////////////////////////////////////

#if 0
void EntityManager::DestroyEntity(const entitiesName& entitiesName)
{
	// erase all the records about this entity from all the realted components;
	// and remove this entity;

	try
	{
		std::set<ComponentType>& addedComponents = entityToComponent_.at(entitiesName);

		
		// go through each related component and remove a record about this entity
		for (const ComponentType type : addedComponents)
		{
			switch (type)
			{
				case ComponentType::TransformComponent:
				{
					transformSystem_.RemoveRecord(entitiesName);
					break;
				}
				case ComponentType::MoveComponent:
				{
					moveSystem_.RemoveRecord(entitiesName);
					break;
				}
				case ComponentType::MeshComp:
				{
					meshSystem_.RemoveRecord(entitiesName);
					break;
				}
				case ComponentType::RenderedComponent:
				{
					renderSystem_.RemoveRecord(entitiesName);
					break;
				}
				default:
				{
					THROW_ERROR("Unknow component type (idx): " + std::to_string(type));
				}
			}
		}
		// delete a record about this entity from the manager
		entityToComponent_.erase(entitiesName);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error("there is no such entity by ID: " + entitiesName);
	}

}
#endif

#pragma endregion

// ************************************************************************************
//                          PUBLIC UPDATING FUNCTIONS
// ************************************************************************************

#pragma region PublicUpdatingAPI

void EntityManager::Update(const float deltaTime)
{
	moveSystem_.UpdateAllMoves(deltaTime);
}

#pragma endregion


// ************************************************************************************
//                          PUBLIC RENDERING FUNCTIONS
// ************************************************************************************

#pragma region PublicRenderingAPI

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
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't get rendering data using the RenderSystem (ECS)");
	}
}

#pragma endregion

// *********************************************************************************
//                     ADD COMPONENTS PUBLIC FUNCTIONS
// *********************************************************************************

void EntityManager::AddTransformComponent(
	const EntityName& enttName,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& scale)
{
	// add the Transform component to a single entity in terms of arrays
	AddTransformComponent(
		std::vector<EntityName>{enttName},
		std::vector<XMFLOAT3>{position},
		std::vector<XMFLOAT3>{direction},
		std::vector<XMFLOAT3>{scale});
}

///////////////////////////////////////////////////////////

void EntityManager::AddTransformComponent(
	const std::vector<EntityName>& enttsNames,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// add transform component to all the input entities

	const ptrdiff_t enttsCount = std::ssize(enttsNames);
	ASSERT_NOT_ZERO(enttsCount, "array of entities names is empty");
	ASSERT_TRUE(enttsCount == positions.size(), "count of entities and positions must be equal");
	ASSERT_TRUE(enttsCount == directions.size(), "count of entities and directions must be equal");
	ASSERT_TRUE(enttsCount == scales.size(), "count of entities and scales must be equal");

	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;
		std::vector<EntityID> enttsIDs;

		GetDataIdxsByNames(enttsNames, enttsDataIdxs);
		GetEnttsIDsByDataIdxs(enttsDataIdxs, enttsIDs);
		SetEnttsHasComponent(enttsDataIdxs, ComponentType::TransformComponent);
		SetEnttsHasComponent(enttsDataIdxs, ComponentType::WorldMatrixComponent);

		transformSystem_.AddRecords(enttsIDs, positions, directions, scales);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const EntityName& enttName,
	const XMFLOAT3& translation,
	const XMFLOAT4& rotationAngles,
	const XMFLOAT3& scaleFactor)
{
	// add the Move component to a single entity in terms of arrays
	AddMoveComponent(
		std::vector<EntityName>{enttName},
		std::vector<XMFLOAT3>{translation},
		std::vector<XMFLOAT4>{rotationAngles},
		std::vector<XMFLOAT3>{scaleFactor});
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const std::vector<EntityName>& enttsNames,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMFLOAT4>& rotationQuats,
	const std::vector<XMFLOAT3>& scaleFactors)
{
	// add the Move component to all the input entities;
	// and setup entities movement using input data arrays

	const ptrdiff_t enttsCount = std::ssize(enttsNames);
	ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
	ASSERT_TRUE(enttsCount == translations.size(), "count of entities and translations must be equal");
	ASSERT_TRUE(enttsCount == rotationQuats.size(), "count of entities and rotationQuats must be equal");
	ASSERT_TRUE(enttsCount == scaleFactors.size(), "count of entities and scaleFactors must be equal");

	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;
		std::vector<EntityID> enttsIDs;

		GetDataIdxsByNames(enttsNames, enttsDataIdxs);
		GetEnttsIDsByDataIdxs(enttsDataIdxs, enttsIDs);
		SetEnttsHasComponent(enttsDataIdxs, ComponentType::TransformComponent);

		moveSystem_.AddRecords(enttsIDs, translations, rotationQuats, scaleFactors);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponents(
	const std::vector<EntityName>& enttsNames,
	const std::vector<std::string>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to this entity all the meshes IDs from the input array

	ASSERT_NOT_EMPTY(enttsNames.empty(), "the array of entities names is empty");
	ASSERT_NOT_EMPTY(meshesIDs.empty(), "the array of meshes IDs is empty");

	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;
		std::vector<EntityID> enttsIDs;

		GetDataIdxsByNames(enttsNames, enttsDataIdxs);
		GetEnttsIDsByDataIdxs(enttsDataIdxs, enttsIDs);
		SetEnttsHasComponent(enttsDataIdxs, ComponentType::MeshComp);

		meshSystem_.AddRecords(enttsIDs, meshesIDs);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponents(
	const std::vector<EntityName>& enttsNames,
	const std::vector<RENDERING_SHADERS>& shadersTypes,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& primTopologyArr)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	const ptrdiff_t enttsCount = std::ssize(enttsNames);
	ASSERT_NOT_ZERO(enttsCount, "the array of entities names is empty");
	ASSERT_TRUE(enttsCount == shadersTypes.size(), "entities count != count of the input shaders types");
	ASSERT_TRUE(enttsCount == primTopologyArr.size(), "entities count != count of the input primitive topology types");
	
	try
	{
		std::vector<ptrdiff_t> enttsDataIdxs;
		std::vector<EntityID> enttsIDs;

		GetDataIdxsByNames(enttsNames, enttsDataIdxs);
		GetEnttsIDsByDataIdxs(enttsDataIdxs, enttsIDs);
		SetEnttsHasComponent(enttsDataIdxs, ComponentType::RenderedComponent);

		renderSystem_.AddRecords(enttsIDs, shadersTypes, primTopologyArr);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add component to entities by IDs: " + StringHelper::Join(enttsNames));
	}
}

// ************************************************************************************
//    
//                             PUBLIC QUERY API
// 
// ************************************************************************************

#pragma region PublicQueryAPI

void EntityManager::GetIDsOfEnttsByNames(
	const std::vector<EntityName>& enttsNames,
	std::vector<EntityID>& outEnttsIDs)
{
	outEnttsIDs.reserve(std::ssize(enttsNames));

	
}

const std::vector<EntityName>& EntityManager::GetAllEnttsNames() const
{
	// return names of all the existing entities
	return names_;
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsByNamesExist(const std::vector<EntityName>& enttsToCheck)
{
	// check if each entity from the input array is created;
	// return: true  -- if some entity from the input arr exists
	//         false -- if all the entities from the input arr don't exist

	const std::vector<EntityName>& entts = GetAllEnttsNames();  // created entities

	for (const EntityName& name : enttsToCheck)
	{
		if (std::find(entts.begin(), entts.end(), name) != entts.end())
			return true;
	}

	return false;
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckIfEnttsExist(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT FIRST!" && 0);
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsHaveComponent(
	const std::vector<EntityName>& enttsNames,
	const ComponentType componentType)
{
	assert("TODO: IMPLEMENT IT FIRST!" && 0);
}

///////////////////////////////////////////////////////////

const std::map<ComponentType, ComponentID>& GetComponentsIDsByTypes(
	const std::vector<ComponentType>& componentTypes)
{
	assert("TODO: IMPLEMENT IT FIRST!" && 0);
}

#pragma endregion




// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************


std::vector<EntityID> EntityManager::GenerateIDsByNames(
	const std::vector<EntityName>& enttsNames)
{
	// generate unique ID for each input entity name
	// return: array of entities IDs

	ASSERT_NOT_EMPTY(enttsNames.empty(), "the entities names array is empty");

	std::vector<EntityID> ids(enttsNames.size(), INVALID_ENTITY_ID);

	// generate a hash by entity name (this hash will be an ID)
	for (size_t idx = 0; idx < enttsNames.size(); ++idx)
		ids[idx] = std::hash<std::string>{}(enttsNames[idx]);

	return ids;
}

///////////////////////////////////////////////////////////

void EntityManager::SetEnttsHasComponent(
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

void EntityManager::GetDataIdxsByNames(
	const std::vector<EntityName>& enttsNames,
	std::vector<ptrdiff_t>& outEnttsDataIdxs)
{
	// get array index of each entity name in the names array;
	// in:  array of entities names
	// out: array of data idxs

	outEnttsDataIdxs.reserve(enttsNames.size());

	const auto beg = names_.begin();
	const auto end = names_.end();

	for (const EntityName& name : enttsNames)
	{
		const auto it = std::find(beg, end, name);
		outEnttsDataIdxs.push_back(std::distance(beg, it));    // store the data idx
	}

	std::sort(outEnttsDataIdxs.begin(), outEnttsDataIdxs.end());
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

	//std::sort(outEnttsIDs.begin(), outEnttsIDs.end());
}