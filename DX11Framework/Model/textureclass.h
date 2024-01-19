////////////////////////////////////////////////////////////////////
// Filename:      textureclass.h
// Description:   Encapsulates the loading, unloading, and accessing
//                of a single texture resource. For each texture 
//                needed an object of this class to be instantiated.
//
// Revising:      09.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>


#include <vector>
#include <memory>
#include <d3d11.h>
#include <assimp/material.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"
#include "../Render/Color.h"


//#include "TextureClassInterface.h"



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
	// make texture from file
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

	// copy constructor
	TextureClass(const TextureClass & src);

	~TextureClass();

	// -----------------------------------------------------------------------------------//

	TextureClass & operator=(const TextureClass & src);
	//virtual TextureClass & operator=(const TextureClass & src) override;

	ID3D11ShaderResourceView*  GetTextureResourceView()  const;
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

	//WCHAR* GetName() const;
	aiTextureType GetType() const;
	UINT GetWidth()         const;     // return the width of the texture
	UINT GetHeight()        const;     // return the height of the texture
	POINT GetTextureSize();

	void SetType(aiTextureType newType);  // change a type of the texture

	// -----------------------------------------------------------------------------------//


private:
	// loads texture from a given file
	bool InitializeTextureFromFile(ID3D11Device* pDevice, const std::string & filePath, aiTextureType type);

	void Initialize1x1ColorTexture(ID3D11Device* pDevice, const Color & colorData, aiTextureType type);
	void InitializeColorTexture(ID3D11Device* pDevice, const Color* pColorData, UINT width, UINT height, aiTextureType type);

private:
	//WCHAR* pTextureName_ = nullptr;                         // a name of the texture

	ID3D11Resource* pTexture_ = nullptr;
	ID3D11ShaderResourceView* pTextureView_ = nullptr;      // a resource view that the shader uses to access the texture data when drawing

	UINT textureWidth_ = 50;                                // dimensions of the texture 
	UINT textureHeight_ = 50;
	aiTextureType type_ = aiTextureType::aiTextureType_UNKNOWN;
};