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
		COM_ERROR_IF_FALSE(false, "you can't have more that only one instance of this class");
	}
}

TextureManagerClass::~TextureManagerClass()
{
	textures_.clear();   // clear up the textures list

	pInstance_ = nullptr;
}








////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

TextureManagerClass* TextureManagerClass::Get() 
{ 
	assert(pInstance_ != nullptr && "you have to create an instance of the TextureManagerClass");
	return pInstance_; 
}

void TextureManagerClass::Initialize(ID3D11Device* pDevice)
{
	// check input params
	assert(pDevice != nullptr);

	pDevice_ = pDevice;

	// create a couple of default textures
	TextureClass unloadedTexture = TextureClass(pDevice, Colors::UnloadedTextureColor, aiTextureType_DIFFUSE);
	TextureClass unhandledTexture = TextureClass(pDevice, Colors::UnhandledTextureColor, aiTextureType_DIFFUSE);

	const auto it = textures_.insert({ "unloaded_texture", std::move(unloadedTexture) });

	// if something went wrong
	if (!it.second)
	{
		COM_ERROR_IF_FALSE(false, "can't insert an unloaded_texture object");
	}

	const auto it2 = textures_.insert({ "unhandled_texture", std::move(unhandledTexture) });

	// if something went wrong
	if (!it2.second)
	{
		COM_ERROR_IF_FALSE(false, "can't insert an unhandled_texture object");
	}

	return;
}

TextureClass* TextureManagerClass::GetTextureByKey(const std::string & texturePath)
{
	// when we call this function with such a texturePath as input parameter we initialize
	// a new texture from the file by this texturePath;
	// then we store this texture in the texture manager;
	//
	// when we call not for the first time with such a texturePath as input parameter we
	// get a texture (which had already been initialized before) by the texturePath and 
	// return a ptr to it so later we can copy its data;
	// it is faster than just reading from its data file anew;

	// check if the input params is correct
	assert(!texturePath.empty());

	try
	{
		// try to get a refference to the texture class obj
		return &(textures_.at(texturePath));  
	}
	catch (const std::out_of_range & e) // currently we don't have such a texture so try to create it
	{
		Log::Debug(LOG_MACRO, e.what());
		std::string debugMsg{ "creation of a texture: " + texturePath };
		Log::Debug(LOG_MACRO, debugMsg.c_str());

		// if we haven't initialized a texture by such a texturePath we do it here
		bool result = InitializeTextureFromFile(texturePath);
		COM_ERROR_IF_FALSE(result, "can't initialize a texture from the file by path: " + texturePath);

		// if everything is ok now we have a texture object by a key which is our input texturePath
		return &(textures_.at(texturePath));
	}

} // end GetTexture

///////////////////////////////////////////////////////////

bool TextureManagerClass::InitializeTextureFromFile(const std::string & texturePath)
{
	// try to create a new texture object from a file by texturePath and insert it
	// into the textures map [key => ptr_to_texture_obj]
	try
	{
		const auto it = textures_.insert({ texturePath, TextureClass(pDevice_, texturePath, aiTextureType_DIFFUSE) });

		// if something went wrong
		if (!it.second)
		{
			COM_ERROR_IF_FALSE(false, "can't insert a texture object by key: " + texturePath);
		}
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for a texture object");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;
} // end InitializeTextureFromFile








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
		COM_ERROR_IF_FALSE(false, "can't allocate memory for a texture object");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

} // end InitializeAllTextures
#endif