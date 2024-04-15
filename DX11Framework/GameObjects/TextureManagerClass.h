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

#include <filesystem>


//////////////////////////////////
// Class name: TextureManagerClass
//////////////////////////////////
class TextureManagerClass
{
public:
	TextureManagerClass();
	~TextureManagerClass();

	void Initialize(ID3D11Device* pDevice);

	// return a pointer to this class instance
	static TextureManagerClass* Get();

	TextureClass* GetTextureByKey(const std::string & textureName);

	TextureClass* CreateTextureWithColor(const Color textureColor, const aiTextureType type)
	{
		const BYTE red = textureColor.GetR();
		const BYTE green = textureColor.GetG();
		const BYTE blue = textureColor.GetB();

		const std::string textureID{ "color_texture" + std::to_string(red) + "_" + std::to_string(green) + "_" + std::to_string(blue) };

		const auto it = textures_.insert({ textureID, TextureClass(pDevice_, Color(red, green, blue), type) });

		// if something went wrong
		if (!it.second)
		{
			COM_ERROR_IF_FALSE(false, "can't insert a texture object by key: " + textureID);
		}

		return &(textures_.at(textureID));
	}

	TextureClass* AddTextureByKey(const std::string & textureID, const TextureClass & texture)
	{
		const auto it = textures_.insert({ textureID, TextureClass(texture) });

		// if something went wrong
		if (!it.second)
		{
			COM_ERROR_IF_FALSE(false, "can't insert a texture object by key: " + textureID);
		}

		return &(textures_.at(textureID));
	}

private:  // restrict a copying of this class instance
	TextureManagerClass(const TextureManagerClass & obj);
	TextureManagerClass & operator=(const TextureManagerClass & obj);

private:
	bool InitializeTextureFromFile(const std::string & texturePath);

private:
	static TextureManagerClass* pInstance_;

	ID3D11Device* pDevice_ = nullptr;

	//fs::path pathToTexturesDir_;
	std::map<std::string, TextureClass> textures_;
};