// *********************************************************************************
// Filename:     ECS_Types.h
// Description:  contains types for Entity-Component-System module of the engine;
// 
// Created:
// *********************************************************************************
#pragma once


#include <string>
#include <vector>
#include <DirectXMath.h>

// ECS_Common typedefs
using UINT = unsigned int;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMMATRIX = DirectX::XMMATRIX;


// "ID" of an entity is just hash which is based on entity name
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


enum RENDERING_SHADERS
{
	COLOR_SHADER,
	TEXTURE_SHADER,
	LIGHT_SHADER
};

