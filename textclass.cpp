//////////////////////////////////
// Filename: textclass.cpp
//////////////////////////////////
#include "textclass.h"

TextClass::TextClass()
{
	m_Font = nullptr;
	m_FontShader = nullptr;

	m_sentence1 = nullptr;
	m_sentence2 = nullptr;
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}



bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd,
							int screenWidth, int screenHeight,
							D3DXMATRIX baseViewMatrix)
{
	bool result;

	// Store the screen width and height
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Initialize the font object
	result = m_Font->Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/fond.dss");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object", L"Error", MB_OK);
		return false;
	}



	// Initialize the first sentence
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Initialize the second sentence
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// Release the first sentence
	ReleaseSentence(&m_sentence1);

	// Release the second  sentence
	ReleaseSentence(&m_sentence2);

	// Release the font shader object
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = nullptr;
	}

	// Release the font object
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = nullptr;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	// Draw the first sentence
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Draw the second sentence
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
}