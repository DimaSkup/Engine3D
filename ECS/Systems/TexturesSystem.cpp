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

using namespace Utils;

namespace ECS
{


TexturesSystem::TexturesSystem(Textured* pTextures)
{
	Assert::NotNullptr(pTextures, "input ptr to the Textures component == nullptr");
	pTexturesComponent_ = pTextures;
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
	const std::vector<TexIDsArr>& texIDs,
	const std::vector<TexPathsArr>& texPaths)
{
	//
	// add own textures set to each input entity
	//

	Textured& texComp = *pTexturesComponent_;
	bool canAddRecords = true;

	// check if there are no records with such entities IDs yet
	for (const EntityID& id : enttsIDs)
	{
		canAddRecords &= (!BinarySearch(texComp.ids_, id));
	}

	Assert::True(canAddRecords, "can't add records: there is already a record with some entity ID");

	// add records (here we execute sorted insertion into the data arrays)
	for (u32 i = 0; const EntityID& id : enttsIDs)
	{
		const ptrdiff_t insertAtPos = GetPosForID(texComp.ids_, enttsIDs[i]);

		InsertAtPos(texComp.ids_, insertAtPos, enttsIDs[i]);
		InsertAtPos(texComp.texIDs_, insertAtPos, texIDs[i]);
		InsertAtPos(texComp.texPaths_, insertAtPos, texPaths[i]);
	}
}

///////////////////////////////////////////////////////////

const TexIDsArr& TexturesSystem::GetTexIDsByEnttID(const EntityID enttID)
{
	const Textured& comp = *pTexturesComponent_;
	
	// if there is a record: 'entt_id' => 'textures_set'
	// then we define an idx to the textures set and return this set
	//if (BinarySearch(comp.ids_, enttID))
	return comp.texIDs_[GetIdxInSortedArr(comp.ids_, enttID)];
}

///////////////////////////////////////////////////////////

void TexturesSystem::GetTexIDsByEnttsIDs(
	const std::vector<EntityID>& ids,
	std::vector<EntityID>& outNoTex,     // entities without the Textured component
	std::vector<EntityID>& outWithTex,   // entities with the Textured component
	std::vector<TexID>& outTexIds) // own textures of entities which have the Textured component
{
	const Textured& comp = *pTexturesComponent_;
	std::vector<bool> flags;
	std::vector<ptrdiff_t> idxs;
	

	// define which input entities has the Textured component and which doesn't
	GetExistingFlags(comp.ids_, ids, flags);

	outNoTex.reserve(flags.size());
	outWithTex.reserve(flags.size());

	std::vector<std::vector<EntityID>*> enttsArrs = { &outNoTex, &outWithTex };

	for (size idx = 0; bool hasTex : flags)
		enttsArrs[hasTex]->push_back(ids[idx++]);

	outNoTex.shrink_to_fit();
	outWithTex.shrink_to_fit();

	// get data idxs of entts which has the Textured component
	Utils::GetIdxsInSortedArr(comp.ids_, outWithTex, idxs);

	// get a textures arr for each entity which has the Textured component
	outTexIds.reserve(Textured::TEXTURES_TYPES_COUNT * std::ssize(outWithTex));

	for (const ptrdiff_t& idx : idxs)
		Utils::AppendArray(outTexIds, comp.texIDs_[idx]);
}

///////////////////////////////////////////////////////////

void TexturesSystem::GetTexIDsByEnttsIDs(
	const std::vector<EntityID>& ids,
	std::vector<TexID>& outTexIds)
{
	const Textured& comp = *pTexturesComponent_;
	std::vector<ptrdiff_t> idxs;

	// get data idxs of entts which has the Textured component
	Utils::GetIdxsInSortedArr(comp.ids_, ids, idxs);

	// get a textures arr for each entity which has the Textured component
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

	const Textured& component = *pTexturesComponent_;
	std::vector<bool> flags;
	Utils::GetExistingFlags(component.ids_, ids, flags);

	outIds.resize(std::ssize(ids));
	u32 pos = 0;

	for (u32 idx = 0; bool hasComponent : flags)
	{
		outIds[pos] = ids[idx++];
		pos += (hasComponent);
	}

	outIds.resize(pos);
}

}