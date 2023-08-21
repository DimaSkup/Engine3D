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
	// ATTENTION: we don't use HERE the delete operator to TextureClass objects because we 
	// delete them only in the textures manager
	if (!texturesArray_.empty())
	{
		texturesArray_.clear();   // just clear up this texture array
	}

	if (!shaderResPtrsBuffer_.empty())
	{
		shaderResPtrsBuffer_.clear();
	}

	return;
}


// add a texture at the end of the textures array
void TextureArrayClass::AddTexture(const WCHAR* textureFilename)
{
	assert((textureFilename != nullptr) && (textureFilename != L'\0'));

	// use the texture manager to get a pointer to the texture object
	TextureClass* pTexture = TextureManagerClass::Get()->GetTexture(textureFilename);

	// we push a texture object at the end of the textures array
	texturesArray_.push_back(pTexture);

	this->UpdateShaderResourcesPtrsBuffer();

	return;
}


// set a texture by some particular index
void TextureArrayClass::SetTexture(const WCHAR* textureFilename, const  UINT index)
{
	assert((textureFilename != nullptr) && (textureFilename != L'\0'));
	
	
	// if the asked index value is bigger that textures array size we just add 
	// this new texture at the end of the textures array
	if (index >= texturesArray_.size())
	{
		this->AddTexture(textureFilename);
	}
	// the asked index is in the proper range
	else
	{
		// use the texture manager to get a pointer to the texture object
		TextureClass* pTexture = TextureManagerClass::Get()->GetTexture(textureFilename);

		// set a texture object by particular index
		texturesArray_[index] = pTexture;
	}

	this->UpdateShaderResourcesPtrsBuffer();
	
	return;
}



//
//  GETTERS
//


// get a pointer to the texture object by particular index
TextureClass* TextureArrayClass::GetTextureByIndex(UINT index) const
{
	assert(index < texturesArray_.size());

	return this->texturesArray_[index];
}

// get a pointer to the shader resource view of the texture by index
ID3D11ShaderResourceView* const TextureArrayClass::GetTextureResourceByIndex(UINT index) const
{
	assert(index < texturesArray_.size());

	return this->texturesArray_[index]->GetTexture();
}

// get an array of texture objects
const std::vector<TextureClass*> & TextureArrayClass::GetTexturesData() const
{
	return this->texturesArray_;
}


// get an array of pointers to the textures resources
ID3D11ShaderResourceView* const* TextureArrayClass::GetTextureResourcesArray()
{

	return shaderResPtrsBuffer_.data(); 
}


// remove a texture by some particular index
void TextureArrayClass::RemoveTextureByIndex(const UINT index)
{
	assert(index <= texturesArray_.size());

	texturesArray_[index] = nullptr;

	return;
}




////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////


// each time when we change the array with pointers to TextureClass 
// we should also call this function to update the shader resources pointers buffer
void TextureArrayClass::UpdateShaderResourcesPtrsBuffer()
{
	// clear the shader resources pointers buffer
	shaderResPtrsBuffer_.clear();   

	// update the shader resources pointers buffer
	for (auto & elem : texturesArray_)
	{
		shaderResPtrsBuffer_.push_back(elem->GetTexture());
	}

	return;
}
