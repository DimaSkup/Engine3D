// **********************************************************************************
// Filename:     EntityManager.h
// Description:
// 
// Created:
// **********************************************************************************

#pragma once


#include "../Common/Types.h"

#include <set> 

// components (ECS)
#include "../Components/Transform.h"
#include "../Components/WorldMatrix.h"
#include "../Components/Movement.h"
#include "../Components/MeshComponent.h"
#include "../Components/Rendered.h"
#include "../Components/Name.h"
#include "../Components/Textured.h"          // if entity has the Textured component it means that this entt has own textures set which is different from the meshes textures
#include "../Components/TextureTransform.h"

// systems (ECS)
#include "../Systems/TransformSystem.h"
#include "../Systems/MoveSystem.h"
#include "../Systems/MeshSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/NameSystem.h"
#include "../Systems/TexturesSystem.h"
#include "../Systems/TextureTransformSystem.h"

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
	std::vector<EntityID> CreateEntities(const u32 newEnttsCount);
	void DestroyEntities(const std::vector<EntityID>& enttsIDs);

	EntityID CreateEntity();
	//void DestroyEntity(const EntityName& enttName);


	void Update(const float totalGameTime, const float deltaTime);


	// public rendering API
	void GetRenderingDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outWorldMatrices,
		std::vector<ECS::RENDERING_SHADERS>& outShaderTypes,
		std::vector<MeshID>& outMeshesIDs,
		std::vector<std::set<EntityID>>& outEnttsByMeshes);


	// ------------------------------------------------------------------------
	// add TRANSFORM component API

	void AddTransformComponent(
		const EntityID& enttID,
		const XMFLOAT3& position = { 0,0,0 },
		const XMVECTOR& dirQuat = { 0,0,0,0 },
		const float uniformScale = 1.0f);

	void AddTransformComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMVECTOR>& dirQuats,
		const std::vector<float>& uniformScales);

	// ------------------------------------
	// add RENDERED component API

	void AddMoveComponent(
		const EntityID& enttID,
		const XMFLOAT3& translation,
		const XMVECTOR& rotationQuat,
		const float uniformScaleFactor);

	void AddMoveComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& translations,
		const std::vector<XMVECTOR>& rotationQuats,
		const std::vector<float>& uniformScaleFactors);

	// ------------------------------------
	// add NAME component API

	void AddNameComponent(
		const EntityID& enttID,
		const EntityName& enttName);

	void AddNameComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<EntityName>& enttsNames);

	// ------------------------------------
	// add MESH component API

	void AddMeshComponent(
		const EntityID enttID,
		const u32 meshesIDs);

	void AddMeshComponent(
		const EntityID enttID,
		const std::vector<u32>& meshesIDs);

	void AddMeshComponent(
		const std::vector<EntityID>& enttID,
		const std::vector<u32>& meshesIDs);

	// ------------------------------------
	// add RENDERED component API

	void AddRenderingComponent(
		const std::vector<EntityID>& enttsIDs,
		const ECS::RENDERING_SHADERS renderShaderType = ECS::RENDERING_SHADERS::LIGHT_SHADER,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void AddRenderingComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<ECS::RENDERING_SHADERS>& renderShadersTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes);

	// ------------------------------------
	// add TEXTURED component API


	void AddTexturedComponent(
		const EntityID& enttID,
		const TexIDsArr& texIDs,
		const TexPathsArr& texPaths);

	void AddTexturedComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<TexIDsArr>& texIDs,
		const std::vector<TexPathsArr>& texPaths);

	// ------------------------------------
	// add TEXTURE TRANSFORM component API
	
	void AddTextureTransformComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMMATRIX>& texTransform);

	void AddTextureTransformComponent(
		const EntityID enttID,
		const u32 textureRows,
		const u32 textureColumns,
		const float animDuration);

	void AddTextureTransformComponentRotationAroundTexCoord(
		const EntityID enttID,
		const float tu,
		const float tv,
		const float rotationSpeed);

	// ------------------------------------
	// components SETTERS API

	void SetEnttsHaveComponent(
		const std::vector<EntityID>& enttsIDs,        // set by entts IDs
		const ComponentType compType);

	void SetEnttsHaveComponent(
		const std::vector<ptrdiff_t>& enttsDataIdxs,  // set by data idxs of entities
		const ComponentType componentType);


	// ---------------------------------------------------------------------------
	// public QUERY API
	inline const std::map<ComponentType, ComponentID>& GetPairsOfComponentTypeToName()
	{
		return componentTypeToName_;
	}

	const std::vector<EntityID>& GetAllEnttsIDs() const;

	void GetComponentFlagsByIDs(
		const std::vector<EntityID>& ids,
		std::vector<ComponentFlagsType>& componentFlags);

	void FilterInputEnttsByComponents(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<ComponentType> compTypes,
		std::vector<EntityID>& outFilteredEntts);

	void GetEnttsByComponent(
		const ComponentType componentType,
		std::vector<EntityID>& outIDs);

	bool CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs);


	inline WorldMatrix& GetWorldComponent() { return world_; }

	
private:
	void GenerateIDs(
		const u32 newEnttsCount,
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
	static const u32 ENTT_MGR_SERIALIZE_DATA_BLOCK_MARKER = 1000;

	// COMPONENTS
	Name names_;
	Transform transform_;
	WorldMatrix world_;
	Movement movement_;
	MeshComponent meshComponent_;
	Rendered renderComponent_;
	Textured textureComponent_;
	TextureTransform texTransform_;

	// SYSTEMS
	NameSystem nameSystem_;
	TransformSystem transformSystem_;
	MoveSystem moveSystem_;
	MeshSystem meshSystem_;
	RenderSystem renderSystem_;
	TexturesSystem texturesSystem_;
	TextureTransformSystem texTransformSystem_;

	// "ID" of an entity is just a numeral index
	std::vector<EntityID> ids_;

	// bit flags for every component, indicating whether this object "has it"
	std::vector<ComponentFlagsType> componentFlags_;

	// pairs ['component_type' => 'component_name']
	std::map<ComponentType, ComponentID> componentTypeToName_;               
};