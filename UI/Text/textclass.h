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
#include "../../ShaderClass/fontshaderclass.h"
#include "../../Engine/Log.h"

#include "../../Model/Vertex.h"
#include "../../Model/VertexBuffer.h"
#include "../../Model/IndexBuffer.h"

#include <memory>
#include <DirectXMath.h>
//#include <DirectXPackedVector.h>  // is necessary for making XMCOLOR structures




//////////////////////////////////
// DATA STRUCTURES
//////////////////////////////////

// The SentenceType is the structure that holds all the data for rendering
struct SentenceType
{
	VertexBuffer<VERTEX_FONT> vertexBuf;
	IndexBuffer               indexBuf;
	std::string text;                       // a text content
	size_t  maxLength;
	int posX, posY;                         // the left upper position on the screen
	float red, green, blue;                 // colour of the sentence
};

// contains the line of text, its upper left position on the screen and RGB-colour values
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



//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass
{
public:
	TextClass();
	~TextClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		int screenWidth, int screenHeight,
		int stringSize,                        // maximal size of the string
		FontClass* pFont,                      // font for the text
		const char* textContent,               // the content of the text
		int posX, int posY,                    // position of the text in the window
		float red, float green, float blue);   // colour of the text

	bool Render(ID3D11DeviceContext* deviceContext, 
		        DirectX::XMMATRIX worldMatrix, 
		        DirectX::XMMATRIX orthoMatrix);

	// adds by particular key a new sentence for output onto the screen or updates a sentence by key with new text data;
	bool SetSentenceByKey(std::string key, std::string text, int posX, int posY, float red, float green, float blue);
	bool CreateSentenceByKey(SentenceType** ppSentence, std::string key, std::string text, int posX, int posY, float red, float green, float blue);

	// memory allocation
	void* operator new(size_t i);
	void  operator delete(void* ptr);

private:  // restrict a copying of this class instance
	TextClass(const TextClass & obj);
	TextClass & operator=(const TextClass & obj);

private:
	bool BuildEmptySentence(SentenceType** ppSentence, size_t maxLength);  // first of all we create an empty sentence (with empty vertices data) and after we update this sentence with text data

	bool UpdateSentence(SentenceType* pSentence, std::string text, int posX, int posY, float red, float green, float blue);
	bool UpdateSentenceVertexBuffer(SentenceType* sentence, std::string text, int posX, int posY);
	
	bool RenderSentence(ID3D11DeviceContext* deviceContext, 
		                SentenceType* pSentence,
		                DirectX::XMMATRIX worldMatrix, 
		                DirectX::XMMATRIX orthoMatrix);
	
private:
	FontClass* pFont_ = nullptr;
	FontShaderClass* pFontShader_ = nullptr;;

	int screenWidth_ = 0;
	int screenHeight_ = 0;
	SentenceType* pSentence_ = nullptr;
};