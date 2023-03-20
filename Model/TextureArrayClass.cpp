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



// releases each element in the texture array
void TextureArrayClass::Shutdown()
{
	for (auto & elem : texturesArray_)
	{
		_RELEASE(elem->pResource); // release the texture resource
		_DELETE(elem);             // release a texture data object
	}

	// release the arrays with textures names and texture pointers
	if (!texPtrBuffer_.empty())
	{
		texPtrBuffer_.clear();
	}

	return;
}



// add a texture at the end of the textures array
bool TextureArrayClass::AddTexture(ID3D11Device* pDevice, WCHAR* textureFilename)
{
	assert(pDevice != nullptr);
	assert(textureFilename != nullptr);

	ID3D11ShaderResourceView* texture = nullptr;

	// load in the texture
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, textureFilename, nullptr, nullptr, &texture, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the first texture");

	// we push a new texture data object at the end of the textures array
	texturesArray_.push_back(new TextureData());
	COM_ERROR_IF_FALSE(texturesArray_.back(), "can't allocate memory for the texture data object");
	
	// set up the texture data obj
	texturesArray_.back()->pName = textureFilename;
	texturesArray_.back()->pResource = texture;

	return true;
}



// set a texture by some particular index
bool TextureArrayClass::SetTexture(ID3D11Device* pDevice, WCHAR* textureFilename, UINT index)
{
	assert(pDevice != nullptr);
	assert(textureFilename != nullptr);
	assert(index <= texturesArray_.size());


	ID3D11ShaderResourceView* texture = nullptr;

	// load in the texture
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, textureFilename, nullptr, nullptr, &texture, nullptr);
	COM_ERROR_IF_FAILED(hr, "can't load in the first texture");

	// before setting a new texture we must release the previous
	this->RemoveTextureByIndex(index);

	// we push a new texture data object at the end of the textures array
	texturesArray_[index - 1] = new TextureData();
	COM_ERROR_IF_FALSE(texturesArray_.back(), "can't allocate memory for the texture data object");

	// set up the texture data obj
	texturesArray_.back()->pName = textureFilename;
	texturesArray_.back()->pResource = texture;

	return true;
}


void TextureArrayClass::RemoveTextureByIndex(UINT index)
{
	assert(index <= texturesArray_.size());

	_RELEASE(texturesArray_[index - 1]->pResource); // release the texture resource
	_DELETE(texturesArray_[index - 1]);             // release the allocated memory

	return;
}



// get an array of texture data objects
const std::vector<TextureArrayClass::TextureData*> & TextureArrayClass::GetTexturesData() const
{
	return this->texturesArray_;
}

// get an array of pointers to the textures resources
ID3D11ShaderResourceView* const* TextureArrayClass::GetTextureResourcesArray()
{
	texPtrBuffer_.clear();   // clear the textures pointers buffer

	// update the textures pointers buffer
	for (auto & elem : texturesArray_)
	{
		texPtrBuffer_.push_back(elem->pResource);
	}

	return texPtrBuffer_.data(); 
}
