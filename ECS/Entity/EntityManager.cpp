#include "EntityManager.h"
#include "../Common/Utils.h"
#include "../Common/LIB_Exception.h"
#include "../Common/Log.h"

#include "EntityManagerSerializer.h"
#include "EntityManagerDeserializer.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <cctype>
#include <random>

using namespace ECS;
using namespace Utils;


EntityManager::EntityManager() :
	nameSystem_ {&names_},
	transformSystem_{ &transform_, &world_ },
	moveSystem_{ &transform_, &world_, &movement_ },
	meshSystem_{ &meshComponent_ },
	renderSystem_{ &renderComponent_, &transform_, &world_, &meshComponent_ },
	texturesSystem_{ &textureComponent_ },
	texTransformSystem_ { &texTransform_ }
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
		Log::Error(LOG_MACRO, "can't serialize data into a file: " + dataFilepath);
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
		Log::Error(LOG_MACRO, "can't deserialize data from the file: " + dataFilepath);
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

	ASSERT_NOT_ZERO(newEnttsCount, "new entitites count == 0");

	std::vector<EntityID> generatedIDs;
	GenerateIDs(newEnttsCount, generatedIDs);

	for (const EntityID& ID : generatedIDs)
	{
		const ptrdiff_t insertAtPos = Utils::GetPosForID(ids_, ID);

		// add new ID into the sorted array of IDs
		Utils::InsertAtPos<EntityID>(ids_, insertAtPos, ID);

		// set that each new entity by default doesn't have any component
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
}


// ************************************************************************************
//                          PUBLIC RENDERING FUNCTIONS
// ************************************************************************************

void EntityManager::GetRenderingDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<XMMATRIX>& outWorldMatrices,
	std::vector<RENDERING_SHADERS>& outShaderTypes,
	std::vector<MeshID>& outMeshesIDs,
	std::vector<std::set<EntityID>>& outEnttsByMeshes)
{
	// get data which will be used for rendering of the entities;
	// in:   array of entities IDs
	// out:  rendering data of each input entity by its ID

	try
	{
		transformSystem_.GetWorldMatricesOfEntts(enttsIDs, outWorldMatrices);
		renderSystem_.GetRenderingDataOfEntts(enttsIDs, outShaderTypes);

		meshSystem_.GetMeshesIDsRelatedToEntts(
			enttsIDs,
			outMeshesIDs,
			outEnttsByMeshes);
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, false);
		THROW_ERROR("can't get rendering data using the RenderSystem (ECS)");
	}
}

// *********************************************************************************
//                     ADD COMPONENTS PUBLIC FUNCTIONS
// *********************************************************************************

#pragma region AddComponentsAPI

void EntityManager::FilterInputEnttsByComponents(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<ComponentType> compTypes,
	std::vector<EntityID>& outFilteredEntts)
{
	ComponentFlagsType bitmask = 0;
	std::vector<ComponentFlagsType> componentFlags;

	// create bitmask
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
		ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
		ASSERT_TRUE(enttsCount == enttsNames.size(), "count of entities IDs and names must be equal");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::NameComponent);

		nameSystem_.AddRecords(enttsIDs, enttsNames);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
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
		ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
		ASSERT_TRUE(enttsCount == std::ssize(positions), "count of entities and positions must be equal");
		ASSERT_TRUE(enttsCount == std::ssize(dirQuats), "count of entities and directions must be equal");
		ASSERT_TRUE(enttsCount == std::ssize(uniformScales), "count of entities and scales must be equal");


		std::vector<ptrdiff_t> enttsDataIdxs;
		
		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::TransformComponent);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::WorldMatrixComponent);

		transformSystem_.AddRecords(enttsIDs, positions, dirQuats, uniformScales);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
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
		ASSERT_NOT_ZERO(enttsCount, "array of entities IDs is empty");
		ASSERT_TRUE(enttsCount == translations.size(), "count of entities and translations must be equal");
		ASSERT_TRUE(enttsCount == rotationQuats.size(), "count of entities and rotationQuats must be equal");
		ASSERT_TRUE(enttsCount == uniformScaleFactors.size(), "count of entities and scaleFactors must be equal");

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
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
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

void EntityManager::AddMeshComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<u32>& meshesIDs)
{
	// add MeshComponent to each entity by its ID; 
	// and bind to each input entity all the meshes IDs from the input array

	try
	{
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_NOT_EMPTY(meshesIDs.empty(), "the array of meshes IDs is empty");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::MeshComp);

		meshSystem_.AddRecords(enttsIDs, meshesIDs);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
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
	const std::vector<EntityID>& enttsIDs,
	const std::vector<ECS::RENDERING_SHADERS>& shadersTypes,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes)
{
	// add RenderComponent to each entity by its ID; 
	// so these entities will be rendered onto the screen

	try
	{
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_TRUE(std::ssize(enttsIDs) == shadersTypes.size(), "entities count != count of the input shaders types");
		ASSERT_TRUE(std::ssize(enttsIDs) == topologyTypes.size(), "entities count != count of the input primitive topoECS::Logy types");

		std::vector<ptrdiff_t> enttsDataIdxs;

		GetDataIdxsByIDs(enttsIDs, enttsDataIdxs);
		SetEnttsHaveComponent(enttsDataIdxs, ComponentType::RenderedComponent);

		renderSystem_.AddRecords(enttsIDs, shadersTypes, topologyTypes);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't add component to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTextureTransformComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMMATRIX>& texTransform)
{
	// add the TextureTransform component to each input entity by its ID
	// and set a STATIC texture transformation for responsible entities

	try
	{
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_TRUE(std::ssize(enttsIDs) == std::ssize(texTransform), "entities count != count of texture transformations");

		SetEnttsHaveComponent(enttsIDs, ComponentType::TextureTransformComponent);
		texTransformSystem_.AddStaticTexTransform(enttsIDs, texTransform);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error(LOG_MACRO, 
			"can't add the TextureTransform component"
			"to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTextureTransformComponent(
	const EntityID enttID,
	const u32 texRows,
	const u32 texColumns,
	const float animDuration)
{
	// for ANIMATED texture transformation;
	// 
	// add the TextureTransform component to the input entity 
	// by its ID and setup a TEXTURE ANIMATION for it

	try
	{
		ASSERT_TRUE((bool)(texRows & texColumns), "the number of texture rows/columns == 0");
		ASSERT_TRUE(animDuration > 0.0f, "the duration of animation can't be <= 0");

		SetEnttsHaveComponent(std::vector<EntityID>{ enttID }, ComponentType::TextureTransformComponent);

		texTransformSystem_.AddAtlasTextureAnimation(
			enttID,
			texRows, 
			texColumns, 
			animDuration);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error(LOG_MACRO,
			"can't add the TextureTransform component"
			"to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>({ enttID }));
	}
}

///////////////////////////////////////////////////////////

void EntityManager::AddTextureTransformComponentRotationAroundTexCoord(
	const EntityID enttID,
	const float tu,
	const float tv,
	const float rotationSpeed)
{
	// setup rotation around particular texture coordinate for input entity
	// (for instance: p(0.5, 0.5) - rotation arount its center)

	try
	{
		SetEnttsHaveComponent(std::vector<EntityID>{ enttID }, ComponentType::TextureTransformComponent);
		texTransformSystem_.AddRotationAroundTexCoord(enttID, tu, tv, rotationSpeed);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error(LOG_MACRO,
			"can't add the TextureTransform component"
			"to entities by IDs: " + Utils::JoinArrIntoStr<EntityID>({ enttID }));
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
		ASSERT_NOT_EMPTY(enttsIDs.empty(), "the array of entities IDs is empty");
		ASSERT_TRUE(std::ssize(enttsIDs) == std::ssize(texIDs), "entities count != textures IDs arrays count");
		ASSERT_TRUE(std::ssize(enttsIDs) == std::ssize(texPaths), "entities count != textures paths arrays count");

		// check each textures IDs arr
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
			
		ASSERT_TRUE(texIDsAreOk, "the textures IDs data is INVALID (wrong number of)");
		ASSERT_TRUE(texPathsAreOk, "the textures paths data is INVALID (wrong number of / some path is empty)");

		// ------------------------------------------------

		SetEnttsHaveComponent(enttsIDs, ComponentType::TexturedComponent);
		texturesSystem_.AddRecords(enttsIDs, texIDs, texPaths);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e);
		Log::Error(LOG_MACRO,
			"can't add the Textured component to entities by IDs: " +
			Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

#pragma endregion


// ************************************************************************************
//                            PUBLIC QUERY API
// ************************************************************************************

#pragma region PublicQueryAPI

const std::vector<EntityID>& EntityManager::GetAllEnttsIDs() const
{
	// return IDs of all the existing entities
	return ids_;
}

///////////////////////////////////////////////////////////

bool EntityManager::CheckEnttsByIDsExist(const std::vector<EntityID>& enttsIDs)
{
	// check by ID if each entity from the input array is created;
	// return: true  -- if some entity from the input arr exists
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
			Log::Error(LOG_MACRO, "Unknown component type: " + std::to_string(componentType));
			THROW_ERROR("can't get IDs of entities which have such component: " + std::to_string(componentType));
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
	// generate unique IDs in quantity newEnttsCount
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

	ASSERT_TRUE(enttsValid, "there is no entity by some input ID");

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
	// get entity ID value from the array by data idx
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
