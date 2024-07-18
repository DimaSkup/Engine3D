// ********************************************************************************
// Filename:      TextureTransformSystem.cpp
// Description:   implementation of the TextureTransformSystem functional
// 
// Created:       29.06.24
// ********************************************************************************
#include "TextureTransformSystem.h"

#include "../Common/LIB_Exception.h"
#include "../Common/log.h"
#include "../Common/Utils.h"

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

	//for (u32 idx = 0; idx < std::ssize(enttsIDs); ++idx)

	for (u32 idx = 0; const EntityID& id : enttsIDs)
	{
		// check if there is no record with such entity ID yet
		if (!BinarySearch(comp.ids_, id))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t pos = GetPosForID(comp.ids_, id);

			InsertAtPos(comp.ids_, pos, id);
			InsertAtPos(comp.types_, pos, TexTransformType::STATIC);
			InsertAtPos(comp.texTransforms_, pos, texTransform[idx]);
		}

		++idx;
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::AddAtlasTextureAnimation(
	const EntityID id,
	const u32 texRows,
	const u32 texColumns,
	const float animDuration)
{
	// make a texture animation for entity by ID;
	//
	// if texRows or texColumns > 0 (or both) we separate a texture into frames 
	// (it's supposed that we use an atlas texture getting it from the Textured component);
	// and go through these frames during the time so we make a texture animation;

	TextureTransform& comp = *pTexTransformComponent_;

	// if there is no records with this ID yet
	if (!BinarySearch(comp.ids_, id))
	{
		// execute sorted insertion into the data arrays
		const ptrdiff_t pos = GetPosForID(comp.ids_, id);

		// add common data
		InsertAtPos(comp.ids_, pos, id);
		InsertAtPos(comp.types_, pos, TexTransformType::ATLAS_ANIMATION);

		// add specific data according to this texture transformation type
		const ptrdiff_t anim_idx = AddAtlasAnimationData(id, texRows, texColumns, animDuration);
		
		// setup the texture transformation to the first animation frame
		// (we just scale a texture and setup position to the top left corner)
		TexAtlasAnimationData& animData = comp.texAtlasAnim_.data_[anim_idx];

		InsertAtPos(comp.texTransforms_, pos, XMMatrixTranspose(
			XMMatrixScaling(animData.texCellWidth_, animData.texCellHeight_, 0)));
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::AddRotationAroundTexCoord(
	const EntityID id,
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
	if (!BinarySearch(comp.ids_, id))
	{
		// execute sorted insertion into the common data arrays
		const ptrdiff_t comp_idx = GetPosForID(comp.ids_, id);

		InsertAtPos(comp.ids_, comp_idx, id);
		InsertAtPos(comp.types_, comp_idx, TexTransformType::ROTATION_AROUND_TEX_COORD);
		InsertAtPos(comp.texTransforms_, comp_idx, DirectX::XMMatrixIdentity());

		// setup specific data
		const ptrdiff_t data_idx = GetPosForID(rotations.ids_, id);

		InsertAtPos(rotations.ids_, data_idx, id);
		InsertAtPos(rotations.texCoords_, data_idx, XMFLOAT2(tu, tv));
		InsertAtPos(rotations.rotationsSpeed_, data_idx, rotationSpeed);
	}
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::GetTexTransformsForEntts(
	const std::vector<EntityID>& ids,
	std::vector<XMMATRIX>& outTexTransforms)
{
	// what we do here:
	// go through each input entity and define if it has some tex transformation 
	// if so we store this transformation or in another case we store an identity matrix;
	//
	// in:    SORTED arr of entities IDs
	// out:   arr of texture transformations for these entities

	const TextureTransform& comp = *pTexTransformComponent_;
	const size idsCount = std::ssize(ids);
	std::vector<ptrdiff_t> idxs;
	std::vector<bool> hasTex;


	idxs.reserve(idsCount);
	hasTex.reserve(idsCount);
	outTexTransforms.reserve(idsCount);

	// get data idxs of tex transformations
	// and define which entities has Textured component
	for (const EntityID& id : ids)
	{
		idxs.push_back(GetIdxInSortedArr(comp.ids_, id));
		hasTex.push_back(BinarySearch(comp.ids_, id));
	}

	const XMMATRIX noTransform = DirectX::XMMatrixIdentity();

	// store texture transformations
	for (u32 i = 0; i < (u32)idxs.size(); ++i)
	{
		// branchless (TODO: measurement)
		//const XMMATRIX transforms[2] = { noTransform, comp.texTransforms_[idxs[i]] };
		//outTexTransforms.emplace_back(transforms[hasTex[i]]);


		const XMMATRIX texTrans = (hasTex[i]) ? comp.texTransforms_[idxs[i]] : noTransform;
		outTexTransforms.emplace_back(texTrans);
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

const ptrdiff_t TextureTransformSystem::AddAtlasAnimationData(
	const EntityID id, 
	const u32 texRows, 
	const u32 texColumns, 
	const float animDuration)
{
	// add new texture atlas animation for entity by ID;
	// in:      texRows    -- count of animation frames in one texture row
	//          texColumns -- count of animation frames in one texture column
	// return:  idx into array of animations in the TextureTransform component

	TexAtlasAnimations& anim = pTexTransformComponent_->texAtlasAnim_;

	const ptrdiff_t anim_idx = GetPosForID(anim.ids_, id);
	InsertAtPos(anim.ids_, anim_idx, id);

	// frame_duration = full_anim_duration / frames_count
	InsertAtPos(anim.timeStep_, anim_idx, animDuration / (texRows * texColumns));
	InsertAtPos(anim.currAnimTime_, anim_idx, 0.0f);

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

	std::vector<u32> idxsOfAnimToUpdate;             // idxs of animations where a frame must be changed
	std::vector<EntityID> enttsToUpdate;                // entts IDs whose texture animation was updated
	std::vector<XMMATRIX> texTransToUpdate;           // computed tex transformations for new animation frames
	std::vector<ptrdiff_t> transformsIdxs;              // idxs of transformations which will be updated

	TextureTransform& comp = *pTexTransformComponent_;
	TexAtlasAnimations& anim = comp.texAtlasAnim_;

	// update animation time of each animation and define if its frame must be changed
	for (u32 idx = 0; idx < anim.ids_.size(); ++idx)
	{
		anim.currAnimTime_[idx] += deltaTime;

		if (anim.currAnimTime_[idx] > anim.timeStep_[idx])
		{
			anim.currAnimTime_[idx] = 0;          // reset the current animation time
			idxsOfAnimToUpdate.push_back(idx);    // and store its index
		}
	}

	// prepare memory for some data
	const size animsToUpdateCount = std::ssize(idxsOfAnimToUpdate);
	texTransToUpdate.reserve(animsToUpdateCount);
	enttsToUpdate.reserve(animsToUpdateCount);

	// update each chosen animation
	for (const u32 animIdx : idxsOfAnimToUpdate)
	{
		TexAtlasAnimationData& data = anim.data_[animIdx];

		// change animation frame
		++data.currTexFrameIdx_;

		// do we need to restart animation?
		if (data.currTexFrameIdx_ >= data.texFramesCount_)
			data.currTexFrameIdx_ = 0;

		// compute frame row and column at the texture (atlas texture)
		const u32 col_idx = data.currTexFrameIdx_ % data.texColumns_;
		const u32 row_idx = data.currTexFrameIdx_ / data.texColumns_;

		const float m00 = data.texCellWidth_;
		const float m11 = data.texCellHeight_;

		// store transformation matrix to use it later for update
		texTransToUpdate.emplace_back(
			m00, 0.0f, 0.0f, 0.0f,                     // m00, m01, m02, m03
			0.0f, m11, 0.0f, 0.0f,                     // m10, m11, m12, m13
			0.0f, 0.0f, 0.0f, 0.0f,                    // m20, m21, m22, m23
			m00 * col_idx, m11 * row_idx, 0.0f, 0.0f); // m30, m31, m32, m33
	}

	// get entities IDs whose animations will be updated
	for (const u32 animIdx : idxsOfAnimToUpdate)
		enttsToUpdate.push_back(anim.ids_[animIdx]);

	// get data idxs of transformations to update and apply new values by these idxs
	GetDataIdxsOfIDs(comp.ids_, enttsToUpdate, transformsIdxs);
	ApplyTexTransformsByIdxs(transformsIdxs, texTransToUpdate);
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::UpdateTextureRotations(const float totalGameTime)
{
	// update all the texture rotations around responsible texture coords 
	// according to the time

	TextureTransform& comp = *pTexTransformComponent_;
	TexRotationsAroundCoords& rotations = comp.texRotations_;
	std::vector<ptrdiff_t> transformsIdxs;                    // apply new data by these idxs
	std::vector<XMMATRIX> texTransToUpdate;                   // new texture transformations
	const size texRotCount = std::ssize(rotations.ids_);

	texTransToUpdate.reserve(rotations.ids_.size());

	// compute and store new rotation transformations
	for (u32 idx = 0; idx < texRotCount; ++idx)
	{
		const XMFLOAT2& texCoord = rotations.texCoords_[idx];

		texTransToUpdate.emplace_back(
			DirectX::XMMatrixTranslation(-texCoord.x, -texCoord.y, 0) *    // translate to the rotation center
			DirectX::XMMatrixRotationZ(rotations.rotationsSpeed_[idx] * totalGameTime) *    // rotate by this angle
			DirectX::XMMatrixTranslation(texCoord.x, texCoord.y, 0)        // translate back to the origin pos
		);
	}

	// get data idxs of transformations to update and apply new values by these idxs
	GetDataIdxsOfIDs(comp.ids_, rotations.ids_, transformsIdxs);
	ApplyTexTransformsByIdxs(transformsIdxs, texTransToUpdate);
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
		outDataIdxs.push_back(GetIdxInSortedArr(allEnttsIDs, id));
}

///////////////////////////////////////////////////////////

void TextureTransformSystem::ApplyTexTransformsByIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	const std::vector<XMMATRIX>& texTransToUpdate)
{
	// here apply new texture transformations by input data indices

	TextureTransform& comp = *pTexTransformComponent_;

	// pay attention that we transpose each matrix so we don't need to do it
	// each frame within the rendering shaders
	for (ptrdiff_t data_idx = 0; const ptrdiff_t idx : dataIdxs)
		comp.texTransforms_[idx] = texTransToUpdate[data_idx++];
}
