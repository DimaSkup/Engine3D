////////////////////////////////////////////////////////////////////
// Filename:     textclass.h
// Description:  handles all the 2D text drawing that the application
//               will need. It renders 2D text to the screen.
//               It uses FontClass to create the vertex buffer for strings
//               and then uses FontShaderClass to render this buffer;
//
// Revising:     04.06.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "fontclass.h"
#include "fontshaderclass.h"
#include "log.h"

#include <vector>
#include <DirectXMath.h>


//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass
{
private:
	// The SentenceType is the structure that holds all the data for rendering
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		size_t vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct RawSentenceLine
	{
		RawSentenceLine(char* str, int n_posX, int n_posY, 
			            float n_red, float n_green, float n_blue)
		{
			size_t strLen = strlen(str);

			string = new(std::nothrow) char[strLen + 1]; // +1 because of null-terminator '\0'
			assert(string != nullptr);
			memcpy(string, str, strLen);
			string[strLen] = '\0';

			posX = n_posX;
			posY = n_posY;

			red = n_red;
			green = n_green;
			blue = n_blue;
		}

		char* string;
		int posX, posY;
		float red, green, blue;
	};

	// The VERTEX structure must match the one in the FontClass
	struct VERTEX
	{
		VERTEX() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f) {}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass& copy);
	~TextClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd,
		            int screenWidth, int screenHeight, 
		            const char* textDataFilename,
		            DirectX::XMMATRIX baseViewMatrix);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, 
		        DirectX::XMMATRIX worldMatrix, 
		        DirectX::XMMATRIX orthoMatrix);

	void* operator new(size_t i);
	void  operator delete(void* ptr);



private:
	bool InitializeSentence(SentenceType** ppSentence, size_t maxLength, ID3D11Device* device);
	bool UpdateSentence(SentenceType* pSentence, char* text,
		                int posX, int posY,
		                float red, float green, float blue,
		                ID3D11DeviceContext* deviceContext);
	void ReleaseSentence(SentenceType** ppSentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, 
		                SentenceType* pSentence,
		                DirectX::XMMATRIX worldMatrix, 
		                DirectX::XMMATRIX orthoMatrix);

	bool InitializeRawSentenceLine(char* str, int posX, int posY);

	bool ReadInTextFromFile(const char* textDataFilename);
private:
	DirectX::XMMATRIX m_baseViewMatrix;

	FontClass* m_pFont;
	FontShaderClass* m_pFontShader;
	int m_screenWidth, m_screenHeight;

	SentenceType** m_ppSentences;
	std::vector<RawSentenceLine*> m_pRawSentencesData;
	size_t m_sentencesCount;
	size_t m_maxStringSize;
};