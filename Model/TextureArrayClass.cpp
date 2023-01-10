////////////////////////////////////////////////////////////////////
// Filename:     TextureArrayClass.cpp
// Description:  an implementation of the TextureArrayClass class
// Created:      09.01.23
////////////////////////////////////////////////////////////////////
#include "TextureArrayClass.h"

// the class default constructor initializes the texture array elements to null
TextureArrayClass::TextureArrayClass()
{
	size_t numOfTextures = 2;

	for (size_t i = 0; i < numOfTextures; i++)
	{
		pTextures[i] = nullptr;
	}
}

// the class copy constructor
TextureArrayClass::TextureArrayClass(const TextureArrayClass& copy) {};

// the class destructor
TextureArrayClass::~TextureArrayClass()
{
	this->Shutdown();
}




////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////


// Initialize() takes in the two texture file names and creates two texture 
// resources in the texture array from those files
bool TextureArrayClass::Initialize(ID3D11Device* pDevice,
								   WCHAR* texFilename1,
								   WCHAR* texFilename2)
{
	HRESULT hr = S_OK;

	// load in the first texture
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFilename1, nullptr, nullptr, &pTextures[0], nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the first texture");

	// load in the second texture
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFilename2, nullptr, nullptr, &pTextures[1], nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the second texture");

	return true;
}


// releases each element in the texture array
void TextureArrayClass::Shutdown()
{
	// release the texture resources
	for (size_t i = 0; i < 2; i++)
		_RELEASE(pTextures[i]);

	return;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return pTextures;
}
