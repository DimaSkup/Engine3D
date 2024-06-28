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
#include "../ECS_Components/Name.h"
#include "../ECS_Components/Textured.h"

// systems (ECS)
#include "../ECS_System/TransformSystem.h"
#include "../ECS_System/MoveSystem.h"
#include "../ECS_System/MeshSystem.h"
#include "../ECS_System/RenderSystem.h"
#include "../ECS_System/NameSystem.h"
#include "../ECS_System/TexturesSystem.h"


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

	// public serialization / deserialization API
	bool Serialize(const std::string& dataFilepath);
	bool Deserialize(const std::string& dataFilepath);

	// public creation/destroyment API
	std::vector<EntityID> CreateEntities(const size_t newEnttsCount);
	void DestroyEntities(const std::vector<EntityID>& enttsIDs);

	EntityID CreateEntity();
	//void DestroyEntity(const EntityName& enttName);

	// public updating API
	void Update(const float deltaTime);

	// public rendering API
	void GetRenderingDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outWorldMatrices,
		std::vector<ECS::RENDERING_SHADERS>& outShaderTypes,
		std::vector<MeshID>& outMeshesIDs,
		std::vector<std::set<EntityID>>& outEnttsByMeshes);

	//
	// API for adding a component to batch of entities
	//
	void SetEnttsHaveComponent(
		const std::vector<EntityID>& enttsIDs,        // set by entts IDs
		const ComponentType compType);

	void SetEnttsHaveComponent(
		const std::vector<ptrdiff_t>& enttsDataIdxs,  // set by data idxs of entities
		const ComponentType componentType);

	void AddNameComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<EntityName>& enttsNames);

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

	void AddMeshComponent(
		const std::vector<EntityID>& enttID,
		const std::vector<size_t>& meshesIDs);

	void AddRenderingComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<ECS::RENDERING_SHADERS>& renderShadersTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes);

	void AddTextureComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<std::vector<TextureID>>& textures);


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
		const std::vector<size_t>& meshesIDs);

	void AddRenderingComponent(
		const EntityID& enttID,
		const ECS::RENDERING_SHADERS renderShaderType,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType);

	
	// public query API
	inline const std::map<ComponentType, ComponentID>& GetPairsOfComponentTypeToName()
	{
		return componentTypeToName_;
	}

	const std::vector<EntityID>& GetAllEnttsIDs() const;


	void GetComponentFlagsByIDs(
		const std::vector<EntityID>& ids,
		std::vector<ComponentFlagsType>& componentFlags);

	void GetEnttsByComponent(
		const ComponentType componentType,
		std::vector<EntityID>& outIDs);

	// check existing
	bool CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs);

	
private:
	void GenerateIDs(
		const size_t newEnttsCount,
		std::vector<EntityID>& outGeneratedIDs);

	void GetDataIdxsByIDs(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ptrdiff_t>& outDataIdxs);

	void GetEnttsIDsByDataIdxs(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		std::vector<EntityID>& outEnttsIDs);

	bool CheckEnttsByDataIdxsHaveComponent(
		const std::vector<ptrdiff_t>& enttsDataIdxs,
		const ComponentType componentType);


public:
	static const size_t ENTT_MGR_SERIALIZE_DATA_BLOCK_MARKER = 1000;

	// COMPONENTS
	Name names_;
	Transform transform_;
	WorldMatrix world_;
	Movement movement_;
	MeshComponent meshComponent_;
	Rendered renderComponent_;
	Textured textureComponent_;

	// SYSTEMS
	NameSystem nameSystem_;
	TransformSystem transformSystem_;
	MoveSystem moveSystem_;
	MeshSystem meshSystem_;
	RenderSystem renderSystem_;
	TexturesSystem texturesSystem_;

	// "ID" of an entity is just a numeral index
	std::vector<EntityID> ids_;

	// bit flags for every component, indicating whether this object "has it"
	std::vector<ComponentFlagsType> componentFlags_;

	// pairs ['component_type' => 'component_name']
	std::map<ComponentType, ComponentID> componentTypeToName_;               
};