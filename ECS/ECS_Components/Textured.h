// *********************************************************************************
// Filename:     Textured.h
// Description:  an ECS component which contains textures data of entities;
// 
// Created:      28.06.24
// *********************************************************************************
#pragma once

#include "../ECS_Common/ECS_Types.h"
#include <vector>


struct Textured
{
	ComponentType type_ = ComponentType::TexturedComponent;
	static const ptrdiff_t TEXTURES_TYPES_COUNT = 22;  // we have this number of types in the assimp

	std::vector<EntityID> ids_;
	std::vector<TexturesSet> textures_;         // each entity with this component has its own textures set
};
