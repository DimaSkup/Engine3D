////////////////////////////////////////////////////////////////////
// Filename:      TextureArrayClass.h
// Description:   each model has its own array of textures for using;
//
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
#include "../Model/TextureManagerClass.h"


//////////////////////////////////
// Class name: TextureArrayClass
//////////////////////////////////
class TextureArrayClass
{
public:
	TextureArrayClass();
	~TextureArrayClass();

	
	void Shutdown();

	void AddTexture(const WCHAR* textureFilename);             // add a texture at the end of the textures array
	void SetTexture(const WCHAR* textureFilename, const UINT index); // set a texture by some particular index
	
	// getters
	TextureClass* GetTextureByIndex(UINT index) const;
	const std::vector<TextureClass*> & GetTexturesData() const;                     // get an array of texture data objects
	ID3D11ShaderResourceView* const* TextureArrayClass::GetTextureResourcesArray(); // get an array of pointers to the textures resources

	void RemoveTextureByIndex(const UINT index);

private:  // restrict a copying of this class instance
	TextureArrayClass(const TextureArrayClass & obj);
	TextureArrayClass & operator=(const TextureArrayClass & obj);

private:
	std::vector<TextureClass*> texturesArray_;
	std::vector<ID3D11ShaderResourceView*> texPtrBuffer_;  // a buffer for pointers to the texture resources
};