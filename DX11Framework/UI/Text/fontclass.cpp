////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
////////////////////////////////////////////////////////////////////
#include "fontclass.h"

FontClass::FontClass(void)
{
	try
	{
		pTexture_ = new TextureClass();
		pFont_ = new FontType[charNum_];
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the font class elements");
	}
}


FontClass::~FontClass(void) 
{
	Log::Debug(THIS_FUNC_EMPTY);

	_DELETE(pTexture_);  // release the texture that was used for the font
	_DELETE_ARR(pFont_); // release the font data array
}

// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //

// Initialize() will load the font data and the font texture
bool FontClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	char* fontDataFilename,
	WCHAR* textureFilename)
{
	Log::Debug(THIS_FUNC_EMPTY);
	
	try
	{
		// load the font data
		bool result = LoadFontData(fontDataFilename);
		COM_ERROR_IF_FALSE(result, "can't load the font data from the file");

		// load the texture
		result = AddTextures(pDevice, pDeviceContext, textureFilename);
		COM_ERROR_IF_FALSE(result, "can't load the texture");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize the FontClass object");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void FontClass::BuildVertexArray(std::vector<VERTEX> & verticesArr,
	const std::string & sentence,
	const POINT & drawAt)
{
	// BuildVertexArray() builds a vertices array by texture data which is based on 
	// input sentence and upper-left position
	// (this function is called by a TextClass object)


	// check input params
	COM_ERROR_IF_FALSE(sentence.empty() != true, "the input sentence is empty");

	size_t strLength = 0;
	int symbol = 0;
	int index = 0;                    // initialize the index for the vertex array
	float drawX = static_cast<float>(drawAt.x);
	float drawY = static_cast<float>(drawAt.y);

	// define the length of the input sentence
	strLength = sentence.length();

	// check if the vertices array has the same length as the input sentence
	if (verticesArr.size() < strLength)
		verticesArr.resize(strLength);

	// go through each symbol of the input sentence
	for (size_t i = 0; i < strLength; i++)
	{
		symbol = static_cast<int>(sentence[i]) - 32;

		// if there is a space
		if (symbol == 0) 
		{
			drawX += 3.0f; // skip 3 pixels
			continue;
		}
		// else we build a polygon for this symbol 
		else  
		{
			// the symbol texture params
			float left = pFont_[symbol].left;
			float right = pFont_[symbol].right;
			float size = static_cast<float>(pFont_[symbol].size);
			

			// first triangle in quad
			verticesArr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesArr[index].texture  = DirectX::XMFLOAT2(left, 0.0f);
			index++;

			verticesArr[index].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight_, 0.0f); // bottom right
			verticesArr[index].texture  = DirectX::XMFLOAT2(right, 1.0f);
			index++;

			verticesArr[index].position = DirectX::XMFLOAT3(drawX, drawY - fontHeight_, 0.0f); // bottom left
			verticesArr[index].texture = DirectX::XMFLOAT2(left, 1.0f);
			index++;


			// second triangle in quad
			verticesArr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesArr[index].texture = DirectX::XMFLOAT2(left, 0.0f);
			index++;

			verticesArr[index].position = DirectX::XMFLOAT3(drawX + size, drawY, 0.0f); // upper right
			verticesArr[index].texture = DirectX::XMFLOAT2(right, 0.0f);
			index++;

			verticesArr[index].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight_, 0.0f); // bottom right
			verticesArr[index].texture = DirectX::XMFLOAT2(right, 1.0f);
			index++;

			// shift the drawing position
			drawX += (size + 1.0f);

		} // else
	} // for

	return;
} // end BuildVertexArray

///////////////////////////////////////////////////////////

ID3D11ShaderResourceView* const FontClass::GetTextureResourceView(void)
{
	// return a pointer to the texture shader resource
	return pTexture_->GetTextureResourceView();
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

	Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

void FontClass::operator delete(void* p)
{
	_aligned_free(p);
}





////////////////////////////////////////////////////////////////////////////////////////////
// 
//                               PRIVATE METHODS 
//
////////////////////////////////////////////////////////////////////////////////////////////

// LoadFontData() loads from the file texture left, right texture coordinates for each symbol
// and the width in pixels for each symbol
bool FontClass::LoadFontData(char* filename)
{
	Log::Debug(THIS_FUNC_EMPTY);

	// check if filename is empty
	assert((filename != nullptr) && (filename[0] != '\0'));

	
	try 
	{
		std::ifstream fin;

		fin.open(filename, std::ifstream::in); // open the file with font data
		if (fin.good())
		{
			// read in data from the file
			for (size_t i = 0; i < charNum_ - 2; i++)
			{
				while (fin.get() != ' ') {}  // skip the ASCII-code of the character
				while (fin.get() != ' ') {}  // skip the character

											 // read in the character font data
				fin >> pFont_[i].left;
				fin >> pFont_[i].right;
				fin >> pFont_[i].size;
			}

			fin.close(); // close the file 
		}
		else  // we can't open the file
		{
			COM_ERROR_IF_FALSE(false, "can't open the file with font data");
		}
	}
	catch (std::ifstream::failure e)
	{
		Log::Error(THIS_FUNC, "exception opening/reading/closing file\n");
		return false;
	}
	
	Log::Debug(THIS_FUNC_EMPTY);
	return true;
} // end LoadFontData

///////////////////////////////////////////////////////////

bool FontClass::AddTextures(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	WCHAR* textureFilename)
{
	// The AddTextures() reads in a texture dds file into the texture shader resource

	Log::Debug(THIS_FUNC_EMPTY);

	assert(pDevice != nullptr);
	assert((textureFilename != nullptr) && (textureFilename != L'\0'));

	// initialize the texture class object
	bool result = pTexture_->Initialize(pDevice, pDeviceContext, textureFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the texture class object");

	return true;
}
