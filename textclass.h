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

#include <DirectXMath.h>

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
		//VERTEX() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	TextClass(void);
	TextClass(const TextClass& copy);
	~TextClass(void);

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd,
		            int screenWidth, int screenHeight, DirectX::XMMATRIX baseViewMatrix);
	void Shutdown(void);
	bool Render(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX, DirectX::XMMATRIX);

	// memory allocation
	void* operator new(size_t i)
	{
		void* ptr = _aligned_malloc(i, 16);
		if (!ptr)
		{
			Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
			return nullptr;
		}

		return ptr;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	bool InitializeSentence(SentenceType**, int maxLength, ID3D11Device* device);
	bool UpdateSentence(SentenceType* sentencePtr, char* text, int posX, int posY, float red, float green, float blue, ID3D11DeviceContext* deviceContext);
	void ReleaseSentence(SentenceType** sentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, DirectX::XMMATRIX world, DirectX::XMMATRIX ortho);

private:
	DirectX::XMMATRIX m_baseViewMatrix;

	FontClass* m_pFont;
	FontShaderClass* m_pFontShader;
	int m_screenWidth, m_screenHeight;
	
	SentenceType* m_pSentence1;
	SentenceType* m_pSentence2;
};