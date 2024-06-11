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

// helper data structure for the Rendered component
struct RenderingParams
{
	RenderingParams(RENDERING_SHADERS shaderType, D3D11_PRIMITIVE_TOPOLOGY topology)
		: renderingShaderType_(shaderType), primTopology_(topology) {}


	RENDERING_SHADERS renderingShaderType_ = RENDERING_SHADERS::COLOR_SHADER;
	D3D11_PRIMITIVE_TOPOLOGY primTopology_ = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};


// Rendered component related data
struct Rendered
{
	ComponentType type_ = ComponentType::RenderedComponent;

	std::unordered_map<EntityID, RenderingParams> records_;
};