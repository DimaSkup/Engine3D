// **********************************************************************************
// Filename:     EntityManager.h
// Description:
// 
// Created:
// **********************************************************************************

#pragma once


#include "../ECS_Common/ECS_Types.h"

#include <set> 
#include <unordered_map>
#include <memory>

// components (ECS)
#include "../ECS_Components/Transform.h"
#include "../ECS_Components/WorldMatrix.h"
#include "../ECS_Components/Movement.h"
#include "../ECS_Components/MeshComponent.h"
#include "../ECS_Components/Rendered.h"

// systems (ECS)
#include "../ECS_System/TransformSystem.h"
#include "../ECS_System/MoveSystem.h"
#include "../ECS_System/MeshSystem.h"
#include "../ECS_System/RenderSystem.h"

class EntityManager final
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
	void CreateEntities(const size_t newEnttsCount);
	void DestroyEntities(const std::vector<EntityID>& enttsIDs);

	//void CreateEntity(const EntityName& enttName);
	//void DestroyEntity(const EntityName& enttName);


	// public updating API
	void Update(const float deltaTime);

	// public rendering API
	void GetRenderingDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outWorldMatrices,
		std::vector<RENDERING_SHADERS>& outShaderTypes);

	//
	// API for adding a component to batch of entities
	//

	void AddTransformComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,
		const std::vector<XMFLOAT3>& scales);

	void AddMoveComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& translations,
		const std::vector<XMFLOAT4>& rotationQuats,
		const std::vector<XMFLOAT3>& scaleFactors);

	void AddMeshComponents(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<std::string>& meshesIDs);

	void AddRenderingComponents(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<RENDERING_SHADERS>& renderShadersTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes);


	//
	// API for adding a component to a single entity 
	//
	void AddTransformComponent(
		const EntityID& enttID,
		const XMFLOAT3& position = { 0,0,0 },
		const XMFLOAT3& direction = { 0,0,0 },
		const XMFLOAT3& scale = { 1,1,1 });

	void AddMoveComponent(
		const EntityID& enttID,
		const XMFLOAT3& translation,
		const XMFLOAT4& rotationAngles,
		const XMFLOAT3& scaleFactor);

	void AddMeshComponent(
		const EntityID& enttID,
		const std::vector<std::string>& meshesIDs);

	void AddRenderingComponent(
		const EntityID& enttID,
		const RENDERING_SHADERS renderShadersTypes,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType);

	
	// public query API
	//const std::map<ComponentType, ComponentID>& GetPairsOfComponentTypeToName();

	const std::vector<EntityID>& GetAllEnttsIDs() const;

	// check existing
	bool CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs);

	// check component related stuff
	bool CheckEnttsByIDsHaveComponent(
		const std::vector<EntityID>& enttsIDs,
		const ComponentType componentType);

	

	
private:
	std::vector<EntityID> GenerateIDs(const size_t newEnttsCount);

	void GetDataIdxsByIDs(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ptrdiff_t>& outDataIdxs);

	void GetEnttsIDsByDataIdxs(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		std::vector<EntityID>& outEnttsIDs);

	void SetEnttsHaveComponent(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		const ComponentType componentType);

	bool CheckEnttsByDataIdxsHaveComponent(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		const ComponentType componentType);



public:
	// COMPONENTS
	Transform transform_;
	WorldMatrix world_;
	Movement movement_;
	MeshComponent meshComponent_;
	Rendered renderComponent_;

	// SYSTEMS
	TransformSystem transformSystem_;
	MoveSystem moveSystem_;
	MeshSystem meshSystem_;
	RenderSystem renderSystem_;

	// "ID" of an entity is just a numeral index
	std::vector<EntityID> ids_;

	// bit flags for every component, indicating whether this object "has it"
	std::vector<uint32_t> componentFlags_;

	// pairs ['component_type' => 'component_name']
	std::map<ComponentType, ComponentID> componentTypeToName_;               
};