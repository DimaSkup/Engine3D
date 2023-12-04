////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.cpp
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#include "TextureManagerClass.h"

// initialize a static pointer to this class instance
TextureManagerClass* TextureManagerClass::pInstance_ = nullptr;



TextureManagerClass::TextureManagerClass(const std::string & pathToTexturesDir)
	: pathToTexturesDir_(pathToTexturesDir)
{
	if (pInstance_ == nullptr)
	{
		pInstance_ = this;
	}
	else
		COM_ERROR_IF_FALSE(false, "you can't have more that only one instance of this class");
}


TextureManagerClass::~TextureManagerClass()
{
	if (!textures_.empty())
	{
		for (auto & elem : textures_)
		{
			_DELETE(elem.second);  // release the texture object
		}

		textures_.clear();   // clear up the textures list
	}

	pInstance_ = nullptr;
}

bool TextureManagerClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	Log::Debug("\n\n\n");
	Log::Print("---------------- INITIALIZATION: TEXTURE MANAGER -----------------");

	bool result = false;


	// get paths to textures
	GetAllTexturesNamesWithinTexturesFolder();

	result = InitializeAllTextures(pDevice, pDeviceContext);
	COM_ERROR_IF_FALSE(result, "can't initialize textures list");
	

	return true;
}


// get texture by its path (name)
TextureClass* TextureManagerClass::GetTexture(const WCHAR* textureName) const
{
	assert((textureName != nullptr) && (textureName != L'\0'));

	// check if we have such a name in the textures list
	auto iterator = textures_.find(textureName);

	// if we found some texture so return a pointer to the texture object
	if (iterator != textures_.end())
	{
		return iterator->second;
	}
	else  // we didn't find any data
	{
		std::string errorMsg{ "there is no texture with such a name: " + StringHelper::ToString(textureName) };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	return;

} // end GetTexture

///////////////////////////////////////////////////////////

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
