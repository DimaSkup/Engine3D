// *********************************************************************************
// Filename:     RenderStates.h
// Description:  an ECS component which is responsible for storing 
//               render states of entities
//
// Created:      28.08.24
// *********************************************************************************
#pragma once

#include "../Common/Types.h"
#include <vector>

namespace ECS
{

enum RenderStatesTypes
{
	// rasterizer params
	FILL_SOLID,
	FILL_WIREFRAME,
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	FRONT_COUNTER_CLOCKWISE,  // CCW
	FRONT_CLOCKWISE,

	// blending states
	NO_COLOR_WRITE,
	NO_BLENDING,
	ALPHA_ENABLE,
	ADDING,
	SUBTRACTING,
	MULTIPLYING,
	TRANSPARENCY,

	// is used to render textures which is either completely opaque or completely 
	// transparent (for instance: wire fence, foliage, tree leaves); so if pixels
	// have alpha values close to 0 we can reject a src pixel from being futher processed
	NO_ALPHA_CLIPPING,
	ALPHA_CLIPPING,  
	

	// to make possible iteration over the enum
	LAST   
};

struct RenderStates
{
	ComponentType type_ = ComponentType::RenderStatesComponent;

	std::vector<EntityID> ids_;
	std::vector<u32> statesHashes_;    // hash where each bit responds for a specific render state
};

};  // namespace ECS
