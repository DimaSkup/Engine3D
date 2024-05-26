// *********************************************************************************
// Filename:      ECS_Unit_Tests.h
// Description:   unit tests for the ECS (Entity-Component-System) module;
// 
// Created:       26.05.24
// *********************************************************************************

#pragma once

#include "../../../ECS_Entity/EntityManager.h"

class ECS_Unit_Tests
{
public:
	ECS_Unit_Tests();

	void Test();
	void TestEntitiesCreation();
	void TestAddTransformComponent();
	void TestAddMovementComponent();

private:
	void EntitiesCreationHelper(EntityManager& entityMgr, std::vector<EntityID>& entityIDs);
	void AddTransformComponentHelper(EntityManager& entityMgr, const std::vector<EntityID>& entityIDs);
	void AddMovementComponentHelper(EntityManager& entityMgr, const std::vector<EntityID>& entityIDs);
};