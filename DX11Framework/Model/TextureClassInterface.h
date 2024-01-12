#pragma once

#include <d3d11.h>
#include <assimp/material.h>

#include "../Render/Color.h"






class TextureClassInterface
{
public:
	virtual ~TextureClassInterface() {};

	virtual TextureClassInterface & operator=(const TextureClassInterface & src) = 0;
	virtual ID3D11ShaderResourceView*  GetTextureResourceView() const = 0;
	virtual ID3D11ShaderResourceView** GetTextureResourceViewAddress() = 0;

	//WCHAR* GetName() const;
	virtual aiTextureType GetType() const = 0;
	virtual UINT GetWidth()  const = 0;                // return the width of the texture
	virtual UINT GetHeight() const = 0;                // return the height of the texture
	virtual POINT GetTextureSize() = 0;                // x = width; y = height
};