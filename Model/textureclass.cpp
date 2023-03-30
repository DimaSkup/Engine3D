
////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureclass.h"
#include <iostream>

#pragma warning (disable : 4996)

TextureClass::TextureClass(void)
{
	pTextureResource_ = nullptr;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass(void)
{
}


// Loads the texture file into the shader resource variable called pTextureResource_.
// The texture can now be used to render with
bool TextureClass::Initialize(ID3D11Device* pDevice, WCHAR* filename)
{
	assert(pDevice);
	assert(filename);

	// Load the texture in
	 HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, filename,
                                                         nullptr, nullptr,
                                                         &pTextureResource_, nullptr);
	if (FAILED(hr))
	{
		Log::Error("%s() (%d): %s %S", __FUNCTION__, __LINE__, 
			"can't create the shader resource view from the file: ", filename);
		return false;
	}

	// initialize the texture name
	pTextureName_ = filename;


	//Log::Get()->Debug("%s()::%d: %S texture file is uploaded", __FUNCTION__, __LINE__, filename);


	return true;
}

// Releases the texture resource if it has been loaded and then sets the poitner to nullptr
void TextureClass::Shutdown(void)
{
	// Release the texture resource
	_RELEASE(pTextureResource_);
	_DELETE(pTextureName_);

	return;
}

// Called by other objects that need access to the texture shader resource so that
// they can use the texture for rendering
ID3D11ShaderResourceView* TextureClass::GetTexture() const
{
	return pTextureResource_;
}

WCHAR* TextureClass::GetName() const
{
	return pTextureName_;
}