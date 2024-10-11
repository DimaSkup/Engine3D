// *********************************************************************************
// Filename:     Textured.h
// Description:  an ECS component which contains textures data of entities;
// 
// Created:      28.06.24
// *********************************************************************************
#pragma once

#include "../Common/Types.h"
#include <vector>

namespace ECS
{

struct Textured
{
	ComponentType type_ = ComponentType::TexturedComponent;
	static const ptrdiff_t TEXTURES_TYPES_COUNT = 22;  // AI_TEXTURE_TYPE_MAX + 1

	std::vector<EntityID>                ids_;
	std::vector<std::vector<TexID>>      texIDs_;    // each entt with this component has an IDs array of its textures
	std::vector<std::vector<TexPath>>    texPaths_;  // each entt with this component has a paths array of its textures
};

}