// *********************************************************************************
// Filename:     Rendered.h
// Description:  an ECS component which adds entities for rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

#include "BaseComponent.h"
#include <map>

class Rendered : public BaseComponent
{
public:
	Rendered() : BaseComponent("Rendered") {};

	virtual void AddRecord(const EntityID& entityID) override
	{
		const auto res = entitiesForRendering_.insert(entityID);
		COM_ERROR_IF_FALSE(res.second, "can't add a record for entity: " + entityID);
	}

	virtual void RemoveRecord(const EntityID& entityID) override
	{
		entitiesForRendering_.erase(entityID);
	}

public:
	std::set<EntityID> entitiesForRendering_; 
};
