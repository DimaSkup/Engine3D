/////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     fontclass.h
// Description:  1. this class will handle the texture for the font,
//               the font data from the text file, and the function
//               used to build vertex buffers with the font data.
//               2. the vertex buffers that hold the font data for 
//               individual sentences will be in the TextStore and
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

#include "../../GameObjects/textureclass.h"
#include "../../GameObjects/Vertex.h"
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

	/////////////////////  Public modification API  /////////////////////
	void Initialize(ID3D11Device* pDevice, 
		const std::string & fontDataFilePath,
		const std::string & textureFilename);

	// builds a vertices array by font texture data which is based on 
	// input sentence and upper-left position
	void BuildVertexArray(
		_Inout_ std::vector<VertexFont> & verticesArr,
		const std::string & sentence,
		const POINT & drawAt);

	// builds an indices array according to the vertices array from the BuildVertexArray func;
	void BuildIndexArray(
		const UINT indicesCount,
		_Inout_ std::vector<UINT> & indicesArr);

	
	/////////////////////  Public query API  /////////////////////
	const UINT GetFontHeight() const;
	ID3D11ShaderResourceView* const GetTextureResourceView();
	ID3D11ShaderResourceView* const* GetTextureResourceViewAddress();

	

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* ptr);

private:  // restrict a copying of this class instance
	FontClass(const FontClass & obj);
	FontClass & operator=(const FontClass & obj);

private:
	void LoadFontData(const std::string & fontDataFilename,
		const size_t numOfFontChar,
		std::vector<FontType> & fontDataArrToInit);

private:
	UINT fontHeight_ = 0;                // the height of character in pixels
	const int charNum_ = 95;             // the count of characters in the texture

	TextureClass fontTexture_;
	std::vector<FontType> fontDataArr_;  // font raw data (position/width of each symbol in the texture, etc.)
};