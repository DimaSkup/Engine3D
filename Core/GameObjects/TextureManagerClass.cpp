////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.cpp
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#include "TextureManagerClass.h"
#include <stdexcept>
#include <filesystem>

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

	// if there is already a texture by such ID we just return a ptr to it
	if (textures_.contains(textureID))
	{
		return &textures_[textureID];
	}

	// or create a new single color texture by such ID
	TextureClass texture(pDevice_, textureColor, type);

	// add a new texture into the storage and return a ptr to it
	return AddTextureByKey(textureID, texture);
}

///////////////////////////////////////////////////////////

TextureClass* TextureManagerClass::AddTextureByKey(
	const TextureID& textureID,    
	TextureClass& texture)
{
	const auto it = textures_.insert({ textureID, texture });
	ASSERT_TRUE(it.second, "can't insert a texture object by key: " + textureID);

	// setup ID (name/path) for this texture
	textures_[textureID].SetName(textureID);

	// return a ptr to the added texture
	return &(textures_[textureID]);
}

///////////////////////////////////////////////////////////

TextureClass* TextureManagerClass::GetTextureByKey(const TextureID& texturePath)
{
	// get a ptr to texture by input key (path); 
	// there are two ways:
	// (1) if there is such a texture stored in the TextureManagerClass we just return a ptr;
	// (2) in another case we firstly create a texture from file by input path and then return a ptr to it


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
	const TextureID& texturePath,           // aka texture ID
	const aiTextureType typeOfTexture)
{
	// return a ptr to the texture which is loaded from the file by texturePath;
	// 
	// input: path to the texture file;
	//        type for texture (if we create it)
	// 
	// 1. if such a texture (with such ID) already exists we just return a ptr to it;
	// 2. if there is no texture by such texturePath (ID) we try to create it
	

	try
	{
		// if there is such a texture we just return a ptr to it
		if (textures_.contains(texturePath))
			return &textures_[texturePath];

		// create a new texture from file
		const auto it = textures_.insert({ texturePath, TextureClass(pDevice_, texturePath, typeOfTexture) });
		ASSERT_TRUE(it.second, "can't create a texture from file: " + texturePath);

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
	// return IDs of all the currently loaded textures

	outTexturesIDs.reserve(textures_.size());

	for (const auto& it : textures_)
		outTexturesIDs.push_back(it.first);
}

void TextureManagerClass::GetAllTexturesSRVs(
	std::vector<ID3D11ShaderResourceView*>& outSRVs)
{
	// return SRV (shader resource view) of all the currently loaded textures

	outSRVs.reserve(textures_.size());

	for (const auto& it : textures_)
		outSRVs.push_back(it.second.GetTextureResourceView());
}

///////////////////////////////////////////////////////////

void TextureManagerClass::GetAllTexturesPathsWithinDirectory(
	const std::string& pathToDir,
	std::vector<std::string>& outPathsToTextures)
{
	// get an array of paths to textures in the directory by pathToDir

	namespace fs = std::filesystem;

	// go through each file in the directory
	for (const fs::directory_entry& entry : fs::directory_iterator(pathToDir))
	{
		const fs::path texturePath = entry.path();
		const fs::path ext = texturePath.extension();

		// if we have a DirectDraw surface texture, or Targa texture, or PNG, etc.
		if ((ext == ".dds") || (ext == ".tga") || (ext == ".png"))
		{
			std::string path = StringHelper::ToString(texturePath);
			std::replace(path.begin(), path.end(), '\\', '/');  // in the pass change from '\\' into '/' symbol

			outPathsToTextures.emplace_back(path);
		}
	}
}



#if 0


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