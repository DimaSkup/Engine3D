// **********************************************************************************
// Filename:      TexturesSystem.cpp
// Description:   implementation of the TexturesSystem's functional
// 
// Created:       28.06.24
// **********************************************************************************
#include "TexturesSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"
#include "../Common/log.h"

#include <stdexcept>
#include <algorithm>

namespace ECS
{


TexturesSystem::TexturesSystem(Textured* pTextures)
{
	Assert::NotNullptr(pTextures, "input ptr to the Textures component == nullptr");
	pTexturesComponent_ = pTextures;


	// setup default (invalid) textures set:
	// 1. add entt ID with value 0
	// 2. add arr of textures IDs with value 0
	// 3. add arr of texture path with default invalid value
	const u32 texTypesCount = 22;
	pTexturesComponent_->ids_.push_back(INVALID_ENTITY_ID);
	pTexturesComponent_->texIDs_.push_back(std::vector<TexID>(texTypesCount, INVALID_TEXTURE_ID));
	pTexturesComponent_->texPaths_.push_back(std::vector<TexPath>(texTypesCount, INVALID_TEXTURE_PATH));
}

///////////////////////////////////////////////////////////

void TexturesSystem::Serialize(std::ofstream& fout, u32& offset)
{
	Assert::True(false, "TODO: implement it!");
}

///////////////////////////////////////////////////////////

void TexturesSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	Assert::True(false, "TODO: implement it!");
}

///////////////////////////////////////////////////////////

void TexturesSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<std::vector<TexID>>& texIDs,     // array of textures IDs arrays
	const std::vector<std::vector<TexPath>>& texPaths) // array of textures paths arrays
{
	//
	// add own textures set to each input entity
	//

	Assert::True(CheckCanAddRecords(enttsIDs), "can't add records: there is already a record with some entity ID");

	Textured& texComp = *pTexturesComponent_;

	// add records (here we execute sorted insertion into the data arrays)
	for (u32 i = 0; const EntityID& id : enttsIDs)
	{
		const ptrdiff_t insertAtPos = Utils::GetPosForID(texComp.ids_, enttsIDs[i]);

		Utils::InsertAtPos(texComp.ids_, insertAtPos, enttsIDs[i]);
		Utils::InsertAtPos(texComp.texIDs_, insertAtPos, texIDs[i]);
		Utils::InsertAtPos(texComp.texPaths_, insertAtPos, texPaths[i]);
	}
}

///////////////////////////////////////////////////////////

const std::vector<TexID>& TexturesSystem::GetTexIDsByEnttID(const EntityID enttID)
{
	const Textured& comp = *pTexturesComponent_;
	const bool exist = Utils::BinarySearch(comp.ids_, enttID);
	const ptrdiff_t idx = (ptrdiff_t)exist * Utils::GetIdxInSortedArr(comp.ids_, enttID);
	
	return comp.texIDs_[idx];
}

///////////////////////////////////////////////////////////

void TexturesSystem::GetTexIDsByEnttsIDs(
	const std::vector<EntityID>& ids,
	std::vector<TexID>& outTexIds)
{
	// NOTICE: expect as input only IDs of that entts which have the Textured component
	// 
	// out: textures IDs of entts which have the Textured component

	const Textured& comp = *pTexturesComponent_;
	std::vector<ptrdiff_t> idxs;

	Utils::GetIdxsInSortedArr(comp.ids_, ids, idxs);

	outTexIds.reserve(Textured::TEXTURES_TYPES_COUNT * std::ssize(ids));

	for (const ptrdiff_t& idx : idxs)
		Utils::AppendArray(outTexIds, comp.texIDs_[idx]);
}

///////////////////////////////////////////////////////////

void TexturesSystem::FilterEnttsWhichHaveOwnTex(
	const std::vector<EntityID>& ids,
	std::vector<EntityID>& outIds)
{
	// out: entts which have the Textured component

	std::vector<bool> flags;
	Utils::GetExistingFlags(pTexturesComponent_->ids_, ids, flags);

	outIds.resize(std::ssize(ids));
	u32 pos = 0;

	for (u32 idx = 0; bool hasComponent : flags)
	{
		outIds[pos] = ids[idx++];
		pos += (hasComponent);
	}

	outIds.resize(pos);
}

///////////////////////////////////////////////////////////

bool TexturesSystem::CheckTexPathsAreValid(
	const std::vector<std::vector<TexPath>>& texPaths)
{
	// check if all the input texture paths are valid

	const size expectTexCount = (size)Textured::TEXTURES_TYPES_COUNT;
	bool texPathsAreOk = true;

	for (const std::vector<TexPath>& pathsArr : texPaths)
	{
		// do we have proper number of paths in the array?
		texPathsAreOk &= (std::ssize(pathsArr) == expectTexCount);

		// check if each path is not empty
		for (const TexPath& path : pathsArr)
			texPathsAreOk &= (!path.empty());
	}

	return texPathsAreOk;
}

///////////////////////////////////////////////////////////

bool TexturesSystem::CheckCanAddRecords(const std::vector<EntityID>& ids)
{
	// check if all input ids of entts don't exist in the component
	return !Utils::CheckValuesExistInSortedArr(pTexturesComponent_->ids_, ids);
}

}