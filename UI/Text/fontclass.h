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
#include <d3d11.h>
#include <fstream>
#include <DirectXMath.h>

#include "../../Model/textureclass.h"
#include "../../Engine/Log.h"


//////////////////////////////////
// Class name: FontClass
//////////////////////////////////
class FontClass
{
private:
	// contains data about character on the font texture
	struct FontType
	{
		float left, right; // left and right edge coordinates on the symbol on the texture
		int size;          // symbol width in pixels
	};

	// contains data of vertex (this structure must match with one in the TextClass)
	struct VERTEX
	{
		VERTEX() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

	
public:
	FontClass(void);
	FontClass(const FontClass& copy);
	~FontClass(void);

	bool Initialize(ID3D11Device* device, char* fontDataFilename, WCHAR* textureFilename);
	void Shutdown(void);

	ID3D11ShaderResourceView* GetTexture(void);

	// builds a vertices array by texture data which is based on 
	// input sentence and upper-left position
	void BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:
	bool LoadFontData(char* fontDataFilename);
	void ReleaseFontData(void);
	bool LoadTexture(ID3D11Device* device, WCHAR* textureFilename);
	void ReleaseTexture(void);

private:
	FontType* pFont_ = nullptr;
	TextureClass* pTexture_ = nullptr;
};