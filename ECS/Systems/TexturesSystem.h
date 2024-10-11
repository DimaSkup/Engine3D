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
		const std::vector<std::vector<TexID>>& texIDs,
		const std::vector<std::vector<TexPath>>& texPaths);

	const std::vector<TexID>& GetTexIDsByEnttID(const EntityID enttID);

	void GetTexIDsByEnttsIDs(
		const std::vector<EntityID>& ids,
		std::vector<EntityID>& outIdsNoTex,
		std::vector<EntityID>& outIdsWithTex,
		std::vector<TexID>& outTexIdsArrs);

	void GetTexIDsByEnttsIDs(
		const std::vector<EntityID>& ids,
		std::vector<TexID>& outTexIds);

	void FilterEnttsWhichHaveOwnTex(
		const std::vector<EntityID>& ids,
		std::vector<EntityID>& outIds);

	bool CheckTexPathsAreValid(const std::vector<std::vector<TexPath>>& texPaths);

private:
	bool CheckCanAddRecords(const std::vector<EntityID>& ids);

private:
	Textured* pTexturesComponent_ = nullptr;   // a ptr to the instance of Textures component 
};

}