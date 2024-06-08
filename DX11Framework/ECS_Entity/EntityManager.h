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
#include "../ECS_Components/WorldMatrix.h"
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
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

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
	void CreateEntities(const std::vector<EntityName>& enttsNames);
	void DestroyEntities(const std::vector<EntityName>& enttsNames);

	const EntityID GenerateIDByName(const EntityName& enttName);
	bool CreateEntity(const EntityName& enttName);
	void DestroyEntity(const EntityName& enttName);


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
		const std::vector<EntityName>& enttsNames,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,
		const std::vector<XMFLOAT3>& scales);

	void AddMoveComponent(
		const std::vector<EntityName>& enttsNames,
		const std::vector<XMFLOAT3>& translations,
		const std::vector<XMFLOAT4>& rotationQuats,
		const std::vector<XMFLOAT3>& scaleFactors);

	void AddMeshComponents(
		const std::vector<EntityName>& enttsNames,
		const std::vector<std::string>& meshesIDs);

	void AddRenderingComponents(
		const std::vector<EntityName>& enttsNames,
		const std::vector<RENDERING_SHADERS>& renderShadersTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& primTopologyArr);


	//
	// API for adding a component to a single entity 
	//
	void AddTransformComponent(
		const EntityName& enttName,
		const DirectX::XMFLOAT3& position = { 0,0,0 },
		const DirectX::XMFLOAT3& direction = { 0,0,0 },
		const DirectX::XMFLOAT3& scale = { 1,1,1 });

	void AddMoveComponent(
		const EntityName& enttName,
		const DirectX::XMFLOAT3& translation,
		const DirectX::XMFLOAT4& rotationAngles,
		const DirectX::XMFLOAT3& scaleFactor);

	void AddMeshComponent(
		const EntityName& enttName,
		const std::vector<std::string>& meshesIDs);

	void AddRenderingComponent(
		const std::vector<EntityName>& enttName,
		const std::vector<RENDERING_SHADERS>& renderShadersTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& primTopologyArr);

	
	// public query API
	void GetIDsOfEnttsByNames(
		const std::vector<EntityName>& enttsNames,
		std::vector<EntityID>& outEnttsIDs);

	const std::vector<EntityName>& GetAllEnttsNames() const;

	bool CheckEnttsByNamesExist(const std::vector<EntityName>& enttsNames);
	bool CheckIfEnttsExist(const std::vector<EntityID>& enttsIDs);

	bool CheckEnttsHaveComponent(
		const std::vector<EntityName>& enttsNames,
		const ComponentType componentType);


	const std::map<ComponentType, ComponentID>& GetPairsOfComponentTypeToName();

	
private:
	std::vector<EntityID> GenerateIDsByNames(
		const std::vector<EntityName>& enttsNames);

	void SetEnttsHasComponent(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		const ComponentType compType);

	void GetDataIdxsByNames(
		const std::vector<EntityName>& enttsNames,
		std::vector<ptrdiff_t>& outEnttsDataIdxs);

	void GetEnttsIDsByDataIdxs(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		std::vector<EntityID>& outEnttsIDs);



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

	// names of each entity
	std::vector<EntityName> names_;

	// bit flags for every component, indicating whether this object "has it"
	std::vector<uint32_t> componentFlags_;

	// pairs ['component_type' => 'component_name']
	std::map<ComponentType, ComponentID> componentTypeToName_;               
};