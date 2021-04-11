//////////////////////////////////
// Filename: fontclass.cpp
//////////////////////////////////
#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the text file containing the font data
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// Load the texture that has the font characters on it
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return false;
}

bool FontClass::LoadFontData(char* filename)
{

}