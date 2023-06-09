////////////////////////////////////////////////////////////////////
// Filename:      TextureManagerClass.cpp
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#include "TextureManagerClass.h"


// initialize a static pointer to this class instance
TextureManagerClass* TextureManagerClass::pInstance_ = nullptr;



TextureManagerClass::TextureManagerClass()
{
	if (pInstance_ == nullptr)
		pInstance_ = this;
	else
		COM_ERROR_IF_FALSE(false, "you can't have more that only one instance of this class");
}

TextureManagerClass::~TextureManagerClass()
{
	pInstance_ = nullptr;
}

bool TextureManagerClass::Initialize(ID3D11Device* pDevice)
{
	Log::Debug(THIS_FUNC_EMPTY);

	TextureClass* pTexture = nullptr;
	bool result = false;
	//std::vector<std::wstring> texturesNames;

	// get paths to textures
	GetAllTexturesNamesWithinTexturesFolder();

	// initialize each texture in the textures folder
	for (auto & elem : textures_)
	{
		try
		{
			TextureClass* pTexture = new TextureClass;   // create a new texture obj
			//textures_.insert({ elem, pTexture });

			WCHAR* wpTextureName = &elem[0];
			result = pTexture->Initialize(pDevice, wpTextureName);  // initialize it with particular texture
			COM_ERROR_IF_FALSE(result, StringConverter::ToString(elem.first));

			elem.second = pTexture;
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			COM_ERROR_IF_FALSE(false, "can't allocate memory for the texture object");
		}
		catch (COMException & e)
		{
			Log::Error(e, true);
			return false;
		}
	}

	return true;
}


// get texture by its path (name)
TextureClass* TextureManagerClass::GetTexture(WCHAR* textureName) const
{
	// check if we have such a name in the textures list
	auto iterator = textures_.find(textureName);

	// if find some texture so return a pointer to the texture object
	if (iterator != textures_.end())
	{
		return iterator->second;
	}
	else  // we didn't find any data
	{
		std::string errorMsg{ "there is no texture with such a name: " + StringConverter::ToString(textureName) };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

}


// get an array of paths to model textures
void TextureManagerClass::GetAllTexturesNamesWithinTexturesFolder()
{
	for (const auto & entry : fs::directory_iterator(TEXTURES_DIR_PATH_))
	{
		fs::path texturePath = entry.path();

		if (texturePath.extension() == ".dds")   // use only directX textures
		{
			std::wstring wTextureName{ texturePath };
			std::replace(wTextureName.begin(), wTextureName.end(), '\\', '/');
			textures_.insert({ wTextureName, nullptr });// .push_back(wTextureName);
		}		
	}
}