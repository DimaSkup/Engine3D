////////////////////////////////////////////////////////////////////
// Filename:     TextureArrayClass.cpp
// Description:  an implementation of the TextureArrayClass class
// Created:      09.01.23
////////////////////////////////////////////////////////////////////
#include "TextureArrayClass.h"

// the class default constructor initializes the texture array elements to null
TextureArrayClass::TextureArrayClass()
{
	Log::Debug(THIS_FUNC_EMPTY);
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
								   WCHAR* texFilename2,
								   WCHAR* texFilename3)
{
	Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* texture = nullptr;

	// load in the 1st texture
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFilename1, nullptr, nullptr, &texture, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the first texture");
	this->textureArray_.push_back(texture);

	// load in the 2nd texture
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFilename2, nullptr, nullptr, &texture, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the second texture");
	this->textureArray_.push_back(texture);

	// load in the 3rd texture
	if (texFilename3)
	{


		hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFilename3, nullptr, nullptr, &texture, nullptr);
		COM_ERROR_IF_FAILED(hr, "can't load in the third texture");

		this->textureArray_.push_back(texture);
	}

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
