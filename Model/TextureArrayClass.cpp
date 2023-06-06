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
	for (auto & elem : texturesArray_)
	{
		_DELETE(elem);             // release a texture object
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

	bool result = false;
	TextureClass* pTexture = nullptr;

	try
	{
		// create and initialize a texture object
		pTexture = new TextureClass();
		
		result = pTexture->Initialize(pDevice, textureFilename);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture class object");
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for a texture class object");
	}

	// we push a new texture object at the end of the textures array
	texturesArray_.push_back(pTexture);
	

	return true;
}



// set a texture by some particular index
bool TextureArrayClass::SetTexture(ID3D11Device* pDevice, WCHAR* textureFilename, UINT index)
{
	assert(pDevice != nullptr);
	assert(textureFilename != nullptr);
	assert(index <= texturesArray_.size());

	bool result = false;
	TextureClass* pTexture = nullptr;

	try
	{
		// create and initialize a new texture object
		pTexture = new TextureClass();
		COM_ERROR_IF_FALSE(pTexture, "can't allocate memory for a texture class object");

		result = pTexture->Initialize(pDevice, textureFilename);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture class object");
	}
	catch (COMException & e)
	{
		Log::Error(THIS_FUNC, e);
		_DELETE(pTexture);  // release memory from the texture object
		return false;
	}

	// before setting a new texture we must release the previous
	this->RemoveTextureByIndex(index);

	// set a texture object by particular index
	texturesArray_[index - 1] = pTexture;

	return true;
}


void TextureArrayClass::RemoveTextureByIndex(UINT index)
{
	assert(index <= texturesArray_.size());

	Log::Error("%S", texturesArray_[index - 1]->GetTexture());

	_DELETE(texturesArray_[index - 1]); // release memory in textures array from the texture object

	return;
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
