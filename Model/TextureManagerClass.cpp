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

bool TextureManagerClass::Initialize(ID3D11Device* pDevice)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;


	// get paths to textures
	GetAllTexturesNamesWithinTexturesFolder();

	result = InitializeAllTextures(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize textures list");
	

	return true;
}


// get texture by its path (name)
TextureClass* TextureManagerClass::GetTexture(const WCHAR* textureName) const
{
	assert((textureName != nullptr) && (textureName != L'\0'));

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


// initialize each texture in the textures folder
bool TextureManagerClass::InitializeAllTextures(ID3D11Device* pDevice)
{

	bool result = false;
	TextureClass* pTexture = nullptr;
	
	try
	{
		for (auto & elem : textures_)
		{
			pTexture = new TextureClass;                            // create a new texture obj
			const WCHAR* wpTextureName = &(elem.first[0]);          // get a name of the texture

			result = pTexture->Initialize(pDevice, wpTextureName);  // initialize the texture obj with particular texture
			COM_ERROR_IF_FALSE(result, StringConverter::ToString(elem.first));

			elem.second = pTexture;   // relate this texture obj to the texture name
		}
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

	return true;
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

			textures_.insert({ wTextureName, nullptr });   // write this [texture_name => texture_obj_addr] pair directly into the textures list
		}		
	}
}