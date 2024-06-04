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

typedef std::string EntityID;
const EntityID INVALID_ENTITY_ID{ "uninitialized_entity" };

typedef std::string ComponentID;
const ComponentID INVALID_COMPONENT_ID = { "invalid_component_id" };   // theoretically imposible ID of component 
							
typedef std::string SystemID;
const SystemID INVALID_SYSTEM_ID = { "invalid_system_id" };

typedef std::string MeshID;


enum ComponentType
{
	TransformComponent,
	MovementComponent,
	RenderedComponent,
	MeshComp,
	EditorTransformComponent,
	ColliderComponent,
	PhysicsTypeComponent,
	VelocityComponent,
	GroundedComponent,
	CollisionComponent,
	CameraComponent
};
