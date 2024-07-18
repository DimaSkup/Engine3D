// ********************************************************************************
// Filename:      TextureTransformSystem.h
// 
// Description:   Entity-Component-System (ECS) system for using 
//                of 3 types of texture transformations:
//                1. STATIC textures transformation (for instance: just scale texture);
//                2. DYNAMIC textures transformation (texture animation; for instance: to animate a fireflame)
//                3. rotation around particular texture coordinate (for instance: around its center to animate fireball)
// 
// Created:       29.06.24
// ********************************************************************************
#pragma once

#include "../Components/TextureTransform.h"

class TextureTransformSystem
{
public:
	TextureTransformSystem(TextureTransform* pTexTransformComp);
	~TextureTransformSystem() {}

	void AddStaticTexTransform(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMMATRIX> texTransform);

	void AddAtlasTextureAnimation(
		const EntityID enttID,
		const uint32_t texRows,
		const uint32_t texColumns,
		const float animDuration);

	void AddRotationAroundTexCoord(
		const EntityID enttID,
		const float tu,
		const float tv,
		const float rotationSpeed);

	void GetTexTransformsForEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outTexTransforms);

	void UpdateAllTextrureAnimations(const float totalGameTime, const float deltaTime);

private:
	const ptrdiff_t AddAtlasAnimationData(
		const EntityID enttID,
		const uint32_t texRows,
		const uint32_t texColumns,
		const float animDuration);

	void UpdateTextureAtlasAnimations(const float deltaTime);
	void UpdateTextureRotations(const float totalGameTime);


	void GetDataIdxsOfIDs(
		const std::vector<EntityID>& allEnttsIDs,
		const std::vector<EntityID>& searchedEnttsIDs,
		std::vector<ptrdiff_t>& outDataIdxs);

	void ApplyTexTransformsByIdxs(
		const std::vector<ptrdiff_t>& dataIdxs,
		const std::vector<XMMATRIX>& texTransforms);

	TextureTransform* pTexTransformComponent_ = nullptr;
};