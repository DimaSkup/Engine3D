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
	if (!texturesArray_.empty())
	{
		texturesArray_.clear();   // just clear up this texture array
	}

	if (!texPtrBuffer_.empty())
	{
		texPtrBuffer_.clear();
	}

	return;
}


// add a texture at the end of the textures array
bool TextureArrayClass::AddTexture(const WCHAR* textureFilename)
{
	assert((textureFilename != nullptr) && (textureFilename != L'\0'));

	TextureClass* pTexture = TextureManagerClass::Get()->GetTexture(textureFilename);

	// we push a texture object at the end of the textures array
	texturesArray_.push_back(pTexture);

	return true;
}


// set a texture by some particular index
bool TextureArrayClass::SetTexture(const WCHAR* textureFilename, const  UINT index)
{
	assert((textureFilename != nullptr) && (textureFilename != L'\0'));
	assert(index <= texturesArray_.size());

	bool result = false;
	TextureClass* pTexture = TextureManagerClass::Get()->GetTexture(textureFilename);

	// set a texture object by particular index
	texturesArray_[index - 1] = pTexture;

	return true;
}


// get an array of texture objects
const std::vector<TextureClass*> & TextureArrayClass::GetTexturesData() const
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
		texPtrBuffer_.push_back(elem->GetTexture());
	}

	return texPtrBuffer_.data(); 
}


// remove a texture by some particular index
void TextureArrayClass::RemoveTextureByIndex(const UINT index)
{
	assert(index <= texturesArray_.size());

	texturesArray_[index - 1] = nullptr;

	return;
}