#include "EntityManager.h"

#include "../Common/Utils.h"
#include "../Common/Assert.h"
#include "../Common/Log.h"

#include "EntityManagerSerializer.h"
#include "EntityManagerDeserializer.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <cctype>
#include <random>

using namespace Utils;

namespace ECS
{


EntityManager::EntityManager() :
	nameSystem_ {&names_},
	transformSystem_{ &transform_, &world_ },
	moveSystem_{ &transform_, &world_, &movement_ },
	meshSystem_{ &meshComponent_ },
	renderSystem_{ &renderComponent_, &transform_, &world_, &meshComponent_ },
	texturesSystem_{ &textureComponent_ },
	texTransformSystem_ { &texTransform_ },
	lightSystem_{ &light_ },
	renderStatesSystem_{ &renderStates_ },
	boundingSystem_ { &bounding_ }
{
	const u32 reserveMemForEnttsCount = 100;

	ids_.reserve(reserveMemForEnttsCount);
	componentFlags_.reserve(reserveMemForEnttsCount);

	// make pairs ['component_type' => 'component_name']
	componentTypeToName_ =
	{
		{ ComponentType::TransformComponent, "Transform" },
		{ ComponentType::NameComponent, "Name" },
		{ ComponentType::MoveComponent, "Movement" },
		{ ComponentType::MeshComp, "MeshComponent" },
		{ ComponentType::RenderedComponent, "Rendered" },
		{ ComponentType::WorldMatrixComponent, "WorldMatrix" }
	};
}

EntityManager::~EntityManager()
{
	//Log::Debug(LOG_MACRO);
}


// ************************************************************************************
//                 PUBLIC SERIALIZATION / DESERIALIZATION API
// ************************************************************************************

bool EntityManager::Serialize(const std::string& dataFilepath)
{
	try
	{
		EntityManagerSerializer serializer;
		serializer.Serialize(*this, dataFilepath);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't serialize data into a file: " + dataFilepath);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool EntityManager::Deserialize(const std::string& dataFilepath)
{
	try
	{
		EntityManagerDeserializer deserializer;
		deserializer.Deserialize(*this, dataFilepath);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't deserialize data from the file: " + dataFilepath);
		return false;
	}

	return true;
}



// ************************************************************************************
//                     PUBLIC CREATION/DESTROYMENT API
// ************************************************************************************

#pragma region PublicCreationDestroymentAPI

EntityID EntityManager::CreateEntity()
{
	// create a new entity;
	// return: a generated ID of this new entity

	return CreateEntities(1).front();
}

///////////////////////////////////////////////////////////

std::vector<EntityID> EntityManager::CreateEntities(const u32 newEnttsCount)
{
	// create batch of new empty entities, generate for each entity 
	// unique ID and set that it hasn't any component by default;
	//
	// return: SORTED array of IDs of just created entities;

	Assert::NotZero(newEnttsCount, "new entitites count == 0");

	std::vector<EntityID> generatedIDs;
	GenerateIDs(newEnttsCount, generatedIDs);

	for (const EntityID& ID : generatedIDs)
	{
		const ptrdiff_t insertAtPos = Utils::GetPosForID(ids_, ID);

		// add new ID into the sorted array of IDs
		// and set that each new entity by default doesn't have any component
		Utils::InsertAtPos<EntityID>(ids_, insertAtPos, ID);		
		Utils::InsertAtPos<u32>(componentFlags_, insertAtPos, 0);
	}

	return generatedIDs;
}

///////////////////////////////////////////////////////////

void EntityManager::DestroyEntities(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}


#pragma endregion

// ************************************************************************************
//                          PUBLIC UPDATING FUNCTIONS
// ************************************************************************************

void EntityManager::Update(const float totalGameTime, const float deltaTime)
{
	moveSystem_.UpdateAllMoves(deltaTime, transformSystem_);
	texTransformSystem_.UpdateAllTextrureAnimations(totalGameTime, deltaTime);
	lightSystem_.Update(deltaTime, totalGameTime);
}

// *********************************************************************************
// 
//                     ADD COMPONENTS PUBLIC FUNCTIONS
// 
// *********************************************************************************

#pragma region AddComponentsAPI

void EntityManager::FilterInputEnttsByComponents(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<ComponentType> compTypes,
	std::vector<EntityID>& outFilteredEntts)
{
	ComponentFlagsType bitmask = 0;
	std::vector<ComponentFlagsType> componentFlags;

	// create a bitmask
	for (const ComponentType& type : compTypes)
		bitmask |= (1 << type);


	// get all the component flags of input entities
	GetComponentFlagsByIDs(enttsIDs, componentFlags);

	outFilteredEntts.reserve(enttsIDs.size());

	// if the entity has such set of components we store this entity ID
	for (u32 idx = 0; idx < enttsIDs.size(); ++idx)
	{
		if (bitmask == (componentFlags[idx] & bitmask))
			outFilteredEntts.push_back(enttsIDs[idx]);
	}

	outFilteredEntts.shrink_to_fit();
}

///////////////////////////////////////////////////////////

void EntityManager::SetEnttsHaveComponent(
	const std::vector<EntityID>& enttsIDs,
	const ComponentType compType)
{
	// go through each entity and set that this entity has the component by type;
	// input: 1. array of entities IDs
	//        2. type of the component (numeral value)

	std::vector<ptrdiff_t> dataIdxs;

	GetDataIdxsByIDs(enttsIDs, dataIdxs);
	SetEnttsHaveComponent(dataIdxs, compType);
}

///////////////////////////////////////////////////////////

void EntityManager::SetEnttsHaveComponent(
	const std::vector<ptrdiff_t>& enttsDataIdxs,
	const ComponentType compType)
{
	// go through each input entity and set that it has the component by type;
	// 
	// input: 1. enttsDataIdxs -- array of data indices 
	//           (you can receive it using the GetDataIdxsByNames function)
	//        2. type of the component (numeral value)

	u32 bitmask = (1 << compType);

	for (const ptrdiff_t idx : enttsDataIdxs)
		componentFlags_[idx] |= bitmask;
}

///////////////////////////////////////////////////////////

void EntityManager::AddNameComponent(
	const EntityID& enttID,
	const EntityName& enttName)
{
	// add the Name component to a single entity in terms of arrays
	AddNameComponent(
		std::vector<EntityID>{enttID},
		std::vector<EntityName>{enttName});
}

///////////////////////////////////////////////////////////

void EntityManager::AddNameComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<EntityName>& enttsNames)
{	
	// add the Name component to all the input entities
	// so each entity will have its own name
	try
	{
		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		Assert::NotZero(enttsCount, "array of entities IDs is empty");
		Assert::True(enttsCount == enttsNames.size(), "count of entities IDs and names must be equal");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::NameComponent);

		nameSystem_.AddRecords(enttsIDs, enttsNames);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		throw LIB_Exception("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTransformComponent(
	const EntityID& enttID,
	const XMFLOAT3& position,
	const XMVECTOR& dirQuat,
	const float uniformScale)
{
	// add the Transform component to a single entity in terms of arrays
	AddTransformComponent(
		std::vector<EntityID>{enttID},
		std::vector<XMFLOAT3>{position},
		std::vector<XMVECTOR>{dirQuat},
		std::vector<float>{uniformScale});
}

///////////////////////////////////////////////////////////

void EntityManager::AddTransformComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMVECTOR>& dirQuats,
	const std::vector<float>& uniformScales)
{
	// add transform component to all the input entities

	try
	{
		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		Assert::NotZero(enttsCount, "array of entities IDs is empty");
		Assert::True(enttsCount == std::ssize(positions), "count of entities and positions must be equal");
		Assert::True(enttsCount == std::ssize(dirQuats), "count of entities and directions must be equal");
		Assert::True(enttsCount == std::ssize(uniformScales), "count of entities and scales must be equal");


		std::vector<ptrdiff_t> enttsDataIdxs;
		
		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::TransformComponent);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::WorldMatrixComponent);

		transformSystem_.AddRecords(enttsIDs, positions, dirQuats, uniformScales);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const EntityID& enttID,
	const XMFLOAT3& translation,
	const XMVECTOR& rotationQuat,
	const float uniformScaleFactor)
{
	// add the Move component to a single entity in terms of arrays
	AddMoveComponent(
		std::vector<EntityID>{enttID},
		std::vector<XMFLOAT3>{translation},
		std::vector<XMVECTOR>{rotationQuat},
		std::vector<float>{uniformScaleFactor});
}

///////////////////////////////////////////////////////////

void EntityManager::AddMoveComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMVECTOR>& rotationQuats,
	const std::vector<float>& uniformScaleFactors)
{
	// add the Move component to all the input entities;
	// and setup entities movement using input data arrays

	try
	{
		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		Assert::NotZero(enttsCount, "array of entities IDs is empty");
		Assert::True(enttsCount == translations.size(), "count of entities and translations must be equal");
		Assert::True(enttsCount == rotationQuats.size(), "count of entities and rotationQuats must be equal");
		Assert::True(enttsCount == uniformScaleFactors.size(), "count of entities and scaleFactors must be equal");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MoveComponent);

		moveSystem_.AddRecords(
			enttsIDs, 
			translations,
			rotationQuats,
			uniformScaleFactors);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponent(
	const EntityID enttID,
	const u32 meshID)
{
	// add the Mesh component to a single entity by ID in terms of arrays;

	AddMeshComponent(
		std::vector<EntityID>{enttID},
		std::vector<u32>{meshID});
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponent(
	const EntityID enttID,
	const std::vector<u32>& meshesIDs) 
{
	// add the Mesh component to a single entity by ID in terms of arrays;
	// input: 1. an entity ID
	//        2. a batch of meshes IDs

	AddMeshComponent(
		std::vector<EntityID>{enttID},
		meshesIDs);
}

///////////////////////////////////////////////////////////

void EntityManager::AddMeshComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<u32>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to each input entity all the meshes IDs from the input array

	try
	{
		Assert::NotEmpty(enttsIDs.empty(), "the array of entities IDs is empty");
		Assert::NotEmpty(meshesIDs.empty(), "the array of meshes IDs is empty");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MeshComp);

		meshSystem_.AddRecords(enttsIDs, meshesIDs);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponent(
	const std::vector<EntityID>& enttsIDs,
	const ECS::RENDERING_SHADERS renderShaderType,
	const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	// add the Rendered component to each input entity by ID
	// and setup them with the same rendering params 

	AddRenderingComponent(
		enttsIDs,
		std::vector<ECS::RENDERING_SHADERS>(enttsIDs.size(), renderShaderType),
		std::vector<D3D11_PRIMITIVE_TOPOLOGY>(enttsIDs.size(), topologyType));
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderingComponent(
	const std::vector<EntityID>& ids,
	const std::vector<ECS::RENDERING_SHADERS>& shadersTypes,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	try
	{
		Assert::NotEmpty(ids.empty(), "the array of entities IDs is empty");
		Assert::True(std::ssize(ids) == shadersTypes.size(), "entities count != count of the input shaders types");
		Assert::True(std::ssize(ids) == topologyTypes.size(), "entities count != count of the input primitive topoECS::Logy types");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(ids, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::RenderedComponent);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::RenderStatesComponent);

		// each entt by default must have particular rendering states
		const std::set<RENDER_STATES> defaultStates
		{
			FILL_MODE_SOLID, 
			CULL_MODE_BACK,
			NO_BLENDING,
			NO_ALPHA_CLIPPING
		};

		const std::vector<std::set<RENDER_STATES>> statesForEachEntt(std::ssize(ids), defaultStates);

		renderSystem_.AddRecords(ids, shadersTypes, topologyTypes);
		renderStatesSystem_.AddOrUpdate(ids, statesForEachEntt);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(ids));
		throw LIB_Exception("can't add a rendering component");
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(ids));
		throw LIB_Exception("can't add a rendering component");
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTexturedComponent(
	const EntityID& enttID,
	const TexIDsArr& texIDs,
	const TexPathsArr& texPaths)
{
	AddTexturedComponent(std::vector<EntityID>{ enttID }, { texIDs }, { texPaths });
}

///////////////////////////////////////////////////////////

void EntityManager::AddTexturedComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<TexIDsArr>& texIDs,        // array of IDs arrays
	const std::vector<TexPathsArr>& texPaths)    // array of paths arrays
{
	// add Textured component to each input entity by its ID
	// and set that this entity has own textures set 
	// (which is different from its mesh's textures set)

	try
	{
		// check input arguments

		Assert::NotEmpty(enttsIDs.empty(), "the array of entities IDs is empty");
		Assert::True(std::ssize(enttsIDs) == std::ssize(texIDs), "entities count != textures IDs arrays count");
		Assert::True(std::ssize(enttsIDs) == std::ssize(texPaths), "entities count != textures paths arrays count");

		bool texIDsAreOk = true;
		bool texPathsAreOk = true;

		for (const TexIDsArr& idsArr : texIDs)
			texIDsAreOk &= (std::ssize(idsArr) == (size)Textured::TEXTURES_TYPES_COUNT);

		for (const TexPathsArr& pathsArr : texPaths)
		{
			// do we have proper number of paths in the array?
			texPathsAreOk &= (std::ssize(pathsArr) == (size)Textured::TEXTURES_TYPES_COUNT);

			// check if each path is not empty
			for (const TexPath& path : pathsArr)
				texPathsAreOk &= (!path.empty());
		}

		Assert::True(texIDsAreOk, "the textures IDs data is INVALID (wrong number of)");
		Assert::True(texPathsAreOk, "the textures paths data is INVALID (wrong number of / some path is empty)");

		// ------------------------------------------------

		SetEnttsHaveComponent(enttsIDs, ComponentType::TexturedComponent);
		texturesSystem_.AddRecords(enttsIDs, texIDs, texPaths);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't add the Textured component to entities by IDs: " + JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTextureTransformComponent(
	const TexTransformType type,
	const EntityID id,
	const TexTransformInitParams& params)
{
	AddTextureTransformComponent(
		type, 
		std::vector<EntityID>{id},
		params);
}

///////////////////////////////////////////////////////////

void EntityManager::AddTextureTransformComponent(
	const TexTransformType type,
	const std::vector<EntityID>& ids,
	const TexTransformInitParams& inParams)
{
	// set texture transformation of input type for each input entity by ID
	//
	// in:   type     -- what kind of texture transformation we want to apply?
	//       inParams -- struct of arrays of texture transformations params according to the input type

	try
	{
		Assert::NotEmpty(ids.empty(), "the array of entities IDs is empty");
	
		SetEnttsHaveComponent(ids, ComponentType::TextureTransformComponent);
		texTransformSystem_.AddTexTransformation(type, ids, inParams);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't add the TextureTransform component"
			       "to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(ids));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddLightComponent(
	const std::vector<EntityID>& ids,
	DirLightsInitParams& params)
{
	try
	{
		Assert::NotEmpty(ids.empty(), "the array of entities IDs is empty");
		Assert::True(CheckEnttsByIDsExist(ids), "the entity mgr doesn't have an entity by some of input ids");

		SetEnttsHaveComponent(ids, ComponentType::LightComponent);
		lightSystem_.AddDirLights(ids, params);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't add the Light component (directional lights)"
			"to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(ids));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddLightComponent(
	const std::vector<EntityID>& ids,
	PointLightsInitParams& params)
{
	try
	{
		Assert::NotEmpty(ids.empty(), "the array of entities IDs is empty");
		Assert::True(CheckEnttsByIDsExist(ids), "the entity mgr doesn't have an entity by some of input ids");

		SetEnttsHaveComponent(ids, ComponentType::LightComponent);
		lightSystem_.AddPointLights(ids, params);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't add the Light component (point lights)"
			"to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(ids));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddLightComponent(
	const std::vector<EntityID>& ids,
	SpotLightsInitParams& params)
{
	try
	{
		Assert::NotEmpty(ids.empty(), "the array of entities IDs is empty");
		Assert::True(CheckEnttsByIDsExist(ids), "the entity mgr doesn't have an entity by some of input ids");

		SetEnttsHaveComponent(ids, ComponentType::LightComponent);
		lightSystem_.AddSpotLights(ids, params);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't add the Light component (spot lights)"
			"to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(ids));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderStatesComponent(
	const std::vector<EntityID>& ids,
	const std::set<RENDER_STATES>& states)
{
	AddRenderStatesComponent(ids, std::vector<std::set<RENDER_STATES>>{states});
}

///////////////////////////////////////////////////////////

void EntityManager::AddRenderStatesComponent(
	const std::vector<EntityID>& ids,
	const std::vector<std::set<RENDER_STATES>>& states)
{
	try
	{
		Assert::NotEmpty(ids.empty(), "the input arr of entities IDs is empty");
		Assert::True(CheckEnttsByIDsExist(ids), "the entity mgr doesn't have an entity by some of input ids");
		Assert::True(CheckArrSizesEqual(ids, states), "count of ids != count of blend states");

		SetEnttsHaveComponent(ids, ComponentType::RenderStatesComponent);
		renderStatesSystem_.AddOrUpdate(ids, states);
	}
	catch (LIB_Exception& e)
	{		
		Log::Error(e);
		Log::Error("can't add the component to entities: " + Utils::JoinArrIntoStr<EntityID>(ids));
		throw LIB_Exception("can't add a render state component");
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddBoundingComponent(
	const EntityID id,
	const DirectX::BoundingBox& data,
	const BoundingType type)
{
	AddBoundingComponent(std::vector<EntityID>{id}, { data }, { type });
}

///////////////////////////////////////////////////////////

void EntityManager::AddBoundingComponent(
	const std::vector<EntityID>& ids,
	const std::vector<DirectX::BoundingBox>& data,
	const std::vector<BoundingType>& types)
{
	try
	{
		Assert::NotEmpty(ids.empty(), "the array of entities IDs is empty");
		Assert::True(CheckEnttsByIDsExist(ids), "the entity mgr doesn't have an entity by some of input ids");
		Assert::True(CheckArrSizesEqual(ids, data), "count of ids != count of data elems");
		Assert::True(CheckArrSizesEqual(ids, types), "count of ids != count of types");

		SetEnttsHaveComponent(ids, ComponentType::BoundingComponent);
		boundingSystem_.Add(ids, data, types);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error("can't add the component to entities: " + Utils::JoinArrIntoStr<EntityID>(ids));
		throw LIB_Exception("can't add the component");
	}
}


#pragma endregion


// ************************************************************************************
//                            PUBLIC QUERY API
// ************************************************************************************

#pragma region PublicQueryAPI

bool EntityManager::CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs)
{
	// check by ID if each entity from the input array is created;
	// return: true  -- if all the entities from the input arr exists in the manager
	//         false -- if some entity from the input arr doesn't exist

	bool allExist = true;

	for (const EntityID& id : enttsIDs)
		allExist &= std::binary_search(ids_.begin(), ids_.end(), id);
	
	return allExist;
}

///////////////////////////////////////////////////////////

void EntityManager::GetComponentFlagsByIDs(
	const std::vector<EntityID>& ids,
	std::vector<ComponentFlagsType>& outFlags)
{
	// get component bitmasks of entities by ID

	std::vector<ptrdiff_t> dataIdxs;
	GetDataIdxsByIDs(ids, dataIdxs);

	outFlags.reserve(ids.size());

	for (const ptrdiff_t idx : dataIdxs)
		outFlags.push_back(componentFlags_[idx]);
}

///////////////////////////////////////////////////////////

void EntityManager::GetEnttsByComponent(
	const ComponentType componentType,
	std::vector<EntityID>& outIDs)
{
	// get IDs of entities which have such component;

	switch (componentType)
	{
		case ComponentType::TransformComponent:
		{
			outIDs = transform_.ids_;
			return;
		}
		case ComponentType::WorldMatrixComponent:
		{
			transformSystem_.GetAllEnttsIDsFromWorldMatrixComponent(outIDs);
			return;
		}
		case ComponentType::MoveComponent:
		{
			outIDs = movement_.ids_;
			return;
		}
		case ComponentType::MeshComp:
		{
			meshSystem_.GetEnttsIDsFromMeshComponent(outIDs);
			return;
		}
		case ComponentType::RenderedComponent:
		{
			outIDs = renderComponent_.ids_;
			return;
		}
		default:
		{
			Log::Error("Unknown component type: " + std::to_string(componentType));
			throw LIB_Exception("can't get IDs of entities which have such component: " + std::to_string(componentType));
		}
	}
}

#pragma endregion




// ************************************************************************************
// 
//                               PRIVATE HELPERS
// 
// ************************************************************************************


void EntityManager::GenerateIDs(
	const u32 newEnttsCount,
	std::vector<EntityID>& outGeneratedIDs)
{
	// generate unique IDs in quantity newEnttsCount;
	// 
	// in:  how many entities we will create
	// out: SORTED array of generated entities IDs

	using engine = std::mt19937;

	std::random_device os_seed;
	const u32 seed = os_seed();
	engine generator(seed);
	std::uniform_int_distribution<u32> distribute(0, UINT_MAX);

	outGeneratedIDs.reserve(newEnttsCount);

	// generate an ID for each new entity
	for (u32 idx = 0; idx < newEnttsCount; ++idx)
	{
		u32 id = distribute(generator);

		// if such ID already exists we generate new id value
		while (std::binary_search(ids_.begin(), ids_.end(), id))
		{
			id = distribute(generator);
		}

		outGeneratedIDs.push_back(id);
	}

	// after generation we sort IDs so then it will be faster to store them
	std::sort(outGeneratedIDs.begin(), outGeneratedIDs.end());
}

///////////////////////////////////////////////////////////

void EntityManager::GetDataIdxsByIDs(
	const std::vector<EntityID>& enttsIDs,
	std::vector<ptrdiff_t>& outDataIdxs)
{
	// get an index into data arrays for each input entity name;
	// since all the IDs are sorted the output arr will have sorted idxs;
	// 
	// in:  array of entities IDs
	// out: array of data idxs


	// check if these entities exist
	bool enttsValid = true;

	for (const EntityID& id : enttsIDs)
		enttsValid &= BinarySearch(ids_, id);

	Assert::True(enttsValid, "there is no entity by some input ID");

	// get idx into array for each ID
	outDataIdxs.reserve(std::ssize(enttsIDs));

	for (const EntityID& id : enttsIDs)
		outDataIdxs.push_back(GetIdxInSortedArr(ids_, id));
}

///////////////////////////////////////////////////////////

void EntityManager::GetEnttsIDsByDataIdxs(
	const std::vector<ptrdiff_t>& enttsDataIdxs,
	std::vector<EntityID>& outEnttsIDs)
{
	// get entity ID value from the array by data idx;
	// 
	// in:   SORTED array of indices
	// out:  array of entities IDs

	outEnttsIDs.reserve(enttsDataIdxs.size());

	for (const ptrdiff_t idx : enttsDataIdxs)
		outEnttsIDs.push_back(ids_[idx]);
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsByDataIdxsHaveComponent(
	const std::vector<ptrdiff_t>& enttsDataIdxs,
	const ComponentType componentType)
{
	const u32 bitmaskForComponent = (1 << componentType);
	bool haveComponent = true;

	for (const ptrdiff_t idx : enttsDataIdxs)
		haveComponent &= (bool)(componentFlags_[idx] & bitmaskForComponent);

	return haveComponent;
}

}