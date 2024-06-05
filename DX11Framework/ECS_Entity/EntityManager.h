// **********************************************************************************
// Filename:     EntityManager.h
// Description:
// 
// Created:
// **********************************************************************************

#pragma once


#include "ECS_Types.h"

#include <set> 
#include <unordered_map>
#include <memory>

// components (ECS)
#include "../ECS_Components/Transform.h"
#include "../ECS_Components/Movement.h"
#include "../ECS_Components/MeshComponent.h"
#include "../ECS_Components/Rendered.h"

// systems (ECS)
#include "../ECS_System/TransformSystem.h"
#include "../ECS_System/MoveSystem.h"
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

	void Serialize();
	void Deserialize();

	// public creation/destroyment API
	bool CreateEntity(const EntityID& entityID);
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
	void AddTransformComponent(
		const EntityID& entityID,
		const DirectX::XMFLOAT3& position = { 0,0,0 },
		const DirectX::XMFLOAT3& direction = { 0,0,0 },
		const DirectX::XMFLOAT3& scale = { 1,1,1 });

	void AddTransformComponent(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& positions,
		const std::vector<DirectX::XMFLOAT3>& directions,
		const std::vector<DirectX::XMFLOAT3>& scales);

	void AddMoveComponent(
		const EntityID& entityID,
		const DirectX::XMFLOAT3& translation,
		const DirectX::XMFLOAT4& rotationAngles,
		const DirectX::XMFLOAT3& scaleFactor);

	void AddMoveComponent(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& translations,
		const std::vector<DirectX::XMFLOAT4>& rotationQuats,
		const std::vector<DirectX::XMFLOAT3>& scaleFactors);

	void AddMeshComponents(
		const std::vector<EntityID>& entityIDs,
		const std::vector<std::string>& meshesIDs);

	void AddRenderingComponents(const std::vector<EntityID>& entityIDs);

	
	// public query API
	const std::set<EntityID> GetAllEntitiesIDs() const;
	bool CheckEntitiesExist(const std::vector<EntityID>& entitiesIDs);

	// INLINE public query API
	inline bool CheckEntityHasComponent(const EntityID& entityID, const ComponentType componentType)
	{
		return entityToComponent_.at(entityID).contains(componentType);
	}

	inline bool CheckEntityExist(const EntityID& entityID) 
	{ 
		return entityToComponent_.contains(entityID); 
	}

	inline const std::map<ComponentType, ComponentID>& GetPairsOfComponentTypeToName()
	{ 
		return componentTypeToName_; 
	}

	
private:
	std::string GetStringOfEntitiesIDs(const std::vector<EntityID>& entitiesIDs);

public:
	// COMPONENTS
	Transform transform_;
	Movement movement_;
	MeshComponent meshComponent_;
	Rendered renderComponent_;

	// SYSTEMS
	TransformSystem transformSystem_;
	MoveSystem MoveSystem_;
	MeshSystem meshSystem_;
	RenderSystem renderSystem_;

	std::map<ComponentType, ComponentID> componentTypeToName_;                // pairs ['component_type' => 'component_name']
	std::unordered_map<EntityID, std::set<ComponentType>> entityToComponent_; // pairs ['entity_id' => 'set_of_added_components']
};

//#include "EntityManagerInlineFunc.inl"
