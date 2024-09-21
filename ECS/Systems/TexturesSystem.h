// **********************************************************************************
// Filename:      TexturesSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                textures data of entities
// 
// Created:       28.06.24
// **********************************************************************************
#pragma once

#include "../Common/Types.h"
#include "../Components/Textured.h"
#include <fstream>

namespace ECS
{

class TexturesSystem
{
public:
	TexturesSystem(Textured* pTextures);
	~TexturesSystem() {};

	void Serialize(std::ofstream& fout, u32& offset);
	void Deserialize(std::ifstream& fin, const u32 offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<TexIDsArr>& texIDs,
		const std::vector<TexPathsArr>& texPaths);

	const TexIDsArr& GetTexIDsByEnttID(const EntityID enttID);

	void GetTexIDsByEnttsIDs(
		const std::vector<EntityID>& ids,
		std::vector<EntityID>& outIdsNoTex,
		std::vector<EntityID>& outIdsWithTex,
		std::vector<TexID>& outTexIdsArrs);

private:
	Textured* pTexturesComponent_ = nullptr;   // a ptr to the instance of Textures component 
};

}