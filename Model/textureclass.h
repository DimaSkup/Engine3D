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
private:
	// we define the Targa file header structure here to 
	// make reading in the data easier (for .tga format)
	struct TargaHeader
	{
		UCHAR data1[12]{ '\0' };
		USHORT width = 0;
		USHORT height = 0;
		UCHAR bpp{ '\0' };
		UCHAR data2{ '\0' };
	};
public:
	TextureClass(void);
	~TextureClass(void);

	// loads texture from a given file
	bool Initialize(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext, 
		const WCHAR* filename); 

	ID3D11ShaderResourceView* const GetTexture(void) const;   // returns a pointer to the texture resource so it can be used for rendering by shaders
	WCHAR* GetName() const;
	UINT GetWidth() const;     // return the width of the texture
	UINT GetHeight() const;    // return the height of the texture

private:
	bool LoadDDSTexture(const WCHAR* filename, ID3D11Device* pDevice);
	bool LoadTargaTexture(const WCHAR* filename, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	bool LoadTarga32Bit(const char* filename);
	std::string GetTextureExtension(const WCHAR* filename);

private:
	UCHAR* pTargaData_ = nullptr;                           // holds the raw Targa data read straight in from the file
	ID3D11Texture2D* pTexture_ = nullptr;                   // holds the structured texture data that DirectX will use for rendering
	ID3D11ShaderResourceView* pTextureResource_ = nullptr;  // a resource view that the shader uses to access the texture data when drawing
	WCHAR* pTextureName_ = nullptr;                         // a name of the texture
	UINT textureWidth_ = 50;                                // dimensions of the texture 
	UINT textureHeight_ = 50;
};