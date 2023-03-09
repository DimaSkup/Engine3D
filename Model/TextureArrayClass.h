////////////////////////////////////////////////////////////////////
// Filename:      TextureArrayClass.h
// Description:   a class for handling initialization of multiple 
//                textures and work with it;
// Created:       09.01.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <d3dx11tex.h>
#include <vector>

#include "../Engine/log.h"

//////////////////////////////////
// Class name: TextureArrayClass
//////////////////////////////////
class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass& copy);
	~TextureArrayClass();

	bool AddTexture(ID3D11Device* pDevice, WCHAR* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* const* GetTextureArray() const;

private:
	std::vector<ID3D11ShaderResourceView*> textureArray_;
};