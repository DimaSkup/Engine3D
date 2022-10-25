
////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////
#include "textureclass.h"
#pragma warning (disable : 4996)

TextureClass::TextureClass(void)
{
	m_texture = nullptr;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass(void)
{
}


// Loads the texture file into the shader resource variable called m_texture.
// The texture can now be used to render with
bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT hr = S_OK;

	// Load the texture in
	hr = D3DX11CreateShaderResourceViewFromFile(device, filename, 
                                                nullptr, nullptr,
                                                &m_texture, nullptr);
	if (FAILED(hr))
	{
		Log::Get()->Error("%s() (%d): %s %S", __FUNCTION__, __LINE__, 
			"can't create the shader resource view from the file: ", filename);
		return false;
	}


	Log::Get()->Debug("%s()::%d: %S texture file is uploaded", __FUNCTION__, __LINE__, filename);


	return true;
}

// Releases the texture resource if it has been loaded and then sets the poitner to nullptr
void TextureClass::Shutdown(void)
{
	// Release the texture resource
	_RELEASE(m_texture);

	return;
}

// Called by other objects that need access to the texture shader resource so that
// they can use the texture for rendering
ID3D11ShaderResourceView* TextureClass::GetTexture(void)
{
	return m_texture;
}