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


// AddTexture() takes in a texture file name, creates a texture 
// resource, and puts it at the back of the texture array
bool TextureArrayClass::AddTexture(ID3D11Device* pDevice, WCHAR* textureFilename)
{
	ID3D11ShaderResourceView* texture = nullptr;

	// load in the texture
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, textureFilename, nullptr, nullptr, &texture, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the first texture");

	// add a pair ['textureName' => 'textureResource'] into the textures map
	textures_.insert({ textureFilename, texture });

	return true;
}


// releases each element in the texture array
void TextureArrayClass::Shutdown()
{
	// release each texture resource
	for (auto & elem : textures_)
	{
		_RELEASE(elem.second);
	}

	// release the arrays with textures names and texture pointers
	if (!textures_.empty())
	{
		textures_.clear();
	}

	return;
}


//ID3D11ShaderResourceView* const* TextureArrayClass::GetTexturesArray() const
const std::map<WCHAR*, ID3D11ShaderResourceView*> TextureArrayClass::GetTexturesData() const
{
	return this->textures_;
}

ID3D11ShaderResourceView* const* TextureArrayClass::GetTexturesArray()
{
	texturesPtrs_.clear();

	for (auto & elem : textures_)
	{
		texturesPtrs_.push_back(elem.second);
	}

	return texturesPtrs_.data();
}
