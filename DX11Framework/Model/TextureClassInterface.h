#pragma once

#include <d3d11.h>
#include <assimp/material.h>

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



class TextureClassInterface
{
public:
	TextureClassInterface(ID3D11Device* pDevice, const std::string & filePath, aiTextureType type);
	TextureClassInterface(ID3D11Device* pDevice, const Color & color, aiTextureType type);
	TextureClassInterface(ID3D11Device* pDevice, const Color* pColorData, UINT width, UINT height, aiTextureType type);

	TextureClassInterface(const TextureClassInterface & src);

	virtual ~TextureClassInterface();

	virtual TextureClass & operator=(const TextureClass & src) = 0;
	virtual ID3D11ShaderResourceView*  GetTextureResourceView() const = 0;
	virtual ID3D11ShaderResourceView** GetTextureResourceViewAddress() = 0;

	virtual aiTextureType GetType() const = 0;
	//WCHAR* GetName() const;
	virtual UINT GetWidth() const = 0;                 // return the width of the texture
	virtual UINT GetHeight() const = 0;                // return the height of the texture
	virtual POINT GetTextureSize() = 0;  // x = width; y = height
};