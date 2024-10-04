/////////////////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
/////////////////////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"

#include <sstream>
#include "../Common/Assert.h"


FontClass::FontClass() : fontDataArr_(charNum_) 
{
}

FontClass::~FontClass() 
{
	Log::Debug();
}



///////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////

void FontClass::Initialize(
	ID3D11Device* pDevice,
	const std::string & fontDataFilePath,
	const std::string & fontTexturePath)
{
	// this function will load the font data and the font texture

	Log::Debug();

	try
	{
		// check input params
		const bool inputDataValid = (!fontDataFilePath.empty() & (!fontTexturePath.empty()));
		Assert::NotZero(inputDataValid, "the input data is INVALID!");

		// initialize a texture for this font
		fontTexture_ = TextureClass(pDevice, fontTexturePath);

		// we need to have a height of the font texture for proper building of the vertices data
		fontHeight_ = fontTexture_.GetHeight();

		// load the data into the font data array
		LoadFontData(fontDataFilePath, fontDataArr_.size(), fontDataArr_);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		throw EngineException("can't initialize the FontClass object");
	}
}

///////////////////////////////////////////////////////////

void FontClass::BuildVertexArray(
	_Inout_ std::vector<VertexFont> & verticesArr,
	const std::string & sentence,
	const POINT & drawAt)
{
	// BuildVertexIndexArrays() builds a vertices array by texture data which is based on 
	// input sentence and upper-left position
	// (this function is called by a TextStore object)

	int index = 0;                    // initialize the index for the vertex array
	float drawX = static_cast<float>(drawAt.x);
	const float topY = static_cast<float>(drawAt.y);
	const float bottomY = topY - fontHeight_;

	const std::vector<FontType> & fontData = fontDataArr_;

	// go through each character of the input sentence
	for (const int ch : sentence)
	{
		const int symbol = ch - 32;

		// if there is a space (symbol == 0)
		if (!symbol) 
		{
			drawX += 3.0f; // skip 3 pixels
			continue;
		}
		// else we build a polygon for this symbol 
		else  
		{
			// the symbol texture params
			const float texLeft = fontData[symbol].left;
			const float texRight = fontData[symbol].right;
			const float width = static_cast<float>(fontData[symbol].size);


			// top left
			verticesArr[index].position = DirectX::XMFLOAT2(drawX, topY);
			verticesArr[index].texture = DirectX::XMFLOAT2(texLeft, 0.0f);

			// bottom right
			verticesArr[index + 1].position = DirectX::XMFLOAT2(drawX + width, bottomY);
			verticesArr[index + 1].texture = DirectX::XMFLOAT2(texRight, 1.0f);

			// bottom left
			verticesArr[index + 2].position = DirectX::XMFLOAT2(drawX, bottomY);
			verticesArr[index + 2].texture = DirectX::XMFLOAT2(texLeft, 1.0f);

			// top right
			verticesArr[index + 3].position = DirectX::XMFLOAT2(drawX + width, topY);
			verticesArr[index + 3].texture = DirectX::XMFLOAT2(texRight, 0.0f);

			index += 4;

			// shift the drawing position  by 1 pixel
			drawX += (width + 1.0f);

		} // else
	} // for

	return;
}

///////////////////////////////////////////////////////////

void FontClass::BuildIndexArray(
	const UINT indicesCount,
	_Inout_ std::vector<UINT> & indicesArr)
{
	// NOTE: the input indices array must be empty before initialization
	assert((indicesArr.empty() & (bool)indicesCount) && "wrong input data");

	for (UINT v_idx = 0, arr_idx = 0; arr_idx < indicesCount; arr_idx += 6)
	{
		indicesArr.insert(indicesArr.end(),  // insert 6 indices at this position
		{
			v_idx, v_idx+1, v_idx+2,  // first triangle 
			v_idx, v_idx+3, v_idx+1,  // second triangle
		});

		v_idx += 4;  // stride by 4 (the number of vertices in symbol)
	}
}

///////////////////////////////////////////////////////////

void* FontClass::operator new(size_t i)
{
	// memory allocation
	// any FontClass object is aligned on 16 in the memory

	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error("can't allocate the memory for object");
	throw std::bad_alloc{};
}

void FontClass::operator delete(void* p)
{
	_aligned_free(p);
}




///////////////////////////////////////////////////////////////////////////////////////////////
//                               PUBLIC QUERY API
///////////////////////////////////////////////////////////////////////////////////////////////

const UINT FontClass::GetFontHeight() const
{
	return fontHeight_;
}

ID3D11ShaderResourceView* const FontClass::GetTextureResourceView()
{
	// return a pointer to the texture shader resource
	return fontTexture_.GetTextureResourceView();
}

ID3D11ShaderResourceView* const* FontClass::GetTextureResourceViewAddress()
{
	return fontTexture_.GetTextureResourceViewAddress();
}





////////////////////////////////////////////////////////////////////////////////////////////
//                             PRIVATE MODIFICATION API 
////////////////////////////////////////////////////////////////////////////////////////////

void FontClass::LoadFontData(const std::string & fontDataFilename,
	const size_t numOfFontChar,
	std::vector<FontType> & fontDataArrToInit)
{
	// LoadFontData() loads from the file texture left, right texture coordinates for each symbol
	// and the width in pixels of each symbol

	std::ifstream fin;

	try 
	{
		fin.open(fontDataFilename, std::ifstream::in);
		Assert::True(fin.is_open(), "can't open the file with font data");
		
		// create a temporal buffer for font data
		std::vector<FontType> fontData(numOfFontChar);

		// read in data from the buffer
		for (size_t i = 0; i < numOfFontChar - 2; i++)
		{
			// skip the ASCII-code of the character and the character itself
			while (fin.get() != ' ') {}
			while (fin.get() != ' ') {}

			// read in the character font data
			fin >> fontData[i].left;
			fin >> fontData[i].right;
			fin >> fontData[i].size;
		}

		// copy the font data from the temporal buffer
		fontDataArrToInit = fontData;

		fin.close();
	}
	catch (std::ifstream::failure e)
	{
		fin.close();
		throw EngineException("exception opening/reading/closing file");
	}
	catch (EngineException & e)
	{
		fin.close();
		Log::Error(e, false);
		throw EngineException("can't load the font data from the file");
	}

	return;

} // end LoadFontData
