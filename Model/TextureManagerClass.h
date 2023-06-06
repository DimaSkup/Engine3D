////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.h
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11tex.h>
#include <vector>
#include <windows.h>

#include "../Engine/log.h"
#include "../Model/textureclass.h"
#include "../Engine/StringConverter.h"




//////////////////////////////////
// Class name: TextureManagerClass
//////////////////////////////////
class TextureManagerClass
{
public:
	TextureManagerClass();
	~TextureManagerClass();

	bool Initialize();  // initialize all the textures

	//const TextureClass* GetTextureByName(const wchar_t* textureName);
	//void RemoveTextureByIndex(UINT index);

private:  // restrict a copying of this class instance
	TextureManagerClass(const TextureManagerClass & obj);
	TextureManagerClass & operator=(const TextureManagerClass & obj);

	std::vector<std::wstring> GetAllFilesNamesWithinFolder(std::string folderName);

private:
	std::string TEXTURES_DIR_PATH { "data/textures/" };
	std::vector<TextureClass*> texturesArray_;
};