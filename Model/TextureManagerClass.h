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
#include <map>
#include <windows.h>

#include "../Engine/log.h"
#include "../Model/textureclass.h"
#include "../Engine/StringConverter.h"

#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;




//////////////////////////////////
// Class name: TextureManagerClass
//////////////////////////////////
class TextureManagerClass
{
public:
	TextureManagerClass();
	~TextureManagerClass();

	// return a pointer to this class instance
	static TextureManagerClass* Get() { return pInstance_; }

	bool Initialize(ID3D11Device* pDevice);  // initialize all the textures

	TextureClass* GetTexture(WCHAR* textureName) const;
	//void RemoveTextureByIndex(UINT index);

private:  // restrict a copying of this class instance
	TextureManagerClass(const TextureManagerClass & obj);
	TextureManagerClass & operator=(const TextureManagerClass & obj);

	// get an array of paths to model textures
	void GetAllTexturesNamesWithinFolder(std::vector<std::wstring> & texturesNames);

private:
	static TextureManagerClass* pInstance_;

	fs::path TEXTURES_DIR_PATH_ { "data/textures/" };
	std::map<std::wstring, TextureClass*> textures_;

};