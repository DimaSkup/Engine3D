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
#include <d3dx11tex.h>
#include "includes.h"
#include "log.h"

//////////////////////////////////
// Class name: TextureClass
//////////////////////////////////
class TextureClass
{
public:
	TextureClass(void);
	TextureClass(const TextureClass&);
	~TextureClass(void);

	bool Initialize(ID3D11Device* device, WCHAR* textureFilename); // loads texture from a given file
	void Shutdown(void);                           // unloads texture when it is no longer needed

	ID3D11ShaderResourceView* GetTexture(void);	   // returns a pointer to the texture resource so it can be used for rendering by shaders

private:
	ID3D11ShaderResourceView* m_texture;
};