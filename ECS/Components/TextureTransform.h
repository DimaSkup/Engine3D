// *********************************************************************************
// Filename:     TextureTransform.h
// Description:  an ECS component which contains 
//               textures transformations for entities
// 
// Created:      29.06.24
// *********************************************************************************
#pragma once

#include "../Common/Types.h"
#include <vector>


enum TexTransformType
{
	STATIC,                     // no movement over the texture, just static transformation of the texture (for instance: scaling of the terrain texture)
	ATLAS_ANIMATION,            // move over an atlas texture to make an animation
	ROTATION_AROUND_TEX_COORD,  // rotation around particular texture coords during the time
};

//
// HELPER DATA STRUCTURES
//

struct TexAtlasAnimationData
{
	// contains animation data for an atlas texture 

	TexAtlasAnimationData() {}

	TexAtlasAnimationData(
		const u32 texRows,
		const u32 texColumns) :
		texRows_(texRows),
		texColumns_(texColumns),
		texFramesCount_(texRows* texColumns),
		texCellWidth_(1.0f / (float)texColumns),
		texCellHeight_(1.0f / (float)texRows) {}

	u32 currTexFrameIdx_ = 0;       // index of the current animation frame
	u32 texFramesCount_ = 0;        // how many animation frames we have
	u32 texColumns_ = 0;            // vertical cells (frames) count
	u32 texRows_ = 0;               // horizontal cells (frames) count

	float texCellWidth_ = 0;        // width of one animation frame
	float texCellHeight_ = 0;       // height of one animation frame
};


struct TexAtlasAnimations
{
	// TYPE: ATLAS_ANIMATION
	std::vector<EntityID> ids_;
	std::vector<float> timeStep_;               // duration of one animation frame; after this time point we change an animation frame
	std::vector<float> currAnimTime_;           // frame time value in [0, timeStep]; when this val >= timeStep we change a frame
	std::vector<TexAtlasAnimationData> data_;
};

struct TexRotationsAroundCoords
{
	// TYPE: ROTATION_AROUND_TEX_COORD
	std::vector<EntityID> ids_;
	std::vector<XMFLOAT2> texCoords_;
	std::vector<float>    rotationsSpeed_;
};




// ************************************************************************************

//
// COMPONENT
//
struct TextureTransform
{
	// NOTE: we get responsible texture transformation by data idx of ID

	ComponentType type_ = ComponentType::TextureTransformComponent;

	std::vector<EntityID> ids_;
	std::vector<TexTransformType> types_;
	std::vector<XMMATRIX> texTransforms_;

	TexAtlasAnimations texAtlasAnim_;
	TexRotationsAroundCoords texRotations_;
};
