
////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureclass.h"
#include <iostream>

#pragma warning (disable : 4996)

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


// Loads the texture file into the shader resource variable called pTextureResource_.
// The texture can now be used to render with
bool TextureClass::Initialize(ID3D11Device* pDevice, WCHAR* filename)
{
	assert(pDevice != nullptr);
	assert(filename != nullptr);

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