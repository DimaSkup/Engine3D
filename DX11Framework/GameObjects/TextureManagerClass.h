////////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.h
// Description:   a manager for work with textures: 
//                when we ask for the texture for the first time we initialize it from 
//                the file and store it in the manager so later it'll be faster to just copy it
//                but not to read it from the file anew.
//                 
// Created:       06.06.23
////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11tex.h>
#include <map>
#include <windows.h>

#include "../Engine/log.h"
#include "../GameObjects/textureclass.h"
#include "../Engine/StringHelper.h"

#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;




//////////////////////////////////
// Class name: TextureManagerClass
//////////////////////////////////
class TextureManagerClass
{
public:
	TextureManagerClass(ID3D11Device* pDevice);
	~TextureManagerClass();

	// return a pointer to this class instance
	static TextureManagerClass* Get() { return pInstance_; }

	TextureClass* GetTexturePtrByKey(const std::string & textureName);

private:  // restrict a copying of this class instance
	TextureManagerClass(const TextureManagerClass & obj);
	TextureManagerClass & operator=(const TextureManagerClass & obj);

private:
	bool InitializeTextureFromFile(const std::string & texturePath);

private:
	static TextureManagerClass* pInstance_;

	ID3D11Device* pDevice_ = nullptr;

	//fs::path pathToTexturesDir_;
	std::map<std::string, std::unique_ptr<TextureClass>> textures_;
};