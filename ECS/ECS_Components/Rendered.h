// *********************************************************************************
// Filename:     Rendered.h
// Description:  an ECS component which adds entities for rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

//#include <vector>
#include "../ECS_Common/ECS_Types.h"
#include <unordered_map>
#include <d3d11.h>


struct Rendered
{
	ComponentType type_ = ComponentType::RenderedComponent;

	std::vector<EntityID> ids_;
	std::vector<RENDERING_SHADERS> shaderTypes_;
	std::vector<D3D11_PRIMITIVE_TOPOLOGY> primTopologies_;
};