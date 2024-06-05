// *********************************************************************************
// Filename:     Rendered.h
// Description:  an ECS component which adds entities for rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

#include <set>

class Rendered
{
public:
	Rendered() {};

public:
	ComponentType type_ = ComponentType::RenderedComponent;
	std::set<EntityID> entitiesForRendering_;
};