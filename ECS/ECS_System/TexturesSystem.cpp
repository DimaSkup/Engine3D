// **********************************************************************************
// Filename:      TexturesSystem.cpp
// Description:   implementation of the TexturesSystem's functional
// 
// Created:       28.06.24
// **********************************************************************************
#include "TexturesSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <stdexcept>
#include <algorithm>

using namespace Utils;


TexturesSystem::TexturesSystem(Textured* pTextures)
{
	ASSERT_NOT_NULLPTR(pTextures, "input ptr to the Textures component == nullptr");
	pTexturesComponent_ = pTextures;
}

///////////////////////////////////////////////////////////

void TexturesSystem::Serialize(std::ofstream& fout, size_t& offset)
{
	assert(0 && "TODO: implement it!");
}

///////////////////////////////////////////////////////////

void TexturesSystem::Deserialize(std::ifstream& fin, const size_t offset)
{
	assert(0 && "TODO: implement it!");
}

///////////////////////////////////////////////////////////

void TexturesSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<std::vector<TextureID>> textures)
{
	// add own textures set to each input entity

	Textured& texComp = *pTexturesComponent_;

	for (size_t i = 0; i < enttsIDs.size(); ++i)
	{
		// check if there is no record with such entity ID yet
		if (!BinarySearch(texComp.ids_, enttsIDs[i]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = GetPosForID(texComp.ids_, enttsIDs[i]);

			InsertAtPos(texComp.ids_, insertAtPos, enttsIDs[i]);
			InsertAtPos(texComp.textures_, insertAtPos, textures[i]);
		}
	}
}

///////////////////////////////////////////////////////////

const TexturesSet& TexturesSystem::GetTexturesSetForEntt(const EntityID enttID)
{
	const Textured& comp = *pTexturesComponent_;
	
	// if there is a record: 'entt_id' => 'textures_set'
	// then we define an idx to the textures set and return this set
	if (BinarySearch(comp.ids_, enttID))
		return comp.textures_[GetIdxOfVal_InSortedArr(comp.ids_, enttID)];

	THROW_ERROR("there is no textures set for entity by ID: " + std::to_string(enttID));
}