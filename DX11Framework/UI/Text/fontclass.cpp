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




// BuildVertexArray() builds a vertices array by texture data which is based on 
// input sentence and upper-left position
// (this function is called by the TextClass object)
void FontClass::BuildVertexArray(void* vertices, 
	const char* sentence, 
	float drawX, 
	float drawY)
{
	assert(vertices != nullptr);
	assert((sentence != nullptr) && (sentence[0] != '\0'));

	VERTEX_FONT* verticesPtr = static_cast<VERTEX_FONT*>(vertices); // cast the vertices array
	int strLength = 0, symbol = 0;
	int index = 0;                    // initialize the index for the vertex array
	

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
			float left = pFont_[symbol].left;
			float right = pFont_[symbol].right;
			float size = static_cast<float>(pFont_[symbol].size);
			

			// first triangle in quad
			verticesPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesPtr[index].texture  = DirectX::XMFLOAT2(left, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight_, 0.0f); // bottom right
			verticesPtr[index].texture  = DirectX::XMFLOAT2(right, 1.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX, drawY - fontHeight_, 0.0f); // bottom left
			verticesPtr[index].texture = DirectX::XMFLOAT2(left, 1.0f);
			index++;


			// second triangle in quad
			verticesPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesPtr[index].texture = DirectX::XMFLOAT2(left, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX + size, drawY, 0.0f); // upper right
			verticesPtr[index].texture = DirectX::XMFLOAT2(right, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight_, 0.0f); // bottom right
			verticesPtr[index].texture = DirectX::XMFLOAT2(right, 1.0f);
			index++;

			drawX += (size + 1.0f);
		} // else
	} // for

	verticesPtr = nullptr;

	return;
}


// GetTexture() return a pointer to the texture resource
ID3D11ShaderResourceView* const FontClass::GetTexture(void)
{
	return pTexture_->GetTexture();
}


// memory allocation
// any FontClass object is aligned on 16 in the memory
void* FontClass::operator new(size_t i)
{
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


// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

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
} // LoadFontData()





// The AddTextures() reads in the font.dds file into the texture shader resource
bool FontClass::AddTextures(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	WCHAR* textureFilename)
{
	Log::Debug(THIS_FUNC_EMPTY);

	assert(pDevice != nullptr);
	assert((textureFilename != nullptr) && (textureFilename != L'\0'));

	// initialize the texture class object
	bool result = pTexture_->Initialize(pDevice, pDeviceContext, textureFilename);
	COM_ERROR_IF_FALSE(result, "can't initialize the texture class object");

	return true;
}
