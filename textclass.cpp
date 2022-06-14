////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass(void)
{
	m_pFont = nullptr;
	m_pFontShader = nullptr;
	
	m_pSentence1 = nullptr;
	m_pSentence2 = nullptr;
}

TextClass::TextClass(const TextClass& copy)
{
}

TextClass::~TextClass(void)
{
}

// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //
bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool hr = S_OK;

	// store the screen width and heihgt
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the base view matrix
	m_baseViewMatrix = baseViewMatrix;

	// create the font object
	m_pFont = new(std::nothrow) FontClass;
	if (!m_pFont)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the FontClass object");
		return false;
	}

	return true;
}






// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //