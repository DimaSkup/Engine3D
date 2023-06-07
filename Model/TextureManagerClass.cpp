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
	std::vector<std::wstring> texturesNames;

	// get paths to textures
	GetAllTexturesNamesWithinFolder(texturesNames);

	// initialize each texture
	for (std::wstring & elem : texturesNames)
	{
		try
		{
			TextureClass* pTexture = new TextureClass;
			textures_.insert({ elem, pTexture });

			WCHAR* wpTextureName = &elem[0];
			result = pTexture->Initialize(pDevice, wpTextureName);
			COM_ERROR_IF_FALSE(result, StringConverter::ToString(elem));
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


TextureClass* TextureManagerClass::GetTexture(WCHAR* textureName) const
{
	auto iterator = textures_.find(textureName);

	if (iterator != textures_.end())
	{
		return iterator->second;
	}

}


// get an array of paths to model textures
void TextureManagerClass::GetAllTexturesNamesWithinFolder(std::vector<std::wstring> & texturesNames)
{
	for (const auto & entry : fs::directory_iterator(TEXTURES_DIR_PATH_))
	{
		fs::path texturePath = entry.path();

		if (texturePath.extension() == ".dds")   // use only directX textures
		{
			std::wstring wTextureName{ texturePath };
			std::replace(wTextureName.begin(), wTextureName.end(), '\\', '/');
			texturesNames.push_back(wTextureName);
		}		
	}
}