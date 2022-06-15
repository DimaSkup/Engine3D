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
	bool result = false;

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

	// initialize the font object
	result = m_pFont->Initialize(device, L"data/fontdata.txt", L"data/font.dds");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font object");
	}

	// create the font shader object
	m_pFontShader = new(std::nothrow) FontShaderClass;
	if (!m_pFontShader)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the FontShaderClass object");
		return false;
	}

	// initalize the font shader object
	result = m_pFontShader->Initialize(device, hwnd);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font shader object");
		return false;
	}


	// --------------------- CREATION OF SENTENCES ----------------------------------- //

	// initialize the first sentence
	result = InitializeSentence(&m_pSentence1, 16, device);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the first sentence");
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_pSentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the first sentence");
		return false;
	}



	// initialize the second sentence
	result = InitializeSentence(&m_pSentence2, 16, device);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the second sentence");
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_pSentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the second sentence");
		return false;
	}

	return true;
} // Initialize()


// The Shutdown() will release the sentences, the font object, and the font shader object
void TextClass::Shutdown(void)
{
	ReleaseSentence(&m_pSentence1);  // release the first sentence
	ReleaseSentence(&m_pSentence2);  // release the second sentence
	
	_SHUTDOWN(m_pFontShader);        // release the font shader object
	_SHUTDOWN(m_pFont);              // release the font object

	return;
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, 
	                   D3DXMATRIX worldMatrix, 
	                   D3DXMATRIX orthoMatrix)
{
	bool result = false;

	// draw the first sentence
	result = RenderSentence(deviceContext, m_pSentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the first sentence");
		return false;
	}

	// draw the second sentence
	result = RenderSentence(deviceContext, m_pSentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the second sentence");
		return false;
	}

	return true;
}



// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

// The InitializeSentence() creates a SentenceType with an empty vertex buffer which will
// be used to store and render sentences. The maxLenght input parameters determines
// how large the vertex buffer will be. All sentences have a vertex and index buffer
// associated with them which is initialize first in this function
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VERTEX* vertices = nullptr;
	ULONG* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr = S_OK;
	int i = 0;

	// create a new sentence object
	*sentence = new(std::nothrow) SentenceType;
	if (!*sentence)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate memory for a new sentence object");
		return false;
	}

	return true;
}