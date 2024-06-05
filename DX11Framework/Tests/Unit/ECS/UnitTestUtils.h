// *********************************************************************************
// Filename:     UnitTestUtils.h
// Description:  contains different utils for ECS components testing
// 
// Created:      26.05.24
// *********************************************************************************

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

	void PrepareRandomDataForArray(
		const size_t arrSize,
		std::vector<DirectX::XMFLOAT4>& outArr);

	bool CheckEntitiesHaveComponent(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entityIDs,
		const ComponentType& componentID);

	bool CheckComponentKnowsAboutEntities(
		EntityManager& entityMgr,
		const std::vector<EntityID>& entitiesIDs,
		const ComponentType& componentType);
};