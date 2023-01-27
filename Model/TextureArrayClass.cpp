////////////////////////////////////////////////////////////////////
// Filename:     TextureArrayClass.cpp
// Description:  an implementation of the TextureArrayClass class
// Created:      09.01.23
////////////////////////////////////////////////////////////////////
#include "TextureArrayClass.h"

// the class default constructor initializes the texture array elements to null
TextureArrayClass::TextureArrayClass()
{
}

// the class copy constructor
TextureArrayClass::TextureArrayClass(const TextureArrayClass& copy) 
{
};

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


// AddTexture() takes in a texture file name and creates a texture 
// resource in the texture array from those file
bool TextureArrayClass::AddTexture(ID3D11Device* pDevice, WCHAR* textureFilename)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* texture = nullptr;

	// load in the texture
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, textureFilename, nullptr, nullptr, &texture, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the first texture");
	textureArray_.push_back(texture);

	return true;
}


// releases each element in the texture array
void TextureArrayClass::Shutdown()
{
	// release each texture resource
	for (size_t i = 0; i < textureArray_.size(); i++)
	{
		_RELEASE(textureArray_[i]);
	}

	if (!textureArray_.empty())
		textureArray_.clear();

	return;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return this->textureArray_.data();
}
