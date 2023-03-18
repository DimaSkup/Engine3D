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
#include <map>
#include <vector>

#include "../Engine/log.h"

//////////////////////////////////
// Class name: TextureArrayClass
//////////////////////////////////
class TextureArrayClass
{
private:


public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass& copy);
	~TextureArrayClass();

	void Shutdown();

	bool AddTexture(ID3D11Device* pDevice, WCHAR* textureFilename);
	bool RemoveTexture(WCHAR* textureName);

	ID3D11ShaderResourceView* const* TextureArrayClass::GetTexturesArray();
	const std::map<WCHAR*, ID3D11ShaderResourceView*> GetTexturesData() const;

private:
	std::map<WCHAR*, ID3D11ShaderResourceView*> textures_;
	std::vector<ID3D11ShaderResourceView*> texturesPtrs_;
};