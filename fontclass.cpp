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
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	VERTEX* verticesPtr = static_cast<VERTEX*>(vertices); // cast the vertices array
	int strLength = 0, symbol = 0;

	// define the length of the input sentence
	strLength = static_cast<int>(strlen(sentence));

	// go through each symbol of the input sentence
	for (size_t i = 0; i < strLength; i++)
	{
		symbol = static_cast<int>(sentence[i]) - 32;

		if (symbol == ' ') // if there is a space
		{
			drawX += 3.0f; // skip 3 pixels
		}
		else  // else we build a polygon for this symbol 
		{
			// first triangle

		}
	}

	return;
}





























/*
// Initialize() will load the font data and the font texture
bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result = false;



	// load in the text file containing the font data
	result = LoadFontData(fontFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't load font data");
		return false;
	}


	// load the texture that has the font characters on it
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't load the font texture");
		return false;
	}

	return true;
}


// Shutdown() will release the font data and the font texture
void FontClass::Shutdown(void)
{
	ReleaseTexture();  // release the font texture
	ReleaseFontData(); // release the font data

	return;
}

// The BuildVertexArray() will be called by the TextClass to build vertex buffers out 
// of the text sentence it sends to this function as input.
// 1. a vertices variable is a pointer to the vertex array
// 2. a sentence variable is the text sentence that will be used to create the vertex array
// 3. drawX and drawY are the screen coordinates of where to draw the sentence
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	//Log::Get()->Debug(THIS_FUNC_EMPTY);
	Log::Get()->Debug(THIS_FUNC, sentence);

	VERTEX* vertexPtr = static_cast<VERTEX*>(vertices);
	int numLetters, index, letter;

	// get the number of letters in the sentence
	numLetters = static_cast<int>(strlen(sentence));

	// initialize the index to the vertex array
	index = 0;

	// draw each letter onto a quad
	for (size_t i = 0; i < numLetters; i++)
	{
		letter = (static_cast<int>(sentence[i])) - 32;

		// if the letter is a space then just move over three pixels
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// first triangle in quad
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // top left
			vertexPtr[index].texture  = DirectX::XMFLOAT2(m_pFont[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX + m_pFont[letter].size, (drawY - 16), 0.0f); // bottom right
			vertexPtr[index].texture  = DirectX::XMFLOAT2(m_pFont[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, (drawY - 16), 0.0f); // bottom left
			vertexPtr[index].texture  = DirectX::XMFLOAT2(m_pFont[letter].left, 1.0f);
			index++;

			// second triangle in quad
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // top left
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_pFont[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX + m_pFont[letter].size, drawY, 0.0f); // top right
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_pFont[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + m_pFont[letter].size), (drawY - 16), 0.0f); // bottom right
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_pFont[letter].right, 1.0f);
			index++;

			// update the x location for drawing by the size of the letter and one pixel
			drawX += (m_pFont[letter].size + 1.0f);
		} // else
	} // for

	return;
} // BuildVertexArray()


// returns the texture resource
ID3D11ShaderResourceView* FontClass::GetTexture(void)
{
	return m_pTexture->GetTexture();
}


*/

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

// The LoadFontData() is where we load the fontdata.txt file which contains the indexing
// information for the texture
bool FontClass::LoadFontData(char* filename)
{
	std::ifstream fin;
	int charNum = 95; // number of characters in the texture
	char temp;

	// create the font spacing buffer
	m_pFont = new(std::nothrow) FontType[charNum];
	if (!m_pFont)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the font spacing buffer");
		return false;
	}

	// read in the font size and spacing between chars
	fin.open(filename);
	if (fin.fail())
	{
		Log::Get()->Error(THIS_FUNC, "can't open the font indexing file");
		return false;
	}

	// read in the ascii characters for text
	for (size_t i = 0; i < charNum; i++)
	{
		fin.get(temp);
		while (temp != ' ')  // skip ASCII value of character
		{
			fin.get(temp);
		}

		fin.get(temp);
		while (temp != ' ')  // skip the character
		{
			fin.get(temp);
		}

		fin >> m_pFont[i].left;     // read in the LEFT  texture U coordinate
		fin >> m_pFont[i].right;    // read in the RIGHT texture U coordinate
		fin >> m_pFont[i].size;     // read in the pixel width of character
	}

	// close the file
	fin.close();

	return true;
}  // LoadFontData()


// The ReleaseFontData() releases the array that holds the texture indexing data
void FontClass::ReleaseFontData(void)
{
	_DELETE(m_pFont);  // release the font data array

	return;
}


// The LoadTexture() reads in the font.dds file into the texture shader resource.
bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result = false;

	// create the texture object
	m_pTexture = new(std::nothrow) TextureClass;
	if (!m_pTexture)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the texture object");
		return false;
	}

	// initialize the texture object
	result = m_pTexture->Initialize(device, filename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture object");
		return false;
	}

	return true;
} // LoadTexture()


// The ReleaseTexture() releases the texture that was used for the font
void FontClass::ReleaseTexture(void)
{
	_SHUTDOWN(m_pTexture);  // release the texture object

	return;
}