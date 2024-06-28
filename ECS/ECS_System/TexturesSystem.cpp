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
	ASSERT_NOT_NULLPTR(pTextures, "ptr to the Textures component == nullptr");
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
	Textured& texComp = *pTexturesComponent_;

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
	{
		// check if there is no record with such entity ID yet
		if (!BinarySearch(texComp.ids_, enttsIDs[idx]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = GetPosForID(texComp.ids_, enttsIDs[idx]);

			InsertAtPos(texComp.ids_, insertAtPos, enttsIDs[idx]);
			InsertAtPos(texComp.textures_, insertAtPos, textures[idx]);
		}
	}
}