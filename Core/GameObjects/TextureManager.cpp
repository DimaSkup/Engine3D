////////////////////////////////////////////////////////////////////
// Filename:      TextureManager.cpp
// Description:   a manager for work with textures: initialization of
//                ALL the textures, getting it and releasing;
// Created:       06.06.23
////////////////////////////////////////////////////////////////////
#include "TextureManager.h"
#include <stdexcept>
#include <filesystem>
#include <set>
#include <algorithm>

// for ID generator
#include <cctype>
#include <random>

#include "../Engine/StringHelper.h"
#include "../Engine/log.h"
#include "../Common/Utils.h"


using namespace Utils;


// initialize a static pointer to this class instance
TextureManager* TextureManager::pInstance_ = nullptr;

TextureManager::TextureManager()
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

TextureManager::~TextureManager()
{
	textures_.clear();   // clear up the textures list
	pInstance_ = nullptr;
}



// ************************************************************************************
//
//                            PUBLIC CREATION API
//
// ************************************************************************************

void TextureManager::Initialize(ID3D11Device* pDevice)
{
	// check input params
	ASSERT_NOT_NULLPTR(pDevice, "ptr to the device == nullptr");
	pDevice_ = pDevice;

	// create and store a couple of default textures
	Add("unloaded", { pDevice, Colors::UnloadedTextureColor });

	Add("unhandled_texture", { pDevice, Colors::UnhandledTextureColor });
}

///////////////////////////////////////////////////////////

TextureClass* TextureManager::Add(
	const TexName& name,
	TextureClass& tex)
{
	// add a new texture by name (key)

	try
	{
		const bool isUniqueName = ArrHasVal(names_, name);
		ASSERT_TRUE(isUniqueName, "there is already a texture by such name: " + name);

		const TexID id = GenerateID();
		const ptrdiff_t insertAtPos = GetPosForVal(ids_, id);

		InsertAtPos(ids_, insertAtPos, id);
		InsertAtPos(names_, insertAtPos, name);
		InsertAtPos(textures_, insertAtPos, tex);

		// return a ptr to the added texture
		return &textures_[insertAtPos];
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't add a texture by name: " + name);
	}
}

///////////////////////////////////////////////////////////

TextureClass* TextureManager::Add(
	const TexName& name,
	TextureClass&& tex)
{
	// add a new texture by name (key)

	try
	{
		const bool isUniqueName = !ArrHasVal(names_, name);
		ASSERT_TRUE(isUniqueName, "there is already a texture by such name: " + name);

		const TexID id = GenerateID();
		const ptrdiff_t insertAtPos = GetPosForVal(ids_, id);

		InsertAtPos(ids_, insertAtPos, id);
		InsertAtPos(names_, insertAtPos, name);
		InsertAtPos(textures_, insertAtPos, tex);

		// return a ptr to the added texture
		return &textures_[insertAtPos];
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't add a texture by name: " + name);
	}
}

///////////////////////////////////////////////////////////

TextureClass* TextureManager::LoadFromFile(const TexPath& path)
{
	// return a ptr to the texture which is loaded from the file by texturePath;
	// 
	// input: path to the texture file;
	// 
	// 1. if such a texture already exists we just return a ptr to it;
	// 2. if there is no texture by such name (path) we try to create it
	
	try
	{
		// 1. if there is such a texture we just return a ptr to it
		if (ArrHasVal(names_, path)) 
			return GetByName(path);

		// 2. OR create a new texture from file
		const TexID id = GenerateID();
		const ptrdiff_t insertAtPos = GetPosForVal(ids_, id);

		InsertAtPos(ids_, insertAtPos, id);
		InsertAtPos(names_, insertAtPos, path);
		InsertAtPos(textures_, insertAtPos, TextureClass(pDevice_, path));

		// return a ptr to the texture obj
		return &textures_[insertAtPos];
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't create a texture from file: " + path);
	}
}

///////////////////////////////////////////////////////////

TextureClass* TextureManager::CreateWithColor(const Color& color)
{
	// if there is already a texture by such ID we just return a ptr to it;
	// or in another case we create this texture and return a ptr to this new texture;

	// generate a name for this texture
	const std::string name{ "color_texture" +
		std::to_string(color.GetR()) + "_" +
		std::to_string(color.GetG()) + "_" +
		std::to_string(color.GetB()) };

	return (ArrHasVal(names_, name)) ? GetByName(name) : Add(name, { pDevice_, color });
}



// ************************************************************************************
//
//                                PUBLIC QUERY API
//
// ************************************************************************************

TextureClass* TextureManager::GetByID(const TexID id)
{
	// return a ptr to the texture by ID or nullptr if there is no such a texture

	return (BinarySearch(ids_, id)) ? &textures_[GetIdxInSortedArr(ids_, id)] : nullptr;
}

///////////////////////////////////////////////////////////

TextureClass* TextureManager::GetByName(const TexName& name)
{
	// return a ptr to the texture by name or nullptr if there is no such a texture
	const ptrdiff_t idx = FindIdxOfVal(names_, name);
	return (idx != names_.size()) ? &textures_[idx] : nullptr;

	//return (ArrHasVal(names_, name)) ? &textures_[FindIdxOfVal(names_, name)] : nullptr;
}

///////////////////////////////////////////////////////////

TexID TextureManager::GetIDByName(const TexName& name)
{
	// return an ID of texture object by input name

	TexIDsArr idsArr;
	GetIDsByNames({ name }, idsArr);
	return idsArr.front();
}

///////////////////////////////////////////////////////////

void TextureManager::GetIDsByNames(const TexNamesArr& names, TexIDsArr& outIDs)
{
	// get data idxs of names
	std::vector<ptrdiff_t> idxs;
	GetDataIdxsByNames(names, idxs);

	// get IDs
	outIDs.reserve(std::ssize(names));

	for (const ptrdiff_t idx : idxs)
		outIDs.push_back(ids_[idx]);
}

///////////////////////////////////////////////////////////

void TextureManager::GetAllTexturesSRVs(
	std::vector<ID3D11ShaderResourceView*>& outSRVs)
{
	// return SRVs (shader resource views) of all the currently loaded textures

	outSRVs.reserve(std::ssize(textures_));

	for (const TextureClass& tex : textures_)
		outSRVs.push_back(tex.GetTextureResourceView());
}

///////////////////////////////////////////////////////////

void TextureManager::GetTexArrByIDs(
	const TexIDsArr& texIDs,
	std::vector<TextureClass*>& outTexPtrs)
{
	// here get arr of ptrs to texture objects by input IDs

	std::vector<ptrdiff_t> idxs;
	GetDataIdxsByIDs(texIDs, idxs);

	// get textures ptrs 
	outTexPtrs.reserve(std::ssize(texIDs));

	for (const ptrdiff_t idx : idxs)
		outTexPtrs.push_back(&textures_[idx]);
}

///////////////////////////////////////////////////////////

void TextureManager::GetSRVsByTexIDs(const TexIDsArr& texIDs, SRVsArr& outSRVs)
{
	// here get SRV (shader resource view) of each input texture by its ID

	std::vector<TextureClass*> texPtrs;
	GetTexArrByIDs(texIDs, texPtrs);

	// get SRVs
	outSRVs.reserve(std::ssize(texIDs));

	for (const TextureClass* pTex : texPtrs)
		outSRVs.push_back(pTex->GetTextureResourceView());
}

///////////////////////////////////////////////////////////

void TextureManager::GetAllTexturesPathsWithinDirectory(
	const std::string& pathToDir,
	TexPathsArr& outPathsToTextures)
{
	// get an array of paths to textures in the directory by pathToDir

	namespace fs = std::filesystem;

	std::set<std::string> extentions{ ".dds", ".tga", ".png", ".bmp" };

	// go through each file in the directory
	for (const fs::directory_entry& entry : fs::directory_iterator(pathToDir))
	{
		const fs::path texturePath = entry.path();
		const std::string ext = texturePath.extension().string();

		// if we have some texture image format
		if (extentions.contains(ext))
		{
			std::string path = StringHelper::ToString(texturePath);
			std::replace(path.begin(), path.end(), '\\', '/');  // in the pass change from '\\' into '/' symbol

			outPathsToTextures.emplace_back(path);
		}
	}
}




// ***********************************************************************************
// 
//                              PRIVATE HELPERS
// 
// ***********************************************************************************

TexID TextureManager::GenerateID()
{
	//
	// generate unique numerical ID
	//

	using engine = std::mt19937;
	std::random_device os_seed;
	const u32 seed = os_seed();
	engine generator(seed);
	std::uniform_int_distribution<u32> distribute(0, UINT_MAX);

	u32 id = distribute(generator);

	// if such ID already exists we generate a new value
	while (std::binary_search(ids_.begin(), ids_.end(), id))
	{
		id = distribute(generator);
	}

	return id;
}

///////////////////////////////////////////////////////////

void TextureManager::GetDataIdxsByIDs(
	const TexIDsArr& texIDs,
	std::vector<ptrdiff_t>& outIdxs)
{
	// check if IDs are valid
	bool idsValid = true;

	for (const TexID id : texIDs)
		idsValid &= (BinarySearch(ids_, id));

	ASSERT_TRUE(idsValid, "there is no texture by some input ID");

	// get idxs by IDs
	outIdxs.reserve(std::ssize(texIDs));

	for (const TexID id : texIDs)
		outIdxs.push_back(GetIdxInSortedArr(ids_, id));
}

///////////////////////////////////////////////////////////

void TextureManager::GetDataIdxsByNames(
	const TexNamesArr& names,
	std::vector<ptrdiff_t>& outIdxs)
{
	// get data idxs of names
	outIdxs.reserve(std::ssize(names));

	for (const TexName& name : names)
		outIdxs.push_back(FindIdxOfVal(names_, name));

	// check if names are valid
	bool namesValid = true;
	const size allNamesCount = std::ssize(names_);

	for (const ptrdiff_t idx : outIdxs)
		namesValid &= (idx < allNamesCount);

	ASSERT_TRUE(namesValid, "there is no texture by some input name:\n" + StringHelper::Join(names));
}

#if 0


///////////////////////////////////////////////////////////

bool TextureManager::InitializeAllTextures(ID3D11Device* pDevice, 
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