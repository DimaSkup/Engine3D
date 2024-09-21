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

#include <d3d11.h>
#include <assimp/material.h>

#include "../Common/Types.h"
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
	static const u32 TEXTURE_TYPE_COUNT = 22;     // AI_TEXTURE_TYPE_MAX + 1

	struct TexDimensions
	{
		UINT width = 0;
		UINT height = 0;
	};

public:
	TextureClass();

	// a constructor for loading textures from the disk
	TextureClass(
		ID3D11Device* pDevice,
		const std::string & filePath);

	// init texture obj with preloaded values
	TextureClass(
		const std::string& path,
		ID3D11Resource* pTexture,
		ID3D11ShaderResourceView* pTextureView,
		const UINT width,
		const UINT height);	

	// make 1x1 texture with single color
	TextureClass(
		ID3D11Device* pDevice, 
		const Color & color);

	// make width_x_height texture with color data
	TextureClass(
		ID3D11Device* pDevice, 
		const Color* pColorData,
		const UINT width,
		const UINT height);

	// a constructor for loading embedded compressed textures 
	TextureClass(
		ID3D11Device* pDevice,
		const std::string& path,
		const uint8_t* pData,
		const size_t size);

	// copy constructor
	TextureClass(const TextureClass & src);

	~TextureClass();

	// -----------------------------------------------------------------------------------//

	TextureClass & operator=(const TextureClass & src);

	inline ID3D11ShaderResourceView* GetTextureResourceView() const { return pTextureView_; }
	inline ID3D11ShaderResourceView* const* GetTextureResourceViewAddress() const { return &pTextureView_; }

	inline const std::string& GetPath() const { return path_; }
	inline UINT GetWidth()  const { return width_; }
	inline UINT GetHeight() const { return height_; }

	POINT GetTextureSize();

	// set where to store this texture if it was generated
	inline void SetPath(const std::string& newPath)  { path_ = newPath; } 

	// -----------------------------------------------------------------------------------//


private:
	void LoadFromFile(
		ID3D11Device* pDevice, 
		const std::string & filePath);

	void Initialize1x1ColorTexture(
		ID3D11Device* pDevice, 
		const Color & colorData);

	void InitializeColorTexture(
		ID3D11Device* pDevice, 
		const Color* pColorData, 
		const UINT width, 
		const UINT height);

private:

	// a name/path of the texture
	std::string path_ {"no_path"};                       

	ID3D11Resource* pTexture_ = nullptr;
	ID3D11ShaderResourceView* pTextureView_ = nullptr;   

	// dimensions of the texture 
	UINT width_ = 30;                                   
	UINT height_ = 30;
};