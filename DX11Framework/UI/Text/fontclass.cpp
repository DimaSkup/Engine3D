/////////////////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
/////////////////////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"



FontClass::FontClass()
{
}

FontClass::~FontClass(void) 
{
	Log::Debug(LOG_MACRO);
}



/////////////////////////////////////////////////////////////////////////////////////////////
// 
//                                PUBLIC METHODS 
//
/////////////////////////////////////////////////////////////////////////////////////////////

bool FontClass::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const std::string & fontDataFilePath,
	const std::string & fontTexturePath)
{
	// this function will load the font data and the font texture

	Log::Debug(LOG_MACRO);

	// check input params
	COM_ERROR_IF_NULLPTR(pDevice, "the ptr to the device == nullptr");
	COM_ERROR_IF_NULLPTR(pDeviceContext, "the ptr to the device context == nullptr");
	COM_ERROR_IF_FALSE(!fontTexturePath.empty(), "the input path to texture is empty");
	COM_ERROR_IF_FALSE(!fontDataFilePath.empty(), "the input path to fond data file is empty");

	// ---------------------------------------------------- //

	try
	{
		// initialize a new texture for this font
		pTexture_ = std::make_unique<TextureClass>(pDevice, fontTexturePath, aiTextureType_DIFFUSE);

		// create an array for data about each character of the font
		pFont_ = std::make_unique<FontType[]>(charNum_);

		// load the font data
		bool result = LoadFontData(fontDataFilePath);
		COM_ERROR_IF_FALSE(result, "can't load the font data from the file");

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(LOG_MACRO, "can't allocate memory for the font class members");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize the FontClass object");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

void FontClass::BuildVertexArray(
	_Inout_ std::vector<VERTEX_FONT> & verticesArr,
	_Inout_ std::vector<UINT> & indicesArr,
	const std::string & sentence,
	const POINT & drawAt)
{
	// BuildVertexArray() builds a vertices array by texture data which is based on 
	// input sentence and upper-left position
	// (this function is called by a TextClass object)


	// check input params
	COM_ERROR_IF_FALSE(sentence.empty() != true, "the input sentence is empty");

	size_t strLength = 0;
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
		const int symbol = static_cast<int>(sentence[i]) - 32;

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
			const float left = pFont_[symbol].left;
			const float right = pFont_[symbol].right;
			const float size = static_cast<float>(pFont_[symbol].size);

			const UINT index1 = index;
			const UINT index2 = index + 1;
			const UINT index3 = index + 2;
			const UINT index4 = index + 3;
			
			// vertices for this symbol
			verticesArr[index1].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f); // upper left
			verticesArr[index1].texture = DirectX::XMFLOAT2(left, 0.0f);

			verticesArr[index2].position = DirectX::XMFLOAT3(drawX, drawY - fontHeight_, 0.0f); // bottom left
			verticesArr[index2].texture = DirectX::XMFLOAT2(left, 1.0f);

			verticesArr[index3].position = DirectX::XMFLOAT3(drawX + size, drawY - fontHeight_, 0.0f); // bottom right
			verticesArr[index3].texture = DirectX::XMFLOAT2(right, 1.0f);

			verticesArr[index4].position = DirectX::XMFLOAT3(drawX + size, drawY, 0.0f); // upper right
			verticesArr[index4].texture = DirectX::XMFLOAT2(right, 0.0f);

			// indices for this symbol
			// build indices data
			indicesArr.push_back(
			{
				index1, index2, index4,  // first triangle
				index2, index3, index4,  // second triangle
			});

			// shift the drawing position  by 1 pixel
			drawX += (size + 1.0f);

		} // else
	} // for

	return;
} // end BuildVertexArray

///////////////////////////////////////////////////////////

ID3D11ShaderResourceView* const FontClass::GetTextureResourceView()
{
	// return a pointer to the texture shader resource
	return pTexture_->GetTextureResourceView();
}

ID3D11ShaderResourceView** FontClass::GetTextureResourceViewAddress()
{
	return pTexture_->GetTextureResourceViewAddress();
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

	Log::Get()->Error(LOG_MACRO, "can't allocate the memory for object");
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


bool FontClass::LoadFontData(const std::string & fontDataFilename)
{
	// LoadFontData() loads from the file texture left, right texture coordinates for each symbol
	// and the width in pixels for each symbol

	std::ifstream fin;

	try 
	{
		fin.open(fontDataFilename, std::ifstream::in); // open the file with font data
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
		fin.close();
		Log::Error(LOG_MACRO, "exception opening/reading/closing file\n");
		return false;
	}

	
	Log::Debug(LOG_MACRO);

	return true;

} // end LoadFontData
