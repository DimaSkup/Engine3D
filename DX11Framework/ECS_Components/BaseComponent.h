// *********************************************************************************
// Filename:   BaseComponent.h
// *********************************************************************************
#pragma once

#include "../ECS_Entity/ECS_Types.h"


#if ECS_VER_0

// *********************************************************************************

#elif ECS_VER_1

#include <set>
#include <vector>
#include "../Engine/EngineException.h"

class BaseComponent
{
public:
	struct BaseComponentData {};

public:
	BaseComponent(const ComponentID& componentID) :
		componentID_(componentID)
	{
		ASSERT_NOT_EMPTY(componentID.empty(), "component id is empty");
	}

	inline const ComponentID& GetComponentID() { return componentID_; }

	virtual void AddRecord(const EntityID& entityID) = 0;
	virtual void RemoveRecord(const EntityID& entityID) = 0;

	virtual std::set<EntityID> GetEntitiesIDsSet() const = 0;

public:
	ComponentID componentID_ = INVALID_COMPONENT_ID;
};

#endif