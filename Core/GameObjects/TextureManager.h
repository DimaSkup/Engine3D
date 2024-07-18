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

#include "textureclass.h"
#include "TextureHelperTypes.h"





class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	// restrict a copying of this class instance
	TextureManager(const TextureManager& obj) = delete;
	TextureManager& operator=(const TextureManager& obj) = delete;

	inline static TextureManager* Get() { return pInstance_; }

	// public creation API
	void Initialize(ID3D11Device* pDevice);

	TextureClass* Add(const TexName& name, TextureClass& tex);
	TextureClass* Add(const TexName& name, TextureClass&& tex);

	TextureClass* LoadFromFile(const TexPath& path);
	TextureClass* CreateWithColor(const Color& textureColor);


	// public query API
	TextureClass* GetByID(const TexID id);
	TextureClass* GetByName(const TexName& name);

	TexID GetIDByName(const TexName& name);
	void GetIDsByNames(const TexNamesArr& names, TexIDsArr& outIDs);

	inline void GetAllTexturesIDs(std::vector<TexID>& outTexturesIDs) { outTexturesIDs = ids_; }
	void GetAllTexturesSRVs(std::vector<ID3D11ShaderResourceView*>& outSRVs);

	void GetTexArrByIDs(const TexIDsArr& texIDs, std::vector<TextureClass*>& outTexPtrs);
	void GetSRVsByTexIDs(const TexIDsArr& texIDs, SRVsArr& outSRVs);

	void GetAllTexturesPathsWithinDirectory(
		const std::string& pathToDir,
		TexPathsArr& outPathsToTextures);

private:
	TexID GenerateID();
	void GetDataIdxsByIDs(const TexIDsArr& texIDs, std::vector<ptrdiff_t>& outIdxs);
	void GetDataIdxsByNames(const TexNamesArr& names, std::vector<ptrdiff_t>& outIdxs);

private:
	static TextureManager* pInstance_;

	ID3D11Device* pDevice_ = nullptr;

	std::vector<TexID> ids_;             // SORTED array of unique IDs
	std::vector<TexName> names_;         // name (there can be path) which is used for searching of texture
	std::vector<TextureClass> textures_;
};