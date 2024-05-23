// *********************************************************************************
// Filename:     Transform.h
// Description:  an ECS component which contains transformation data of entities;
// 
// Created:
// *********************************************************************************
#pragma once


#include "../ECS_Entity/ECS_Types.h"
#include <DirectXMath.h>

#if ECS_VER_0

#include "BaseComponent.h"
#include <vector>



class Transform : public BaseComponent
{
public:
	struct ComponentData
	{
		DirectX::XMFLOAT3 position_{ 0,0,0 };
		DirectX::XMFLOAT3 direction_{ 0,0,0 };
		DirectX::XMFLOAT3 scale_{ 0,0,0 };
	};

public:
	Transform() :	BaseComponent("Transform") {}
	~Transform() = default;
};


// *********************************************************************************

#elif ECS_VER_1

#include "BaseComponent.h"
#include <map>

class Transform : public BaseComponent
{
public:
	struct ComponentData
	{
		DirectX::XMFLOAT3 position_{ 0,0,0 };
		DirectX::XMFLOAT3 direction_{ 0,0,0 };
		DirectX::XMFLOAT3 scale_{ 0,0,0 };
		DirectX::XMMATRIX world_ = DirectX::XMMatrixIdentity();
	};

public:
	Transform() : BaseComponent("Transform") {}
	~Transform() = default;

	virtual void AddRecord(const EntityID& entityID) override
	{
		const auto res = entityToData_.insert({ entityID, ComponentData() });
		COM_ERROR_IF_FALSE(res.second, "can't create a record for entity: " + entityID);
	}

	virtual void RemoveRecord(const EntityID& entityID) override
	{
		entityToData_.erase(entityID);
	}

public:
	std::map<EntityID, ComponentData> entityToData_;   // pairs of: ['entity_id' => 'data_structure'] 
};

#endif
