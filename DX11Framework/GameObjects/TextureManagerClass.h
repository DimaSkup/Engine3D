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

	// restrict a copying of this class instance
	TextureManagerClass(const TextureManagerClass& obj) = delete;
	TextureManagerClass& operator=(const TextureManagerClass& obj) = delete;

	inline static TextureManagerClass* Get()
	{
		// return a pointer to this class instance
		assert((pInstance_ != nullptr) && "you have to create an instance of the TextureManagerClass");
		return pInstance_;
	}

	void Initialize(ID3D11Device* pDevice);

	TextureClass* AddTextureByKey(const std::string& textureID, TextureClass& texture);
	TextureClass* GetTextureByKey(const std::string & textureName);
	TextureClass* CreateTextureWithColor(const Color& textureColor, const aiTextureType type);
	
private:
	void InitializeTextureFromFile(const std::string & texturePath);

private:
	static TextureManagerClass* pInstance_;

	ID3D11Device* pDevice_ = nullptr;
	std::map<std::string, TextureClass> textures_;  // ['texture_path/key' => 'texture_obj']
};