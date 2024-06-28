// **********************************************************************************
// Filename:      TexturesSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                textures data of entities
// 
// Created:       28.06.24
// **********************************************************************************
#pragma once

#include "../ECS_Common/ECS_Types.h"
#include "../ECS_Components/Textured.h"
#include <fstream>


class TexturesSystem
{
public:
	TexturesSystem(Textured* pTextures);

	void Serialize(std::ofstream& fout, size_t& offset);
	void Deserialize(std::ifstream& fin, const size_t offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<std::vector<TextureID>> textures);

private:
	Textured* pTexturesComponent_ = nullptr;   // a ptr to the instance of Textures component 
};