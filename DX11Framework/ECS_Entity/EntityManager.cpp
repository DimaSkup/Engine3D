#include "EntityManager.h"

#include <cassert>
#include <algorithm>
#include <vector>

#include "EntityManagerHelpers.h"




EntityManager::EntityManager()
{
	CreateComponents();
}

EntityManager::~EntityManager()
{
	//
}


void EntityManager::CreateSystems()
{
	//
}

void EntityManager::CreateComponents()
{
	std::unique_ptr<BaseComponent> pTransform = std::make_unique<Transform>();
	std::unique_ptr<BaseComponent> pMovementComponent = std::make_unique<Movement>();
	std::unique_ptr<BaseComponent> pMeshComponent = std::make_unique<MeshComponent>();
	std::unique_ptr<BaseComponent> pRenderComponent = std::make_unique<Rendered>();

	allComponents_.insert({ pTransform->GetComponentID() , std::move(pTransform)});
	allComponents_.insert({ pMovementComponent->GetComponentID(), std::move(pMovementComponent)});
	allComponents_.insert({ pMeshComponent->GetComponentID(), std::move(pMeshComponent) });
	allComponents_.insert({ pRenderComponent->GetComponentID(), std::move(pRenderComponent)});


	// store IDs of components as set
	for (const auto& it : allComponents_)
		componentsIDsSet_.insert(it.first);
}

///////////////////////////////////////////////////////////

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
	}
}

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

	const auto it = entityToComponent_.find(entityID);

	if (it != entityToComponent_.end())
	{
		std::set<ComponentID>& relatedComponents = it->second;

		// go through each related component and remove a record about this entity
		for (const ComponentID& componentID : relatedComponents)
			allComponents_[componentID].get()->RemoveRecord(entityID);

		// delete a record about this entity from the manager
		entityToComponent_.erase(entityID);
	}
}


// ************************************************************************************
//                          PUBLIC UPDATING FUNCTIONS
// ************************************************************************************

void EntityManager::Update(const float deltaTime)
{
	Transform* pTransform = static_cast<Transform*>(allComponents_.find("Transform")->second.get());
	Movement* pMovement = static_cast<Movement*>(allComponents_.find("Movement")->second.get());

	moveSystem_.Update(
		deltaTime,
		*pTransform,
		*pMovement);
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
		Rendered* pRenderComponent = static_cast<Rendered*>(allComponents_.at("Rendered").get());
		Transform* pTransformComponent = static_cast<Transform*>(allComponents_.at("Transform").get());
		MeshComponent* pMeshComponent = static_cast<MeshComponent*>(allComponents_.at("MeshComponent").get());

		renderSystem_.Render(
			pDeviceContext,
			*pRenderComponent,
			*pTransformComponent,
			*pMeshComponent,
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
	// add transform component for a SINGLE entity and
	// set its transform data to default if we didn't passed any
	ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");

	const std::string componentID{ "Transform" };
	try
	{
		Transform* pTransformComponent = static_cast<Transform*>(allComponents_.at(componentID).get());
		AddComponentHelper({ entityID }, &transformSystem_, pTransformComponent);
		transformSystem_.SetWorlds({ entityID }, { position }, { direction }, { scale }, *pTransformComponent);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("there is no such a component: " + componentID);
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTransformComponents(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// add transform component to all the input entities

	ASSERT_NOT_EMPTY(entityIDs.empty(), "array of entities IDs is empty");
	ASSERT_TRUE(entityIDs.size() == positions.size(), "count of entities and positions must be equal");
	ASSERT_TRUE(entityIDs.size() == directions.size(), "count of entities and directions must be equal");
	ASSERT_TRUE(entityIDs.size() == scales.size(), "count of entities and scales must be equal");

	const std::string componentID{ "Transform" };
	try
	{
		Transform* pTransformComponent = static_cast<Transform*>(allComponents_.at(componentID).get());
		AddComponentHelper(entityIDs, &transformSystem_, pTransformComponent);
		transformSystem_.SetWorlds(entityIDs, positions, directions, scales, *pTransformComponent);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("there is no such a component: " + componentID);
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMovementComponents(
	const std::vector<EntityID>& entityIDs,
	const std::vector <DirectX::XMFLOAT3>& translations,
	const std::vector <DirectX::XMFLOAT4>& rotationQuats,
	const std::vector <DirectX::XMFLOAT3>& scaleFactors)
{
	// add movement component to all the input entities;
	// and setup entities movement using input data arrays

	ASSERT_NOT_EMPTY(entityIDs.empty(), "array of entities IDs is empty");
	ASSERT_TRUE(entityIDs.size() == translations.size(), "count of entities and translations must be equal");
	ASSERT_TRUE(entityIDs.size() == rotationQuats.size(), "count of entities and rotationQuats must be equal");
	ASSERT_TRUE(entityIDs.size() == scaleFactors.size(), "count of entities and scaleFactors must be equal");

	const std::string componentID{ "Movement" };

	try
	{
		Movement* pMovementComponent = static_cast<Movement*>(allComponents_.at(componentID).get());
		AddComponentHelper(entityIDs, &moveSystem_, pMovementComponent);
		moveSystem_.SetTranslationsByIDs(entityIDs, translations, *pMovementComponent);
		moveSystem_.SetRotationQuatsByIDs(entityIDs, rotationQuats, *pMovementComponent);
		moveSystem_.SetScaleFactorsByIDs(entityIDs, scaleFactors, *pMovementComponent);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("there is no such a component: " + componentID);
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponents(
	const std::vector<EntityID>& entityIDs,
	const std::vector<std::string>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to this entity all the meshes IDs from the input array

	ASSERT_NOT_EMPTY(entityIDs.empty(), "the array of entity IDs is empty");
	ASSERT_NOT_EMPTY(meshesIDs.empty(), "the array of meshes IDs is empty");
	const std::string componentID{ "MeshComponent" };

	try
	{
		MeshComponent* pMeshComponent = static_cast<MeshComponent*>(allComponents_.find(componentID)->second.get());
		AddComponentHelper(entityIDs, &meshSystem_, pMeshComponent);

		for (const MeshID& meshID : meshesIDs)
		{
			meshSystem_.AddMeshToEntities(entityIDs, meshID, *pMeshComponent);
		}
		
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("there is no such a component: " + componentID);
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponents(const std::vector<EntityID>& entityIDs)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	ASSERT_NOT_EMPTY(entityIDs.empty(), "the array of entity IDs is empty");
	const std::string componentID{ "Rendered" };
	
	try
	{
		Rendered* pRenderComponent = static_cast<Rendered*>(allComponents_.find(componentID)->second.get());
		AddComponentHelper(entityIDs, &renderSystem_, pRenderComponent);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("there is no such a component: " + componentID);
	}
}

// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************

void EntityManager::AddComponentHelper(
	const std::vector<EntityID>& entitiesIDs,
	BaseSystem* pSystem,
	BaseComponent* pComponent)
{
	// add component to each entity by its ID:
	// 1. make a record ['entity_id' => 'component_id'] inside the manager;
	// 2. make a record ['entity_id' => 'component_data'] inside the component;

	const std::string componentID = pComponent->GetComponentID();

	try
	{
		// check if there is such a component and entities by IDs
		CheckComponentExist(componentID);
		CheckEntitiesExist(entitiesIDs);

		for (const EntityID& entityID : entitiesIDs)
		{
			if (!CheckEntityHasComponent(entityID, componentID))   // check if we can add component to entity
			{
				entityToComponent_[entityID].insert(componentID);  // set that this entity has the component
				pSystem->AddEntity(entityID, pComponent);          // add a record about this entity into the component
			}
		}
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("Can't add a component (" + componentID + ") " + 
			        "to entities : " + GetStringOfEntitiesIDs(entitiesIDs));
	}
}