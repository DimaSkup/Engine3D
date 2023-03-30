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
#include "../Model/textureclass.h"

//////////////////////////////////
// Class name: TextureArrayClass
//////////////////////////////////
class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass& copy);
	~TextureArrayClass();

	void Shutdown();

	bool AddTexture(ID3D11Device* pDevice, WCHAR* textureFilename);             // add a texture at the end of the textures array
	bool SetTexture(ID3D11Device* pDevice, WCHAR* textureFilename, UINT index); // set a texture by some particular index
	void RemoveTextureByIndex(UINT index);


	const std::vector<TextureClass*> & GetTexturesData() const;                     // get an array of texture data objects
	ID3D11ShaderResourceView* const* TextureArrayClass::GetTextureResourcesArray(); // get an array of pointers to the textures resources

private:
	std::vector<TextureClass*> texturesArray_;
	std::vector<ID3D11ShaderResourceView*> texPtrBuffer_;  // a buffer for pointers to the texture resources
};