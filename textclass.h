////////////////////////////////////////////////////////////////////
// Filename:     textclass.h
// Description:  handles all the 2D text drawing that the application
//               will need. It renders 2D text to the screen.
//               1. holds the font data and constructs vertex buffers
//               which are needed for rendering strings;
//               2. uses FontClass to create the vertex buffer for strings
//               and then uses FontShaderClass to render this buffer;
//
// Revising:     10.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "fontclass.h"
#include "fontshaderclass.h"
#include "log.h"

//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass
{

private:
	// SentenceType structure is the structure that holds the rendering information 
	// for reach text sentence
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	// The VERTEX must match the one in the FontClass
	struct VERTEX
	{
		VERTEX() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TextClass(void);
	TextClass(const TextClass& copy);
	~TextClass(void);

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd,
		            int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX, D3DXMATRIX);

private:
	bool InitializeSentence(SentenceType**, int maxLength, ID3D11Device* device);
	bool UpdateSentence(SentenceType* sentencePtr, char* text, int posX, int posY, float red, float green, float blue, ID3D11DeviceContext* deviceContext);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
	FontClass* m_pFont;
	FontShaderClass* m_pFontShader;
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;

	SentenceType* m_pSentence1;
	SentenceType* m_pSentence2;
};