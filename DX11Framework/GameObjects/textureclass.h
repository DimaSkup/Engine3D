// *********************************************************************************
// Filename:      textureclass.h
// Description:   Encapsulates the loading, unloading, and accessing
//                of a single texture resource. For each texture 
//                needed an object of this class to be instantiated.
//
// Revising:      09.04.22
// *********************************************************************************
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////

#include <vector>
#include <memory>
#include <d3d11.h>
#include <assimp/material.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Render/Color.h"


enum class TextureStorageType
{
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};


//////////////////////////////////
// Class name: TextureClass
//////////////////////////////////
class TextureClass
{
public:
	TextureClass();

	// a constructor for loading textures from the disk
	TextureClass(ID3D11Device* pDevice,
		const std::string & filePath,
		const aiTextureType type);

	// make 1x1 texture with single color
	TextureClass(ID3D11Device* pDevice, 
		const Color & color, 
		const  aiTextureType type);

	// make width_x_height texture with color data
	TextureClass(ID3D11Device* pDevice, 
		const Color* pColorData,
		const UINT width,
		const UINT height,
		const aiTextureType type);

	// a constructor for loading embedded compressed textures 
	TextureClass(ID3D11Device* pDevice,
		const uint8_t* pData,
		const size_t size,
		const aiTextureType type);

	// copy constructor
	TextureClass(const TextureClass & src);

	~TextureClass();

	// -----------------------------------------------------------------------------------//

	TextureClass & operator=(const TextureClass & src);

	ID3D11ShaderResourceView*  GetTextureResourceView()  const;
	ID3D11ShaderResourceView* const* GetTextureResourceViewAddress() const;

	aiTextureType GetType() const;
	UINT GetWidth()         const;        // return the width of the texture
	UINT GetHeight()        const;        // return the height of the texture
	POINT GetTextureSize();

	void SetType(aiTextureType newType);  // change a type of the texture

	// -----------------------------------------------------------------------------------//


private:
	// loads texture from a given file
	void InitializeTextureFromFile(ID3D11Device* pDevice, const std::string & filePath, const aiTextureType type);

	void Initialize1x1ColorTexture(ID3D11Device* pDevice, const Color & colorData, const aiTextureType type);
	void InitializeColorTexture(ID3D11Device* pDevice, const Color* pColorData, const UINT width, UINT height, aiTextureType type);

private:
	//WCHAR* pTextureName_ = nullptr;                          // a name of the texture

	ID3D11Resource* pTexture_ = nullptr;
	ID3D11ShaderResourceView* pTextureView_ = nullptr;         // a resource view that the shader uses to access the texture data when drawing

	UINT textureWidth_ = 30;                                   // dimensions of the texture 
	UINT textureHeight_ = 30;

	aiTextureType type_ = aiTextureType::aiTextureType_UNKNOWN;
};