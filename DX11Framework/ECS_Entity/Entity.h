// *********************************************************************************
// Filename:      Entity.h
// Description:   an Entity class for Entity-Component-System (ECS) module
// *********************************************************************************
#pragma once

#include "ECS_Types.h"

// *********************************************************************************

#if ECS_VER_0

#include <map>

class BaseComponent;

typedef unsigned int UINT;

class Entity
{
public:
	Entity(const EntityID id) : id_(id) {}
	EntityID GetID() const { return id_; }

	void AddComponent(const ComponentID & componentID)
	{
		COM_ERROR_IF_EMPTY(componentID.empty(), "the input component id is empty");

		const auto res = components_.insert(componentID);
		if (!res.second)
		{
			COM_ERROR_IF_FALSE(false, "can't add component (" + componentID + ") to entity (" + id_ + ")");
		}
	}

	bool HasComponent(const ComponentID & componentID)
	{
		return (components_.find(componentID) != components_.end()) ? true : false;
	}



private:
	EntityID id_ = {"empty_entity"};
	std::set<ComponentID> components_;
};

// *********************************************************************************

#elif ECS_VER_1

#endif