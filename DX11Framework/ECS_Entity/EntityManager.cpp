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

	allComponents_.insert({ "Transform" , std::move(pTransform) });
	allComponents_.insert({ "Movement" , std::move(pMovementComponent) });
	allComponents_.insert({ "MeshComponent", std::move(pMeshComponent) });
	allComponents_.insert({ "Rendered", std::move(pRenderComponent) });
}

///////////////////////////////////////////////////////////

void EntityManager::CreateEntities(const std::vector<EntityID>& entityIDs)
{
	COM_ERROR_IF_EMPTY(entityIDs.empty(), "array of entity IDs is empty");

	for (const EntityID& entityID : entityIDs)
	{
		const auto res = entityToComponent_.insert({ entityID, {} });
		if (!res.second)
		{
			COM_ERROR("can't create entity with such ID: " + entityID);
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
		std::set<ComponentID>& componentsIds = it->second;
		std::vector<BaseComponent*> componentsPtrs;

		// get ptrs to all the related components
		for (const ComponentID& id : componentsIds)
			componentsPtrs.push_back(allComponents_.find(id)->second.get());

		// go through each component and remove record about this entity
		for (BaseComponent* pComponent : componentsPtrs)
			pComponent->RemoveRecord(entityID);

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
		COM_ERROR("can't find a component by its component ID");
	}
}



// *********************************************************************************
//                     ADD COMPONENTS PUBLIC FUNCTIONS
// *********************************************************************************

void EntityManager::AddTransformComponents(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// add transform component to all the input entities

	COM_ERROR_IF_EMPTY(entityIDs.empty(), "array of entities IDs is empty");
	assert(entityIDs.size() == positions.size());
	assert(entityIDs.size() == directions.size());
	assert(entityIDs.size() == scales.size());

	const std::string componentID{ "Transform" };
	Transform* pTransform = static_cast<Transform*>(allComponents_.find(componentID)->second.get());
	UINT data_idx = 0;

	for (const EntityID& entityID : entityIDs)
	{
		// check if we can add component to entity
		const bool canAddComponent = HasEntity(entityID) && !HasComponent(entityID, componentID);
		COM_ERROR_IF_FALSE(canAddComponent, "can't add a component (" + componentID + ") to the entity (" + entityID + ")");

		transformSystem_.AddEntity(entityID, pTransform);
		transformSystem_.SetWorld(entityID, scales[data_idx], directions[data_idx], positions[data_idx], *pTransform);
		++data_idx;
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMovementComponents(
	const std::vector<EntityID>& entityIDs,
	const std::vector <DirectX::XMFLOAT3>& translations,
	const std::vector <DirectX::XMFLOAT4>& rotationQuats,
	const std::vector <DirectX::XMFLOAT3>& scaleFactors)
{
	// add movement component to all the input entities

	COM_ERROR_IF_EMPTY(entityIDs.empty(), "array of entities IDs is empty");
	assert(entityIDs.size() == translations.size());
	assert(entityIDs.size() == rotationQuats.size());
	assert(entityIDs.size() == scaleFactors.size());

	const std::string componentID{ "Movement" };
	Movement* pMovementComponent = static_cast<Movement*>(allComponents_.find(componentID)->second.get());

	for (const EntityID& entityID : entityIDs)
	{
		// check if we can add component to entity
		const bool canAddComponent = HasEntity(entityID) && !HasComponent(entityID, componentID);
		COM_ERROR_IF_FALSE(canAddComponent, "can't add a component (" + componentID + ") to the entity (" + entityID + ")");

		moveSystem_.AddEntity(entityID, pMovementComponent);
	}

	//moveSystem_.SetTranslationsByIDs(entityIDs, translations, *pMovementComponent);
	moveSystem_.SetRotationQuatsByIDs(entityIDs, rotationQuats, *pMovementComponent);
	//moveSystem_.SetScaleFactorsByIDs(entityIDs, scaleFactors, *pMovementComponent);
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponents(
	const std::vector<EntityID>& entityIDs,
	const std::string& meshID)
{
	COM_ERROR_IF_EMPTY(meshID.empty(), "mesh ID is empty");

	const std::string componentID{ "MeshComponent" };
	MeshComponent* pMeshComponent = static_cast<MeshComponent*>(allComponents_.find(componentID)->second.get());

	for (const EntityID& entityID : entityIDs)
	{
		// check if we can add component to entity
		const bool canAddComponent = (HasEntity(entityID) && !HasComponent(entityID, componentID));
		COM_ERROR_IF_FALSE(canAddComponent, "can't add a component (" + componentID + ") to the entity (" + entityID + ")");

		// add a component and init its data for this entity
		meshSystem_.AddEntity(entityID, pMeshComponent);
		meshSystem_.AddMesh(entityID, meshID, *pMeshComponent);
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponents(const std::vector<EntityID>& entityIDs)
{
	const std::string componentID{ "Rendered" };
	Rendered* pRenderComponent = static_cast<Rendered*>(allComponents_.find(componentID)->second.get());

	for (const EntityID& entityID : entityIDs)
	{
		// check if we can add component to entity
		const bool canAddComponent = (HasEntity(entityID) && !HasComponent(entityID, componentID));
		COM_ERROR_IF_FALSE(canAddComponent, "can't add a component (" + componentID + ") to the entity (" + entityID + ")");

		// add a component for this entity
		renderSystem_.AddEntity(entityID, pRenderComponent);
	}
}