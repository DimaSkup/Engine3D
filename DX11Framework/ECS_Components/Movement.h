// *********************************************************************************
// Filename:     Movement.h
// Description:  an ECS component which contains movement data of entities;
// 
// Created:
// *********************************************************************************
#pragma once



#include "../ECS_Entity/ECS_Types.h"
#include "BaseComponent.h"

#include <DirectXMath.h>
#include <map>

class Movement : public BaseComponent
{
public:
	struct ComponentData
	{
		DirectX::XMFLOAT3 translation_{0,0,0};
		DirectX::XMFLOAT4 rotationQuat_{0,0,0,0};                // rotation quatertion 
		DirectX::XMFLOAT3 scaleChange_{1,1,1};
	};

public:
	Movement() : BaseComponent(__func__)	{}

	virtual void AddRecord(const EntityID& entityID) override
	{
		const auto res = entityToData_.insert({ entityID, ComponentData() });
		ASSERT_TRUE(res.second, "can't create a record for entity: " + entityID);
	}

	virtual void RemoveRecord(const EntityID& entityID) override
	{
		entityToData_.erase(entityID);
	}

	std::map<EntityID, ComponentData>& GetRefToData()
	{
		return entityToData_;
	}

	virtual std::set<EntityID> GetEntitiesIDsSet() const override
	{
		std::set<EntityID> entityIDs;

		for (const auto& it : entityToData_)
			entityIDs.insert(it.first);

		return entityIDs;
	}

public:
	std::map<EntityID, ComponentData> entityToData_;   // pairs of: ['entity_id' => 'data_structure'] 
};