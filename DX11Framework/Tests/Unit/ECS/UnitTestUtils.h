#pragma once

#include "../../../ECS_Entity/EntityManager.h"
#include <vector>
#include "../Common/MathHelper.h"


class UnitTestUtils final
{
public:
	UnitTestUtils() {}

	void EntitiesCreationHelper(
		EntityManager& entityMgr,
		std::vector<EntityID>& entityIDs);

	void PrepareRandomDataForArray(
		const size_t arrSize,
		std::vector<DirectX::XMFLOAT3>& outArr);

	void CheckEntitiesHaveComponent(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs,
		const ComponentType& componentID);

	void CheckComponentKnowAboutEntities(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entitiesIDs,
		const ComponentType& componentType);

	void AddTransformComponentHelper(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& positions,
		const std::vector<DirectX::XMFLOAT3>& directions,
		const std::vector<DirectX::XMFLOAT3>& scales);

	void AddMovementComponentHelper(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs);
};