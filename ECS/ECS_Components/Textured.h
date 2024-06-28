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

	std::vector<EntityID> ids_;
	std::vector<std::vector<TextureID>> textures_;
};
