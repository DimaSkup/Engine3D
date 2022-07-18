////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
////////////////////////////////////////////////////////////////////
#include "fontclass.h"

FontClass::FontClass(void)
{
	m_pFont = nullptr;
	m_pTexture = nullptr;
}

FontClass::FontClass(const FontClass& copy) {}

FontClass::~FontClass(void) {}

// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //

// Initialize() will load the font data and the font texture
bool FontClass::Initialize(ID3D11Device* device, 
	                       char* fontDataFilename,
	                       WCHAR* textureFilename)
{
	bool result = false;

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// load the font data
	result = LoadFontData(fontDataFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't load the font data from the file");
		return false;
	}


	// load the texture
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't load the texture");
		return false;
	}

	return true;
}


// Shutdown() will release the memory from the font data and font texture
void FontClass::Shutdown(void)
{
	ReleaseTexture();
	ReleaseFontData();

	return;
}


// BuildVertexArray() builds a vertices array by texture data which is based on 
// input sentence and upper-left position
// (this function is called by the TextClass object)
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	VERTEX* verticesPtr = static_cast<VERTEX*>(vertices); // cast the vertices array
	int strLength = 0, symbol = 0;
	int index = 0;                    // initialize the index for the vertex array
	int fontHeight = 16;              // the height of this font

	// define the length of the input sentence
	strLength = static_cast<int>(strlen(sentence));

	// go through each symbol of the input sentence
	for (size_t i = 0; i < strLength; i++)
	{
		symbol = static_cast<int>(sentence[i]) - 32;

		if (symbol == 0) // if there is a space
		{
			drawX += 3.0f; // skip 3 pixels
		}
		else  // else we build a polygon for this symbol 
		{
			// the symbol texture params
			float left = m_pFont[symbol].left;
			float right = m_pFont[symbol].right;
			float size = static_cast<float>(m_pFont[symbol].size);


			// first triangle in quad
			verticesPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesPtr[index].texture  = DirectX::XMFLOAT2(left, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight, 0.0f); // bottom right
			verticesPtr[index].texture  = DirectX::XMFLOAT2(right, 1.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX, drawY - fontHeight, 0.0f); // bottom left
			verticesPtr[index].texture = DirectX::XMFLOAT2(left, 1.0f);
			index++;


			// second triangle in quad
			verticesPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesPtr[index].texture = DirectX::XMFLOAT2(left, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX + size, drawY, 0.0f); // upper right
			verticesPtr[index].texture = DirectX::XMFLOAT2(right, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight, 0.0f); // bottom right
			verticesPtr[index].texture = DirectX::XMFLOAT2(right, 1.0f);
			index++;


			drawX += (size + 1.0f);
		} // else
	} // for

	return;
}


// GetTexture() return a pointer to the texture resource
ID3D11ShaderResourceView* FontClass::GetTexture(void)
{
	return m_pTexture->GetTexture();
}


// memory allocation
// any FontClass object is aligned on 16 in the memory
void* FontClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void FontClass::operator delete(void* p)
{
	_aligned_free(p);
}


// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

// LoadFontData() loads from the file texture left, right texture coordinates for each symbol
// and the width in pixels for each symbol
bool FontClass::LoadFontData(char* filename)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	std::ifstream fin;
	int charNum = 95;  // the count of characters in the texture

	// allocate the memory for the font data
	m_pFont = new(std::nothrow) FontType[charNum];
	if (!m_pFont)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the font data array");
		return false;
	}

	fin.open(filename); // open the file with font data
	if (fin.fail())
	{
		Log::Get()->Error(THIS_FUNC, "can't open the file with font data");
		return false;
	}


	// read in data from the file
	for (size_t i = 0; i < charNum; i++)
	{
		while (fin.get() != ' ') {}  // skip the ASCII-code of the character
		while (fin.get() != ' ') {}  // skip the character


		// read in the character font data
		fin >> m_pFont[i].left;
		fin >> m_pFont[i].right;
		fin >> m_pFont[i].size;
	}


	fin.close(); // close the file 

	return true;
} // LoadFontData()


// The ReleaseFontData() releases the array that holds the texture indexing data
void FontClass::ReleaseFontData(void)
{
	_DELETE(m_pFont); // release the font data array

	return;
}


// The LoadTexture() reads in the font.dds file into the texture shader resource
bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* textureFilename)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// create a texture object
	m_pTexture = new(std::nothrow) TextureClass();
	if (!m_pTexture)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the texture class object");
		return false;
	}

	// initialize the texture class object
	result = m_pTexture->Initialize(device, textureFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture class object");
		return false;
	}

	return true;
}


// The ReleaseTexture() releases the texture that was used for the font
void FontClass::ReleaseTexture(void)
{
	_SHUTDOWN(m_pTexture); // release the texture object

	return;
}