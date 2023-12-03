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
#include <d3dx11tex.h>
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
private:
	// we define the Targa file header structure here to 
	// make reading in the data easier (for .tga format)
	struct TargaHeader
	{
		UCHAR data1[12]{ '\0' };
		UCHAR bpp{ '\0' };
		UCHAR data2{ '\0' };

		USHORT width = 0;
		USHORT height = 0;
	};


public:
	TextureClass();
	TextureClass(ID3D11Device* pDevice, const Color & color, aiTextureType type);
	TextureClass(ID3D11Device* pDevice, const Color* pColorData, UINT width, UINT height, aiTextureType type);
	TextureClass::TextureClass(const TextureClass & src);
	~TextureClass();

	// loads texture from a given file
	bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const WCHAR* filename); 

	TextureClass & operator=(const TextureClass & src);

	ID3D11ShaderResourceView*  GetTextureResourceView() const;  
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

	aiTextureType GetType() const;
	WCHAR* GetName() const;
	UINT GetWidth() const;     // return the width of the texture
	UINT GetHeight() const;    // return the height of the texture
	POINT TextureClass::GetTextureSize();

private:
	void Initialize1x1ColorTexture(ID3D11Device* pDevice, const Color & colorData, aiTextureType type);
	void InitializeColorTexture(ID3D11Device* pDevice, const Color* pColorData, UINT width, UINT height, aiTextureType type);

	bool LoadDDSTexture(const WCHAR* filename, ID3D11Device* pDevice);
	bool LoadTargaTexture(const WCHAR* filename, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool LoadTarga32Bit(const char* filename);
	std::string GetTextureExtension(const WCHAR* filename);

private:
	UCHAR* pTargaData_ = nullptr;                           // holds the raw Targa data read straight in from the file
	WCHAR* pTextureName_ = nullptr;                         // a name of the texture

	ID3D11Resource* pTexture_ = nullptr;
	//ID3D11Texture2D* pTexture_ = nullptr;                   // holds the structured texture data that DirectX will use for rendering
	ID3D11ShaderResourceView* pTextureView_ = nullptr;      // a resource view that the shader uses to access the texture data when drawing

	UINT textureWidth_ = 50;                                // dimensions of the texture 
	UINT textureHeight_ = 50;
	aiTextureType type_ = aiTextureType::aiTextureType_UNKNOWN;
};