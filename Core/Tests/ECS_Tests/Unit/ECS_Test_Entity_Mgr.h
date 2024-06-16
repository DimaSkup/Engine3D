// *********************************************************************************
// Filename:      ECS_Test_Entity_Mgr.h
// Description:   test for the EntityManager functional
// 
// Created:       15.06.24
// *********************************************************************************
#pragma once

class ECS_Test_Entity_Mgr
{
public:
	ECS_Test_Entity_Mgr() {}


	void TestEntityMgrEntitiesCreation();
	void TestEntityMgrSerializationDeserialization();

#if 0
	void SeedEnttMgr(
		EntityManager& entityMgr,
		const UINT enttsCount,
		const std::vector<ComponentType>& componentTypes);
#endif
};