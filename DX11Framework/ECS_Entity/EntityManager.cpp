#include "EntityManager.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <sstream>

using namespace DirectX;

EntityManager::EntityManager() :
	transformSystem_{ &transform_ },
	MoveSystem_{ &transform_, &movement_ },
	meshSystem_{ &meshComponent_ },
	renderSystem_{ &renderComponent_, &transform_ ,&meshComponent_ }
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

bool EntityManager::CreateEntity(const EntityID& entityID)
{
	ASSERT_NOT_EMPTY(entityID.empty(), "input entity ID is empty");
	
	// if such entity already exist 
	if (entityToComponent_.contains(entityID))
	{
		Log::Error(LOG_MACRO, "can't create entity because there is already one with such ID: " + entityID);
		return false;
	}

	// there is no entity by such ID so create it
	else
	{
		const auto res = entityToComponent_.insert({ entityID, {} });
		if (!res.second)
		{
			THROW_ERROR("can't create entity with such ID: " + entityID);
		}

		return true;
	}
}

///////////////////////////////////////////////////////////

void EntityManager::CreateEntities(const std::vector<EntityID>& entityIDs)
{
	ASSERT_NOT_EMPTY(entityIDs.empty(), "array of entity IDs is empty");

	for (const EntityID& entityID : entityIDs)
	{
		const auto res = entityToComponent_.insert({ entityID, {} });
		if (!res.second)
		{
			THROW_ERROR("can't create entity with such ID: " + entityID);
		}
	}
}

///////////////////////////////////////////////////////////

void EntityManager::DestroyEntity(const EntityID& entityID)
{
	// erase all the records about this entity from all the realted components;
	// and remove this entity;

	try
	{
		std::set<ComponentType>& addedComponents = entityToComponent_.at(entityID);

		
		// go through each related component and remove a record about this entity
		for (const ComponentType type : addedComponents)
		{
			switch (type)
			{
				case ComponentType::TransformComponent:
				{
					transformSystem_.RemoveRecord(entityID);
					break;
				}
				case ComponentType::MoveComponent:
				{
					MoveSystem_.RemoveRecord(entityID);
					break;
				}
				case ComponentType::MeshComp:
				{
					meshSystem_.RemoveRecord(entityID);
					break;
				}
				case ComponentType::RenderedComponent:
				{
					renderSystem_.RemoveRecord(entityID);
					break;
				}
				default:
				{
					THROW_ERROR("Unknow component type (idx): " + std::to_string(type));
				}
			}
		}
		// delete a record about this entity from the manager
		entityToComponent_.erase(entityID);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error("there is no such entity by ID: " + entityID);
	}

}


// ************************************************************************************
//                          PUBLIC UPDATING FUNCTIONS
// ************************************************************************************

void EntityManager::Update(const float deltaTime)
{
	MoveSystem_.UpdateAllMoves(deltaTime);
}


// ************************************************************************************
//                          PUBLIC RENDERING FUNCTIONS
// ************************************************************************************

void EntityManager::Render(
	ID3D11DeviceContext* pDeviceContext,
	MeshStorage& meshStorage,
	ColorShaderClass& colorShader,
	TextureShaderClass& textureShader,
	LightShaderClass& lightShader,
	const std::vector<DirectionalLight>& dirLights,
	const std::vector<PointLight>& pointLights,
	const std::vector<SpotLight>& spotLights,
	const DirectX::XMFLOAT3& cameraPos,
	const DirectX::XMMATRIX& viewProj)
{
	try
	{
		renderSystem_.Render(
			pDeviceContext,
			meshStorage,
			colorShader,
			textureShader,
			lightShader,
			dirLights,
			pointLights,
			spotLights,
			cameraPos,
			viewProj);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't find a component by its component ID");
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't render scene using the RenderSystem (ECS)");
	}
}



// *********************************************************************************
//                     ADD COMPONENTS PUBLIC FUNCTIONS
// *********************************************************************************

void EntityManager::AddTransformComponent(
	const EntityID& entityID,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& scale)
{
	// add the Transform component to a single entity in terms of arrays
	AddTransformComponent(
		std::vector<EntityID>{entityID},
		std::vector<XMFLOAT3>{position},
		std::vector<XMFLOAT3>{direction},
		std::vector<XMFLOAT3>{scale});
}

///////////////////////////////////////////////////////////

void EntityManager::AddTransformComponent(
	const std::vector<EntityID>& entitiesIDs,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// add transform component to all the input entities

	ASSERT_NOT_EMPTY(entitiesIDs.empty(), "array of entities IDs is empty");
	ASSERT_TRUE(entitiesIDs.size() == positions.size(), "count of entities and positions must be equal");
	ASSERT_TRUE(entitiesIDs.size() == directions.size(), "count of entities and directions must be equal");
	ASSERT_TRUE(entitiesIDs.size() == scales.size(), "count of entities and scales must be equal");

	try
	{
		for (const EntityID& entityID : entitiesIDs)
		{
			entityToComponent_.at(entityID).insert(ComponentType::TransformComponent);  // set that this entity has the component
			transformSystem_.AddRecord(entityID);                                    // add a record about this entity into the component
		}

		// setup transform data + world matrix for each entity by ID
		transformSystem_.SetWorlds(entitiesIDs, positions, directions, scales);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add Transform component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add Transform component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const EntityID& entityID,
	const XMFLOAT3& translation,
	const XMFLOAT4& rotationAngles,
	const XMFLOAT3& scaleFactor)
{
	// add the Move component to a single entity in terms of arrays
	AddMoveComponent(
		std::vector<EntityID>{entityID},
		std::vector<XMFLOAT3>{translation},
		std::vector<XMFLOAT4>{rotationAngles},
		std::vector<XMFLOAT3>{scaleFactor});
}

void EntityManager::AddMoveComponent(
	const std::vector<EntityID>& entitiesIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMFLOAT4>& rotationQuats,
	const std::vector<XMFLOAT3>& scaleFactors)
{
	// add the Move component to all the input entities;
	// and setup entities movement using input data arrays

	ASSERT_NOT_EMPTY(entitiesIDs.empty(), "array of entities IDs is empty");
	ASSERT_TRUE(entitiesIDs.size() == translations.size(), "count of entities and translations must be equal");
	ASSERT_TRUE(entitiesIDs.size() == rotationQuats.size(), "count of entities and rotationQuats must be equal");
	ASSERT_TRUE(entitiesIDs.size() == scaleFactors.size(), "count of entities and scaleFactors must be equal");

	try
	{
		for (const EntityID& entityID : entitiesIDs)
		{
			ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");
			entityToComponent_.at(entityID).insert(ComponentType::MoveComponent);  // set that this entity has the component
			MoveSystem_.AddRecord(entityID);                                    // add a record about this entity into the component
		}

		MoveSystem_.SetTranslationsByIDs(entitiesIDs, translations);
		MoveSystem_.SetRotationQuatsByIDs(entitiesIDs, rotationQuats);
		MoveSystem_.SetScaleFactorsByIDs(entitiesIDs, scaleFactors);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add Transform component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add movement component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponents(
	const std::vector<EntityID>& entitiesIDs,
	const std::vector<std::string>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to this entity all the meshes IDs from the input array

	ASSERT_NOT_EMPTY(entitiesIDs.empty(), "the array of entity IDs is empty");
	ASSERT_NOT_EMPTY(meshesIDs.empty(), "the array of meshes IDs is empty");

	try
	{
		for (const EntityID& entityID : entitiesIDs)
		{
			entityToComponent_.at(entityID).insert(ComponentType::MeshComp);  // set that this entity has the component
			meshSystem_.AddRecord(entityID);                                    // add a record about this entity into the component
		}

		meshSystem_.AddMeshesToEntities(entitiesIDs, meshesIDs);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add Transform component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add mesh component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponents(const std::vector<EntityID>& entitiesIDs)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	ASSERT_NOT_EMPTY(entitiesIDs.empty(), "the array of entities IDs is empty");
	
	try
	{
		for (const EntityID& entityID : entitiesIDs)
		{
			entityToComponent_.at(entityID).insert(ComponentType::RenderedComponent);  // set that this entity has the component
			renderSystem_.AddRecord(entityID);                                    // add a record about this entity into the component
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add Transform component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't add render component to entities by IDs: " + GetStringOfEntitiesIDs(entitiesIDs));
	}
}

///////////////////////////////////////////////////////////

const std::set<EntityID> EntityManager::GetAllEntitiesIDs() const
{
	std::set<EntityID> ids;

	for (const auto& it : entityToComponent_)
		ids.insert(it.first);
	
	return ids;
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEntitiesExist(const std::vector<EntityID>& entitiesIDs)
{
	// check if each entity from the input array is created;
	// return: true  -- if each entity exists
	//         false -- if any entity from input arr doesn't exist

	const std::set<EntityID> createdEntities = GetAllEntitiesIDs();
	const std::set<EntityID> enttIDsSet(entitiesIDs.begin(), entitiesIDs.end());

	return std::includes(createdEntities.begin(), createdEntities.end(), enttIDsSet.begin(), enttIDsSet.end());
}


// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************

std::string EntityManager::GetStringOfEntitiesIDs(const std::vector<EntityID>& entitiesIDs)
{
	std::string entitiesIDsStr{ "" };

	for (const EntityID& id : entitiesIDs)
		entitiesIDsStr += { id + ", "};

	return entitiesIDsStr;
}