////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.cpp
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#include "TextureManagerClass.h"
#include <stdexcept>

// initialize a static pointer to this class instance
TextureManagerClass* TextureManagerClass::pInstance_ = nullptr;



TextureManagerClass::TextureManagerClass()
{
	if (pInstance_ == nullptr)
	{
		pInstance_ = this;	
	}
	else
	{
		THROW_ERROR("you can't have more that only one instance of this class");
	}
}

TextureManagerClass::~TextureManagerClass()
{
	textures_.clear();   // clear up the textures list
	pInstance_ = nullptr;
}



// ************************************************************************************
//
//                               PUBLIC FUNCTIONS
//
// ************************************************************************************

void TextureManagerClass::Initialize(ID3D11Device* pDevice)
{
	// check input params
	ASSERT_NOT_NULLPTR(pDevice, "ptr to the device == nullptr");
	pDevice_ = pDevice;

	// create a couple of default textures
	TextureClass unloadedTexture(pDevice, Colors::UnloadedTextureColor, aiTextureType_DIFFUSE);
	TextureClass unhandledTexture(pDevice, Colors::UnhandledTextureColor, aiTextureType_DIFFUSE);

	// store these texture into the storage
	AddTextureByKey("unloaded_texture", unloadedTexture);
	AddTextureByKey("unhandled_texture", unhandledTexture);
}

///////////////////////////////////////////////////////////

TextureClass* TextureManagerClass::CreateTextureWithColor(
	const Color& textureColor, 
	const aiTextureType type)
{
	// create a texture with a single color by input textureColor

	// generate an ID for this texture
	const BYTE red = textureColor.GetR();
	const BYTE green = textureColor.GetG();
	const BYTE blue = textureColor.GetB();
	const std::string textureID{ "color_texture" + std::to_string(red) + "_" + std::to_string(green) + "_" + std::to_string(blue) };

	TextureClass texture(pDevice_, textureColor, type);

	// add a new texture into the storage and return a ptr to it
	return AddTextureByKey(textureID, texture);
}

///////////////////////////////////////////////////////////

TextureClass* TextureManagerClass::AddTextureByKey(
	const TextureID& textureID,
	TextureClass& texture)
{
	const auto it = textures_.insert({ textureID, std::move(texture) });

	// if something went wrong
	if (!it.second)
	{
		THROW_ERROR("can't insert a texture object by key: " + textureID);
	}

	// return a ptr to the added texture
	return &(textures_.at(textureID));
}

///////////////////////////////////////////////////////////

TextureClass* TextureManagerClass::GetTextureByKey(const TextureID& texturePath)
{
	// get a ptr to texture by input key (path); 
	// there are two ways:
	// (1) if there is such a texture stored in the TextureManagerClass we just return a ptr;
	// (2) in another case we firstly create a texture from file by input path and then return a ptr to it

	ASSERT_NOT_EMPTY(texturePath.empty(), "the path to texture is empty");

	// if there is already a texture by such key (path) we return a ptr to it
	if (textures_.contains(texturePath))
	{
		return &textures_[texturePath];  
	}

	// we don't have such a texture yet so create it and return a ptr to this new texture
	else
	{
		Log::Debug(LOG_MACRO, "creation of a texture: " + texturePath);
		LoadTextureFromFile(texturePath);
		return &textures_[texturePath];
	}
}

///////////////////////////////////////////////////////////

TextureClass* TextureManagerClass::LoadTextureFromFile(
	const TextureID& texturePath,
	const aiTextureType typeOfTexture)
{
	// try to create a new texture object from a file by texturePath and insert it
	// into the textures map [key => ptr_to_texture_obj]
	try
	{
		// if there is such a texture we just return a ptr to it
		if (textures_.contains(texturePath))
			return &textures_[texturePath];

		// create a new texture from file
		const auto it = textures_.insert({ texturePath, TextureClass(pDevice_, texturePath, typeOfTexture) });

		// if something went wrong
		if (!it.second)
			THROW_ERROR("can't create a texture from file: " + texturePath);

		// return a ptr to the texture obj
		return &textures_[texturePath];
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't create a texture from file: " + texturePath);
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't create a texture from file: " + texturePath);
	}
}

///////////////////////////////////////////////////////////

void TextureManagerClass::GetAllTexturesIDs(std::vector<TextureID>& outTexturesIDs)
{
	for (const auto& it : textures_)
		outTexturesIDs.push_back(it.first);
}

void TextureManagerClass::GetAllTexturesSRVs(std::vector<ID3D11ShaderResourceView*>& outSRVs)
{
	for (const auto& it : textures_)
		outSRVs.push_back(it.second.GetTextureResourceView());
}





#if 0
void TextureManagerClass::GetAllTexturesNamesWithinTexturesFolder()
{
	// get an array of paths to models textures in the directory by pathToTexturesDir_

	// go through each file in the directory
	for (const auto & entry : fs::directory_iterator(pathToTexturesDir_))
	{
		fs::path texturePath = entry.path();
	
		if ((texturePath.extension() == ".dds") ||          // if we have a DirectDraw surface texture ...
			(texturePath.extension() == ".tga"))           // ... OR a Targa texture
		{
			std::wstring wTextureName{ texturePath };
			std::replace(wTextureName.begin(), wTextureName.end(), '\\', '/');  // in the pass change from '\\' into '/' symbol

			textures_.insert({ wTextureName, nullptr });   // write this [texture_name => texture_obj_addr] pair directly into the textures list
		}		
	}

	return;

} // end GetAllTexturesNamesWithinTexturesFolder

///////////////////////////////////////////////////////////

bool TextureManagerClass::InitializeAllTextures(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext)
{
	// initialize each texture from the textures folder;
	//
	// earlier in the GetAllTexturesNamesWithinTexturesFolder() function we filled in
	// the textures_ map with pairs [texture_name => nullptr];
	// now we get these texture_names and initialize texture objects

	bool result = false;
	TextureClass* pTexture = nullptr;
	
	try
	{
		for (auto & elem : textures_)
		{
			// get a name of the texture
			const WCHAR* wpTextureName = &(elem.first[0]);          
			std::string filePath{ StringHelper::ToString(elem.first) };

			// create a new texture obj from the texture file
			pTexture = new TextureClass(pDevice, filePath, aiTextureType_DIFFUSE);                           
		
			// relate this texture obj to the texture name
			elem.second = pTexture;   

			std::string debugMsg{ filePath + " -- texture is loaded" };
			Log::Debug(THIS_FUNC, debugMsg.c_str());
		}
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		ASSERT_TRUE(false, "can't allocate memory for a texture object");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

} // end InitializeAllTextures
#endif