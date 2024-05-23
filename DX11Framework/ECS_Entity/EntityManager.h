// **********************************************************************************
// Filename:     EntityManager.h
// Description:
// 
// Created:
// **********************************************************************************

#pragma once


#include "ECS_Types.h"

#if ECS_VER_0

#include "Entity.h"
#include "../ECS_System/BaseSystem.h"
#include "../ECS_Components/BaseComponent.h"

#include "../ECS_Components/Transform.h"
#include "../ECS_Components/Movement.h"
#include "../ECS_Components/Mesh.h"

#include "../Engine/log.h"

#include <string>
#include <map>
#include <vector>

typedef unsigned int UINT;

class EntityManager
{
public:
	EntityManager();
	EntityManager(const EntityManager&) = delete;
	EntityManager(EntityManager&&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager& operator=(EntityManager&&) = delete;
	~EntityManager();

	// public creation API
	void CreateSystems();
	void CreateEntity(const std::string & entityID);

	void CreateAllComponents()
	{
		try
		{
			std::unique_ptr<BaseComponent> pTransform = std::make_unique<Transform>();

			allComponents_.insert({ "Transform", std::move(pTransform) });
		}
		catch (const std::bad_alloc& e)
		{
			COM_ERROR_IF_FALSE(false, "can't allocate memory for component by ID: ");
		}
	}

	void AddComponentToEntity(const EntityID& entityID, BaseComponent* pComponent)
	{
		try
		{
			entities_.at(entityID).AddComponent(pComponent->GetComponentID())
		}
		catch (const std::out_of_range& e)
		{
			COM_ERROR_IF_FALSE(false, "there is no entity by such ID: " + entityID);
		}
	}

	// public updating API
	void Update(const double deltaTime);

	// public destroying API
	void DestroyEntity(const EntityID entityID);

public:
	//using Entities = std::map<EntityID, Entity>;
//	using Systems = ;

	std::map<std::string, Entity>  entities_;
	std::map<ComponentID, std::unique_ptr<BaseComponent>> allComponents_;
	std::vector<BaseSystem*>   systems_;
};

#elif ECS_VER_1

#include <set> 
#include <memory>

// components (ECS)
#include "../ECS_Components/Transform.h"
#include "../ECS_Components/Movement.h"
#include "../ECS_Components/MeshComponent.h"
#include "../ECS_Components/Rendered.h"

// systems (ECS)
#include "../ECS_System/TransformSystem.h"
#include "../ECS_System/MovementSystem.h"
#include "../ECS_System/MeshSystem.h"
#include "../ECS_System/RenderSystem.h"


#include "../Light/LightHelper.h"

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	// restrict any copying of instances of this class
	EntityManager(const EntityManager&) = delete;
	EntityManager(EntityManager&&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager& operator=(EntityManager&&) = delete;

	// public initialization API
	void CreateSystems();
	void CreateComponents();

	void CreateEntities(const std::vector<EntityID>& entityIDs);
	void DestroyEntity(const EntityID& entityID);

	// public updating API
	void Update(const float deltaTime);

	// public rendering API
	void Render(
		ID3D11DeviceContext* pDeviceContext,
		MeshStorage& meshStorage,
		ColorShaderClass& colorShader,
		TextureShaderClass& textureShader,
		LightShaderClass& lightShader,
		const std::vector<DirectionalLight>& dirLights,
		const std::vector<PointLight>& pointLights,
		const std::vector<SpotLight>& spotLights,
		const DirectX::XMFLOAT3& cameraPos,
		const DirectX::XMMATRIX& viewProj);

	
	// add components public API
	void AddTransformComponents(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& positions,
		const std::vector<DirectX::XMFLOAT3>& directions,
		const std::vector<DirectX::XMFLOAT3>& scales);

	void AddMovementComponents(const std::vector<EntityID>& entityIDs,
		const std::vector <DirectX::XMFLOAT3>& translations,
		const std::vector <DirectX::XMFLOAT4>& rotationQuats,
		const std::vector <DirectX::XMFLOAT3>& scaleFactors);

	void AddMeshComponents(
		const std::vector<EntityID>& entityIDs,
		const std::string& meshID);

	void AddRenderingComponents(const std::vector<EntityID>& entityIDs);


	// public query API
	inline bool HasComponent(const EntityID& entityID, const ComponentID& componentID)
	{
		// check if this entity already has such a component
		return entityToComponent_.at(entityID).contains(componentID);
	}

	inline bool HasEntity(const EntityID& entityID)
	{
		// check if there is an entity by such ID
		return entityToComponent_.contains(entityID);
	}


public:
	//using Entities = std::map<EntityID, Entity>;
	//using Components = std::vector<std::vector<Component*>>;
	//using Systems = std::vector<BaseSystem*>;

	//Entities   entities_;
	//Components allComponents_;
	//Systems    systems_;

	std::map<EntityID, std::set<ComponentID>> entityToComponent_;

	// COMPONENTS
	std::map<ComponentID, std::unique_ptr<BaseComponent>> allComponents_;

	// SYSTEMS
	TransformSystem transformSystem_;
	MovementSystem  moveSystem_;
	MeshSystem      meshSystem_;
	RenderSystem    renderSystem_;

	//std::vector<std::string>              IDs_;                      // text ID (name) of entity
	//std::vector<std::set<ComponentID>>  componentsSet_;            // each entity has its own set of components
};


#endif