// *********************************************************************************
// Filename:   BaseSystem.h
// *********************************************************************************
#pragma once

#include "../ECS_Entity/ECS_Types.h"

#if ECS_VER_0

class BaseSystem
{
public:
	BaseSystem(const std::string & sysID, BaseComponent* pComponent) :
		systemID_(sysID),
		pComponent_(pComponent)
	{
		ASSERT_NOT_EMPTY(sysID.empty(), "system ID is empty");
		ASSERT_NOT_NULLPTR(pComponent, "ptr to component == nullptr");
	}
	virtual ~BaseSystem() = default;

	virtual void Update(const double deltaTime) = 0;

	virtual void AddEntity(const EntityID& entityID)
	{
#if 0
		const bool res = pComponent_->entityToData_.insert({ entityID, BaseComponent::ComponentData() });

		ASSERT_TRUE(res.second,
			"can't add an entity (" + entityID + ")  " +
			"to component(" + pComponent_->componentID_ + ")");

#endif
	}


	virtual void RemoveEntity(const EntityID& entityID) = 0;

protected:
	BaseComponent* pComponent_ = nullptr;
	const std::string systemID_{ "unknown_system" };

};

#elif ECS_VER_1

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
	const std::string systemID_;
};

#endif