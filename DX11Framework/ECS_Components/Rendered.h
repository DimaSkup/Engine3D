// *********************************************************************************
// Filename:     Rendered.h
// Description:  an ECS component which adds entities for rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

//#include <vector>
#include <unordered_map>
#include "../GameObjects/RenderingShaderHelperTypes.h"
#include <d3d11.h>

class Rendered
{
public:
	struct RenderingParams
	{
		RenderingParams(RENDERING_SHADERS shaderType, D3D11_PRIMITIVE_TOPOLOGY topology)
			: renderingShaderType_(shaderType), primTopology_(topology) {}


		RENDERING_SHADERS renderingShaderType_ = RENDERING_SHADERS::COLOR_SHADER;
		D3D11_PRIMITIVE_TOPOLOGY primTopology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

public:
	Rendered() {};

public:
	ComponentType type_ = ComponentType::RenderedComponent;

	std::unordered_map<EntityID, RenderingParams> records_;

	//std::vector<EntityID>                 ids_;
	//std::vector<RENDERING_SHADERS>        renderingShaderType_;  // what kind of rendering shader will be used for rendering the entity by the same IDX
	//std::vector<D3D11_PRIMITIVE_TOPOLOGY> usePrimTopology_;      // what kind of primitive topology will be used for render the entity by the same IDX
};