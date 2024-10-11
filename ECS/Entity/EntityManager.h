// **********************************************************************************
// Filename:     EntityManager.h
// Description:
// 
// Created:
// **********************************************************************************

#pragma once




#include <set> 
#include <cassert>

#include "../Common/Types.h"
//#include "../Common/log.h"

// components (ECS)
#include "../Components/Transform.h"
#include "../Components/WorldMatrix.h"
#include "../Components/Movement.h"
#include "../Components/MeshComponent.h"
#include "../Components/Rendered.h"
#include "../Components/Name.h"
#include "../Components/Textured.h"          // if entity has the Textured component it means that this entt has own textures set which is different from the meshes textures
#include "../Components/TextureTransform.h"
#include "../Components/Light.h"
#include "../Components/RenderStates.h"
#include "../Components/Bounding.h"

// systems (ECS)
#include "../Systems/TransformSystem.h"
#include "../Systems/MoveSystem.h"
#include "../Systems/MeshSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/NameSystem.h"
#include "../Systems/TexturesSystem.h"
#include "../Systems/TextureTransformSystem.h"
#include "../Systems/LightSystem.h"
#include "../Systems/RenderStatesSystem.h"
#include "../Systems/BoundingSystem.h"

namespace ECS
{

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


	// ------------------------------------------------------------------------
	// add TRANSFORM component API

	void AddTransformComponent(
		const EntityID& enttID,
		const XMFLOAT3& position = { 0,0,0 },
		const XMVECTOR& dirQuat = { 0,0,0,1 },  // no rotation by default
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
		const RENDERING_SHADERS renderShaderType = RENDERING_SHADERS::LIGHT_SHADER,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void AddRenderingComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<RENDERING_SHADERS>& renderShadersTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes);

	// ------------------------------------
	// add TEXTURED component API

	void AddTexturedComponent(
		const EntityID& enttID,
		const std::vector<TexID>& texIDs,
		const std::vector<TexPath>& texPaths);

	void AddTexturedComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<std::vector<TexID>>& texIDs,
		const std::vector<std::vector<TexPath>>& texPaths);

	// ------------------------------------
	// add TEXTURE TRANSFORM component API
	
	void AddTextureTransformComponent(
		const TexTransformType type,
		const EntityID enttID,
		const TexTransformInitParams& params);

	void AddTextureTransformComponent(
		const TexTransformType type,
		const std::vector<EntityID>& ids,
		const TexTransformInitParams& params);

	// ------------------------------------
	// add LIGHT component API

	void AddLightComponent(const std::vector<EntityID>& ids, DirLightsInitParams& params);
	void AddLightComponent(const std::vector<EntityID>& ids, PointLightsInitParams& params);
	void AddLightComponent(const std::vector<EntityID>& ids, SpotLightsInitParams& params);

	// ------------------------------------
	// add RENDER STATES component API

	void AddRenderStatesComponent(
		const std::vector<EntityID>& ids,
		const std::set<RenderStatesTypes>& states);

	void AddRenderStatesComponent(
		const std::vector<EntityID>& ids, 
		const std::vector<std::set<RenderStatesTypes>>& states);

	// ------------------------------------
	// add BOUNDING component API

	void AddBoundingComponent(
		const EntityID id,
		const DirectX::BoundingBox& data,
		const BoundingType type);

	void AddBoundingComponent(
		const std::vector<EntityID>& ids,
		const std::vector<DirectX::BoundingBox>& data,
		const std::vector<BoundingType>& types);

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

	inline const Transform& GetComponentTransform() const { return transform_; }
	inline const Movement& GetComponentMovement()   const { return movement_; }
	inline const WorldMatrix& GetComponentWorld()   const { return world_; }
	inline const MeshComponent& GetComponentMesh()  const { return meshComponent_; }
	inline const Name& GetComponentName()           const { return names_; }
	inline const Rendered& GetComponentRendered()   const { return renderComponent_; }
	inline const Textured& GetComponentTextured()   const { return textureComponent_; }
	inline const TextureTransform& GetComponentTexTransform() const { return texTransform_; }
	inline const Light& GetComponentLight()         const { return light_; }
	inline const Bounding& GetComponentBounding()   const { return bounding_; }

	inline const std::map<ComponentType, ComponentID>& GetMapCompTypeToName() {	return componentTypeToName_; }
	inline const std::vector<EntityID>& GetAllEnttsIDs() const { return ids_; }

	void CheckEnttsHaveComponents(
		const std::vector<EntityID>& ids,
		const std::vector<ComponentType>& componentsTypes,
		std::vector<bool>& outHasComponent);

	void GetComponentHashesByIDs(
		const std::vector<EntityID>& ids,
		std::vector<ComponentsHash>& componentFlags);

	void FilterInputEnttsByComponents(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<ComponentType> compTypes,
		std::vector<EntityID>& outFilteredEntts);

	void GetEnttsByComponent(
		const ComponentType componentType,
		std::vector<EntityID>& outIDs);

	bool CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs);

	inline WorldMatrix& GetWorldComponent() { return world_; }

	ComponentsHash GetHashByComponents(const std::vector<ComponentType>& components);

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


	// SYSTEMS
	LightSystem            lightSystem_;
	NameSystem             nameSystem_;
	TransformSystem        transformSystem_;
	MoveSystem             moveSystem_;
	MeshSystem             meshSystem_;
	RenderSystem           renderSystem_;
	TexturesSystem         texturesSystem_;
	TextureTransformSystem texTransformSystem_;
	RenderStatesSystem     renderStatesSystem_;
	BoundingSystem         boundingSystem_;
	

	// "ID" of an entity is just a numeral index
	std::vector<EntityID> ids_;

	// bit flags for every component, indicating whether this object "has it"
	std::vector<ComponentsHash> componentHashes_;

	// pairs ['component_type' => 'component_name']
	std::map<ComponentType, ComponentID> componentTypeToName_;  

	//ECS::Log logger_;

private:

	// COMPONENTS
	Transform        transform_;
	Movement         movement_;
	MeshComponent    meshComponent_;
	WorldMatrix      world_;
	Rendered         renderComponent_;
	Textured         textureComponent_;
	Name             names_;
	TextureTransform texTransform_;
	Light            light_;
	RenderStates     renderStates_;
	Bounding         bounding_;
};

};