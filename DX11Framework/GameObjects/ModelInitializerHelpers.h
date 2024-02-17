///////////////////////////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializerHelper.h
// Description:   contains private helpers for the ModelInitializer class;
// 
// Created:       16.02.24
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "textureclass.h"


TextureStorageType DetermineTextureStorageType(const aiScene* pScene,
	aiMaterial* pMaterial,
	const UINT index,
	const aiTextureType textureType)
{
	// this function determines all the possible texture storage types

	if (pMaterial->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	// get path to the texture
	aiString path;
	pMaterial->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();

	// check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSES TEXTURES" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}

	// check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSES TEXTURES" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}

	// lastly check if texture is a filepath by check for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None;   // no texture exists

} 

///////////////////////////////////////////////////////////

UINT GetIndexOfEmbeddedCompressedTexture(aiString* pStr)
{
	// this function returns an index of the embedded compressed texture by path pStr

	assert(pStr->length >= 2);             // assert that path is "*0", "*1", or something like that
	return (UINT)atoi(&pStr->C_Str()[1]);  // return an index
}