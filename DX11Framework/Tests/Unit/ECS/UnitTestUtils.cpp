#include "UnitTestUtils.h"

using namespace DirectX;


void UnitTestUtils::EntitiesCreationHelper(
	EntityManager& entityMgr,
	std::vector<EntityID>& entityIDs)
{
	// generate unique ID for each entity and create entities inside the manager

	const std::vector<std::string> prefixes{ "sphere_", "cylinder_", "cube_" };
	const UINT entitiesCountPerPrefix = 10;

	// IDs generation
	for (const std::string& prefix : prefixes)
	{
		for (UINT idx = 0; idx < entitiesCountPerPrefix; ++idx)
		{
			entityIDs.push_back({ prefix + std::to_string(idx) });
		}
	}

	// create this bunch of entities
	entityMgr.CreateEntities(entityIDs);
}

///////////////////////////////////////////////////////////

void UnitTestUtils::PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<DirectX::XMFLOAT3>& outArr)
{
	outArr.resize(arrSize);

	// go through each element of the array and generate for it random data
	for (DirectX::XMFLOAT3& elem : outArr)
	{
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
	}
}

///////////////////////////////////////////////////////////

void UnitTestUtils::CheckEntitiesHaveComponent(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs,
	const ComponentType& componentType)
{
	// check if there is a records ['entity_id' => 'component_type'] inside the manager
	for (const EntityID& entityID : entityIDs)
	{
		const bool has = entityMgr.CheckEntityHasComponent(entityID, componentType);
		ASSERT_TRUE(has, "entity (" + entityID + ") must have the component by idx (type): " + std::to_string(componentType));
	}
}

///////////////////////////////////////////////////////////

void UnitTestUtils::CheckComponentKnowAboutEntities(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entitiesIDs,
	const ComponentType& componentType)
{
	// check if a component with componentType constains records with entities
	// from the input entitiesIDs array;

	std::set<EntityID> entitiesInsideComponent;

	switch (componentType)
	{
		case ComponentType::TransformComponent:
		{
			entitiesInsideComponent = entityMgr.transformSystem_.GetEntitiesIDsSet();
			break;
		}
		case ComponentType::MovementComponent:
		{
			entitiesInsideComponent = entityMgr.MoveSystem_.GetEntitiesIDsSet();
			break;
		}
		case ComponentType::MeshComp:
		{
			entitiesInsideComponent = entityMgr.meshSystem_.GetEntitiesIDsSet();
			break;
		}
		case ComponentType::RenderedComponent:
		{
			entitiesInsideComponent = entityMgr.renderSystem_.GetEntitiesIDsSet();
			break;
		}
	}

	// check if component with componentType has records about entities
	for (const EntityID& entityID : entitiesIDs)
	{
		const bool has = entitiesInsideComponent.contains(entityID);
		ASSERT_TRUE(has, "there is no record with such entity (" + entityID + ") inside the component: " + std::to_string(componentType));
	}
}

///////////////////////////////////////////////////////////

void UnitTestUtils::AddTransformComponentHelper(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs,  // to these entities we will add the component
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,
	const std::vector<XMFLOAT3>& scales)
{
	entityMgr.AddTransformComponents(entityIDs, positions, directions, scales);
}

///////////////////////////////////////////////////////////

void UnitTestUtils::AddMovementComponentHelper(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs)
{
	// prepare default movement data for the entities
	std::vector<DirectX::XMFLOAT3> translations(entityIDs.size(), { 0,0,0 });
	std::vector<DirectX::XMFLOAT4> rotQuats(entityIDs.size(), { 0,0,0,0 });
	std::vector<DirectX::XMFLOAT3> scaleChanges(entityIDs.size(), { 1,1,1 });

	entityMgr.AddMovementComponents(entityIDs, translations, rotQuats, scaleChanges);
}