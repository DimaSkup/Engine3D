////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureclass.h"

#pragma warning (disable : 4996)

#include <iostream>


TextureClass::TextureClass(void)
{
	try
	{
		pTextureName_ = new WCHAR[100]{ L'\0' };  // maximum symbols = 100
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the texture class elements");
	}
	
}

TextureClass::~TextureClass(void)
{
	_RELEASE(pTextureResource_);   // Release the texture resource
	_DELETE_ARR(pTextureName_);    // release the texture name
}




////////////////////////////////////////////////////////////////////
//
//                       PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////


// Loads the texture file into the shader resource variable called pTextureResource_.
// The texture can now be used to render with
bool TextureClass::Initialize(ID3D11Device* pDevice, const WCHAR* filename)
{
	assert(pDevice != nullptr);
	assert(filename != nullptr);

	std::string textureExt = GetTextureExtension(filename);

	// if we have a DirectDraw Surface (DDS) container format
	if (textureExt == "dds")  
	{
	
	}
	// if we have a Targa file format
	else if (textureExt == "tga")  
	{

	}
	

	

	// initialize the texture name
	wcscpy(pTextureName_, filename);

	return true;
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

// return the width of the texture
UINT TextureClass::GetWidth() const
{
	return 0;
}

UINT TextureClass::GetHeight() const
{
	return 0;
}







////////////////////////////////////////////////////////////////////
//
//                       PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////


bool TextureClass::LoadDDSTexture(const WCHAR* textureFilename, ID3D11Device* pDevice)
{
	// Load the texture in
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, textureFilename,
		nullptr, nullptr,
		&pTextureResource_, nullptr);


	if (FAILED(hr))
	{
		Log::Error("%s() (%d): %s %S", __FUNCTION__, __LINE__,
			"can't create the shader resource view from the file: ", textureFilename);
		return false;
	}

	return true;
}

bool TextureClass::LoadTargaTexture(const WCHAR* textureFilename)
{
	return true;
}


// returns an extension of a file by the textureFilename path
std::string TextureClass::GetTextureExtension(const WCHAR* textureFilename)
{
	std::string ext{ "" };
	std::string strFilename = StringConverter::ToString(textureFilename);
	
	return strFilename.substr(strFilename.find_last_of(".") + 1);
}