// *********************************************************************************
// Filename:   BaseSystem.h
// *********************************************************************************
#pragma once

#include "../ECS_Entity/ECS_Types.h"

#include "../Engine/EngineException.h"
#include "../ECS_Components/BaseComponent.h"

class BaseSystem
{
public:
	BaseSystem(const std::string& sysID) :
		systemID_{ sysID }
	{
		ASSERT_NOT_EMPTY(sysID.empty(), "system ID is empty");
	}

	virtual void AddEntity(const EntityID& entityID, BaseComponent* pComponent)
	{
		// add new record into component by input entity ID
		ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");
		ASSERT_NOT_NULLPTR(pComponent, "ptr to component == nullptr");
		pComponent->AddRecord(entityID);
	}

	virtual void RemoveEntity(const EntityID& entityID, BaseComponent* pComponent)
	{
		// remove a record from component by input entity ID
		ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");
		ASSERT_NOT_NULLPTR(pComponent, "ptr to component == nullptr");
		pComponent->RemoveRecord(entityID);
	}

private:
	const std::string systemID_{ INVALID_SYSTEM_ID };
};