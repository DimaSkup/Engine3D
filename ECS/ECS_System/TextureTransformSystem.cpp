// ********************************************************************************
// Filename:      TextureTransformSystem.cpp
// Description:   implementation of the TextureTransformSystem functional
// 
// Created:       29.06.24
// ********************************************************************************
#include "TextureTransformSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/log.h"
#include "../ECS_Common/Utils.h"

#include <fstream>    // for serialization / deserialization of data

using namespace Utils;
using namespace DirectX;


TextureTransformSystem::TextureTransformSystem(TextureTransform* pTexTransformComp)
{
	ASSERT_NOT_NULLPTR(pTexTransformComp, "input ptr to the texture transform component == nullptr");
	pTexTransformComponent_ = pTexTransformComp;
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::AddStaticTexTransform(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMMATRIX> texTransform)
{
	// add a STATIC (it won't move) texture transformation to each input entity

	TextureTransform& comp = *pTexTransformComponent_;

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
	{
		// check if there is no record with such entity ID yet
		if (!BinarySearch(comp.ids_, enttsIDs[idx]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = GetPosForID(comp.ids_, enttsIDs[idx]);

			InsertAtPos(comp.ids_, insertAtPos, enttsIDs[idx]);
			InsertAtPos(comp.types_, insertAtPos, TexTransformType::STATIC);
			InsertAtPos(comp.texTransforms_, insertAtPos, texTransform[idx]);
		}
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::AddAtlasTextureAnimation(
	const EntityID enttID,
	const uint32_t texRows,
	const uint32_t texColumns,
	const float animDuration)
{
	// make a texture animation for entity by ID;
	//
	// if texRows or texColumns > 0 (or both) we separate a texture into frames 
	// (it's supposed that we use an atlas texture getting it from the Textured component);
	// and go through these frames during the time so we make a texture animation;

	TextureTransform& comp = *pTexTransformComponent_;

	// if there is no records with this ID yet
	if (!BinarySearch(comp.ids_, enttID))
	{
		// execute sorted insertion into the data arrays
		const ptrdiff_t comp_idx = GetPosForID(comp.ids_, enttID);

		// add common data
		InsertAtPos(comp.ids_, comp_idx, enttID);
		InsertAtPos(comp.types_, comp_idx, TexTransformType::ATLAS_ANIMATION);

		// add specific data according to this texture transformation type
		const ptrdiff_t anim_idx = AddAtlasAnimationData(enttID, texRows, texColumns, animDuration);
		
		// setup the texture transformation to the first animation frame
		// (we just scale a texture and setup position to the top left corner)
		TexAtlasAnimationData& animData = comp.texAtlasAnim_.data_[anim_idx];

		InsertAtPos(comp.texTransforms_, comp_idx, XMMatrixTranspose(
			XMMatrixScaling(animData.texCellWidth_, animData.texCellHeight_, 0)));
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::AddRotationAroundTexCoord(
	const EntityID enttID,
	const float tu,
	const float tv,
	const float rotationSpeed)
{
	// add rotation around particular texture coordinate for input entity
	// (for instance: p(0.5, 0.5) - rotation arount its center)
	// 
	// input: tu,tv         -- texture coords
	//        rotationSpeed -- how fast the texture will rotate

	TextureTransform& comp = *pTexTransformComponent_;
	TexRotationsAroundCoords& rotations = comp.texRotations_;

	// if there is no records with this ID yet
	if (!BinarySearch(comp.ids_, enttID))
	{
		// execute sorted insertion into the common data arrays
		const ptrdiff_t comp_idx = GetPosForID(comp.ids_, enttID);

		InsertAtPos(comp.ids_, comp_idx, enttID);
		InsertAtPos(comp.types_, comp_idx, TexTransformType::ROTATION_AROUND_TEX_COORD);
		InsertAtPos(comp.texTransforms_, comp_idx, DirectX::XMMatrixIdentity());

		// setup specific data
		const ptrdiff_t data_idx = GetPosForID(rotations.ids_, enttID);

		InsertAtPos(rotations.ids_, data_idx, enttID);
		InsertAtPos(rotations.texCoords_, data_idx, XMFLOAT2(tu, tv));
		InsertAtPos(rotations.rotationsSpeed_, data_idx, rotationSpeed);
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::GetTexTransformsForEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<XMMATRIX>& outTexTransforms)
{
	// 1. if entity by ID has some tex transformation we store this transformation
	// 2. if entity by ID hasn't any tex transformation we store an identity matrix;

	TextureTransform& comp = *pTexTransformComponent_;
	outTexTransforms.reserve(std::ssize(enttsIDs));

	for (const EntityID& id : enttsIDs)
	{
		// if we have some texture transformation by this ID
		if (BinarySearch(comp.ids_, id))
		{
			const ptrdiff_t transform_idx = GetIdxOfVal_InSortedArr(comp.ids_, id);
			outTexTransforms.emplace_back(comp.texTransforms_[transform_idx]);
		}
		else
		{
			outTexTransforms.emplace_back(DirectX::XMMatrixIdentity());
		}	
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::UpdateAllTextrureAnimations(
	const float totalGameTime,
	const float deltaTime)
{
	UpdateTextureAtlasAnimations(deltaTime);
	UpdateTextureRotations(totalGameTime);
}



// ********************************************************************************
//  
//                              PRIVATE HELPERS
// 
// ********************************************************************************

const ptrdiff_t  TextureTransformSystem::AddAtlasAnimationData(
	const EntityID enttID, 
	const uint32_t texRows, 
	const uint32_t texColumns, 
	const float animDuration)
{
	TexAtlasAnimations& anim = pTexTransformComponent_->texAtlasAnim_;

	const ptrdiff_t anim_idx = GetPosForID(anim.ids_, enttID);
	InsertAtPos(anim.ids_, anim_idx, enttID);

	// frame_duration = full_anim_duration / frames_count
	InsertAtPos<float>(anim.timeStep_, anim_idx, animDuration / (texRows * texColumns));
	InsertAtPos<float>(anim.currAnimTime_, anim_idx, 0);

	// setup animation frames data
	InsertAtPos(anim.data_, anim_idx, TexAtlasAnimationData(texRows, texColumns));

	return anim_idx;
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::UpdateTextureAtlasAnimations(const float deltaTime)
{
	// here we update animation time for each texture atlas animation;
	// if some animation frame must be changed for particular animation we compute
	// a new texture transformation matrix for it

	std::vector<size_t> idxsOfAnimToUpdate;             // idxs of animations where a frame must be changed
	std::vector<EntityID> enttsToUpdate;                // entts IDs whose texture animation was updated
	std::vector<XMMATRIX> transformsToUpdate;           // computed tex transformations for new animation frames
	std::vector<ptrdiff_t> transformsIdxs;              // idxs of transformations which will be updated

	TextureTransform& comp = *pTexTransformComponent_;
	TexAtlasAnimations& anim = comp.texAtlasAnim_;

	// update animation time of each animation and define if its frame must be changed
	for (size_t idx = 0; idx < anim.ids_.size(); ++idx)
	{
		anim.currAnimTime_[idx] += deltaTime;

		if (anim.currAnimTime_[idx] > anim.timeStep_[idx])
		{
			anim.currAnimTime_[idx] = 0;          // reset the current animation time
			idxsOfAnimToUpdate.push_back(idx);    // and store its index
		}
	}

	// prepare memory for some data
	const ptrdiff_t animsToUpdateCount = std::ssize(idxsOfAnimToUpdate);
	transformsToUpdate.reserve(animsToUpdateCount);
	enttsToUpdate.reserve(animsToUpdateCount);

	// update each chosen animation
	for (const size_t animIdx : idxsOfAnimToUpdate)
	{
		TexAtlasAnimationData& data = anim.data_[animIdx];

		// change animation frame
		++data.currTexFrameIdx_;

		// do we need to restart animation?
		if (data.currTexFrameIdx_ >= data.texFramesCount_)
			data.currTexFrameIdx_ = 0;

		// compute frame row and column at the texture (atlas texture)
		const uint32_t col_idx = data.currTexFrameIdx_ % data.texColumns_;
		const uint32_t row_idx = data.currTexFrameIdx_ / data.texColumns_;

		DirectX::XMMATRIX texTransform = DirectX::XMMatrixScaling(
			data.texCellWidth_,
			data.texCellHeight_,
			0.0f);

		// set translation for this frame (by X and Y respectively)
		float* translation = texTransform.r[3].m128_f32;
		translation[0] = data.texCellWidth_ * col_idx;
		translation[1] = data.texCellHeight_ * row_idx;

		transformsToUpdate.push_back(texTransform);
#if 0
		// store transformation matrix to use it later for update
		transformsToUpdate.emplace_back()
			data.texCellWidth_, 0,0,0,     // s0, 0,  0, 0
			0, data.texCellHeight_, 0,0,   // 0,  s1, 0, 0
			0,0,0,0,
		)
#endif
	}

	// get entities IDs whose animations will be updated
	for (const size_t animIdx : idxsOfAnimToUpdate)
		enttsToUpdate.push_back(anim.ids_[animIdx]);

	// get data idxs of transformations to update and apply new values by these idxs
	GetDataIdxsOfIDs(comp.ids_, enttsToUpdate, transformsIdxs);
	ApplyTexTransformsByIdxs(transformsIdxs, transformsToUpdate);
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::UpdateTextureRotations(const float totalGameTime)
{
	// update all the texture rotations around responsible texture coords 
	// according to the time

	TextureTransform& comp = *pTexTransformComponent_;
	TexRotationsAroundCoords& rotations = comp.texRotations_;
	std::vector<XMMATRIX> texTransformsToUpdate;
	std::vector<ptrdiff_t> transformsIdxs;

	texTransformsToUpdate.reserve(rotations.ids_.size());

	// compute and store new rotation transformations
	for (size_t idx = 0; idx < rotations.ids_.size(); ++idx)
	{
		const XMFLOAT2& texCoord = rotations.texCoords_[idx];

		texTransformsToUpdate.emplace_back(
			DirectX::XMMatrixTranslation(-texCoord.x, -texCoord.y, 0) *    // translate to the rotation center
			DirectX::XMMatrixRotationZ(rotations.rotationsSpeed_[idx] * totalGameTime) *    // rotate by this angle
			DirectX::XMMatrixTranslation(texCoord.x, texCoord.y, 0)        // translate back to the origin pos
		);
	}

	// get data idxs of transformations to update and apply new values by these idxs
	GetDataIdxsOfIDs(comp.ids_, rotations.ids_, transformsIdxs);
	ApplyTexTransformsByIdxs(transformsIdxs, texTransformsToUpdate);
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::GetDataIdxsOfIDs(
	const std::vector<EntityID>& allEnttsIDs,
	const std::vector<EntityID>& searchedEnttsIDs,
	std::vector<ptrdiff_t>& outDataIdxs)
{
	// here we get data idx of ID in the examined array;
	//
	// input:   1. examined array of IDs
	//          2. array of searched IDs 
	// output:     array of data idxs

	outDataIdxs.reserve(searchedEnttsIDs.size());

	for (const EntityID id : searchedEnttsIDs)
		outDataIdxs.push_back(GetIdxOfVal_InSortedArr(allEnttsIDs, id));
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::ApplyTexTransformsByIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	const std::vector<XMMATRIX>& texTransformsToUpdate)
{
	// here apply new texture transformations by input data indices

	TextureTransform& comp = *pTexTransformComponent_;
	ptrdiff_t data_idx = 0;

	// pay attention that we transpose each matrix so we don't need to do it
	// each frame within the rendering shaders
	for (const ptrdiff_t idx : dataIdxs)
		comp.texTransforms_[idx] = DirectX::XMMatrixTranspose(texTransformsToUpdate[data_idx++]);
}
