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

#include "../Engine/macros.h"
#include "../Engine/Log.h"

//////////////////////////////////
// Class name: TextureClass
//////////////////////////////////
class TextureClass
{
public:
	TextureClass(void);
	~TextureClass(void);

	bool Initialize(ID3D11Device* pDevice, WCHAR* textureFilename); // loads texture from a given file

	ID3D11ShaderResourceView* GetTexture(void) const;	   // returns a pointer to the texture resource so it can be used for rendering by shaders
	WCHAR* GetName() const;

private:
	ID3D11ShaderResourceView* pTextureResource_ = nullptr;
	WCHAR* pTextureName_ = nullptr;      // a name of the texture
};