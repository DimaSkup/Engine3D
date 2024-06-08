// *********************************************************************************
// Filename:     ECS_Types.h
// Description:  contains types for Entity-Component-System module of the engine;
// 
// Created:
// *********************************************************************************
#pragma once

#define ECS_VER_0 false    // using lite OOP for ECS
#define ECS_VER_1 true     // no OOP

#include <string>

// "ID" of an entity is just an index into the scene array (look inside of the Entity Manager)
typedef size_t EntityID;
const EntityID INVALID_ENTITY_ID{ INT_MAX };

typedef std::string EntityName;
const EntityName INVALID_ENTITY_NAME{ "invalid_entity_name" };

typedef std::string ComponentID;
const ComponentID INVALID_COMPONENT_ID = { "invalid_component_id" };   // theoretically imposible ID of component 
							
typedef std::string SystemID;
const SystemID INVALID_SYSTEM_ID = { "invalid_system_id" };

typedef std::string MeshID;


enum ComponentType
{
	TransformComponent,
	MoveComponent,
	RenderedComponent,
	MeshComp,
	WorldMatrixComponent,
	EditorTransformComponent,
	ColliderComponent,
	PhysicsTypeComponent,
	VelocityComponent,
	GroundedComponent,
	CollisionComponent,
	CameraComponent,
};
