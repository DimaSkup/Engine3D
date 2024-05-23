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
const ComponentID INVALID_COMPONENT_ID = { "invalid_component" };   // theoretically imposible ID of component 
													  // another code to init with: = std::numeric_limits<size_t>::max()

/*
enum ComponentType
{
	Transform,
	Movement,
	Rendered,
	EditorTransform,
	Mesh,
	Collider,
	PhysicsType,
	Velocity,
	Grounded,
	Collision,
	Camera
};

*/
