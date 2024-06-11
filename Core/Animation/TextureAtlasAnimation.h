// *********************************************************************************
// Filename:     TextureAtlasAnimation.h
// Description:  contains different data to make possible to do 
//               animation using texture atlas
// 
// Created:      29.04.24
// *********************************************************************************
#pragma once

#include <vector>
#include <cassert>
#include <DirectXMath.h>

typedef unsigned int UINT;

class TextureAtlasAnimation
{
public:
	TextureAtlasAnimation();

	void Initialize(
		const UINT textureRows,
		const UINT textureColumns,
		const float animDuration);

	void Update(const float deltaTime, _Out_ DirectX::XMMATRIX & texTransform);

public:
	std::vector<DirectX::XMFLOAT2> textureTranslations_;

	float currAnimTime_   = 0.0f;
	float texCellWidth_   = 0.0f;
	float texCellHeight_  = 0.0f;
	float animDuration_   = 0.0f;      // summary duration of the animation
	float timeStep_       = 0.0f;      // duration of one animation frame
	UINT texColumns_      = 0;         // horizontal cells count
	UINT texRows_         = 0;         // vertical cells count
	UINT texFramesCount_  = 0;
	UINT currTexFrameIdx_ = 0;
};
