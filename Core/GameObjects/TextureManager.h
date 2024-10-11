////////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      TextureManager.h
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
#include <windows.h>
#include <map>

#include "textureclass.h"
#include "TextureHelperTypes.h"





class TextureManager
{
public:
	static const TexID TEX_ID_UNLOADED = 0;
	static const TexID TEX_ID_UNHANDLED = 1;

public:
	TextureManager();
	~TextureManager();

	// restrict a copying of this class instance
	TextureManager(const TextureManager& obj) = delete;
	TextureManager& operator=(const TextureManager& obj) = delete;

	inline static TextureManager* Get() { return pInstance_; }

	// public creation API
	void Initialize(ID3D11Device* pDevice);

	TexID Add(const TexName& name, TextureClass& tex);
	TexID Add(const TexName& name, TextureClass&& tex);
	

	TexID LoadFromFile(const TexPath& path);

	// TODO: FIX IT
	//void LoadFromFile(const std::vector<TexPath>& texPaths, std::vector<TexID>& outTexIDs);

	TexID CreateWithColor(const Color& textureColor);


	// public query API
	TextureClass* GetTexPtrByID(const TexID id);
	TextureClass* GetTexPtrByName(const TexName& name);

	TexID GetIDByName(const TexName& name);
	void GetIDsByNames(const std::vector<TexName>& names, std::vector<TexID>& outIDs);

	inline void GetAllTexturesIDs(std::vector<TexID>& outTexturesIDs) { outTexturesIDs = ids_; }
	void GetAllTexturesSRVs(std::vector<SRV*>& outSRVs);

	void GetTexArrByIDs(const std::vector<TexID>& texIDs, std::vector<TextureClass*>& outTexPtrs);
	void GetSRVsByTexIDs(const std::vector<TexID>& texIDs, std::vector<SRV*>& outSRVs);

	void GetAllTexturesPathsWithinDirectory(
		const std::string& pathToDir,
		std::vector<TexPath>& outPathsToTextures);

private:
	void AddDefault(const TexName& name, TextureClass&& tex, const TexID id);

	TexID GenerateID();
	void GetDataIdxsByIDs(const std::vector<TexID>& texIDs, std::vector<ptrdiff_t>& outIdxs);
	void GetDataIdxsByNames(const std::vector<TexName>& names, std::vector<ptrdiff_t>& outIdxs);

	void UpdateMapIdToSRV();


private:
	const u32 INVALID_TEXTURE_ID = 0;

	static TextureManager* pInstance_;
	ID3D11Device* pDevice_ = nullptr;

	std::map<TexID, SRV*> idToSRV_;

	std::vector<TexID> ids_;             // SORTED array of unique IDs
	std::vector<TexName> names_;         // name (there can be path) which is used for searching of texture
	std::vector<TextureClass> textures_;
};