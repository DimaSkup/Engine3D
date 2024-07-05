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
using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMVECTOR = DirectX::XMVECTOR;
using XMMATRIX = DirectX::XMMATRIX;

using ComponentFlagsType = uint32_t;
using MeshID = size_t;
using TextureID = std::string;
using TexturesSet = std::vector<TextureID>;

using u32 = uint32_t;

// "ID" of an entity is just hash which is based on entity name
typedef size_t EntityID;
typedef std::string EntityName;
typedef std::string ComponentID;
typedef std::string SystemID;

const EntityID INVALID_ENTITY_ID{ INT_MAX };
const EntityName INVALID_ENTITY_NAME{ "invalid_entity_name" };


enum ComponentType
{
	TransformComponent,
	MoveComponent,
	RenderedComponent,
	MeshComp,

	WorldMatrixComponent,
	NameComponent,
	AIComponent,
	HealthComponent,

	DamageComponent,
	EnemyComponent,
	EditorTransformComponent,
	ColliderComponent,

	PhysicsTypeComponent,
	VelocityComponent,
	GroundedComponent,
	CollisionComponent,

	CameraComponent,
	TexturedComponent,
	TextureTransformComponent
};

namespace ECS
{
	enum RENDERING_SHADERS
	{
		COLOR_SHADER,
		TEXTURE_SHADER,
		LIGHT_SHADER
	};
};

