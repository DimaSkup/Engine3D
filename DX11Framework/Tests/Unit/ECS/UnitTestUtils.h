#pragma once

#include "../../../ECS_Entity/EntityManager.h"
#include <vector>


class UnitTestUtils final
{
public:
	UnitTestUtils() {}

	void EntitiesCreationHelper(
		EntityManager& entityMgr,
		std::vector<EntityID>& entityIDs);

	void CheckEntitiesHaveComponent(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs,
		const ComponentID& componentID);

	void AddTransformComponentHelper(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs);

	void AddMovementComponentHelper(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs);
};