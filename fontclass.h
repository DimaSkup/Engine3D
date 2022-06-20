////////////////////////////////////////////////////////////////////
// Filename:     fontclass.h
// Description:  1. this class will handle the texture for the font,
//               the font data from the text file, and the function
//               used to build vertex buffers with the font data.
//               2. the vertex buffers that hold the font data for 
//               individual sentences will be in the TextClass and
//               not inside this class.
//
// Revising:     10.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <fstream>

#include "includes.h"
#include "log.h"
#include "textureclass.h"


//////////////////////////////////
// Class name: FontClass
//////////////////////////////////
class FontClass
{
private:
	struct FontType        // hold the indexing data read in from the font index file
	{
		float left, right; // TU texture coordinates
		int size;          // width of the symbol in pixels
	};

	struct VERTEX
	{
		VERTEX() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	FontClass(void);
	FontClass(const FontClass& copy);
	~FontClass(void);

	bool Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename);
	void Shutdown(void);

	ID3D11ShaderResourceView* GetTexture(void);

	// will handle building and returning a vertex array of triangles that will render the 
	// character sentence which was given as input to this function;
	void BuildVertexArray(VERTEX* vertices, char* sentence, float, float); 

private:

private:
	bool LoadFontData(char* fontIndexFilename);
	void ReleaseFontData(void);
	bool LoadTexture(ID3D11Device* device, WCHAR* texture);
	void ReleaseTexture(void);

private:
	FontType* m_pFont;
	TextureClass* m_pTexture; 
};  // FontClass
