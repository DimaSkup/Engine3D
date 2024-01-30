/////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     fontclass.h
// Description:  1. this class will handle the texture for the font,
//               the font data from the text file, and the function
//               used to build vertex buffers with the font data.
//               2. the vertex buffers that hold the font data for 
//               individual sentences will be in the TextClass and
//               not inside this class.
//
// Revising:     10.06.22
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <fstream>
#include <DirectXMath.h>
#include <vector>
#include <memory>     // for std::unique_ptr

#include "../../Model/textureclass.h"
#include "../../Model/Vertex.h"
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

public:
	FontClass();
	~FontClass();

	bool Initialize(ID3D11Device* device, 
		ID3D11DeviceContext* pDeviceContext, 
		const std::string & fontDataFilePath,
		const std::string & textureFilename);

	ID3D11ShaderResourceView* const GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

	// builds a vertices array by texture data which is based on 
	// input sentence and upper-left position
	void BuildVertexArray(std::vector<VERTEX> & verticesArr,
		const std::string & sentence, 
		const POINT & drawAt);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:  // restrict a copying of this class instance
	FontClass(const FontClass & obj);
	FontClass & operator=(const FontClass & obj);


private:
	bool LoadFontData(const std::string & fontDataFilename);

private:
	std::unique_ptr<FontType[]> pFont_;
	std::unique_ptr<TextureClass> pTexture_;

	const int charNum_ = 95;                    // the count of characters in the texture
	const UINT fontHeight_ = 32;                // the height of character in pixels
};