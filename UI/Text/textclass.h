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

#include <map>
#include <memory>
#include <vector>
#include <DirectXMath.h>
//#include <DirectXPackedVector.h>  // is necessary for making XMCOLOR structures


//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass
{
public:
	// The SentenceType is the structure that holds all the data for rendering
	struct SentenceType
	{
		VertexBuffer<VERTEX_FONT> vertexBuf;
		IndexBuffer               indexBuf;
		std::string text;                       // a text content
		size_t vertexCount, indexCount, maxLength;
		int posX, posY;                   // the left upper position on the screen
		float red, green, blue;           // colour of the sentence
	};

	

private:
	

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



public:
	TextClass();
	TextClass(const TextClass& copy);
	~TextClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd,
		            int screenWidth, int screenHeight, 
		            DirectX::XMMATRIX baseViewMatrix);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, 
		        DirectX::XMMATRIX worldMatrix, 
		        DirectX::XMMATRIX orthoMatrix);

	//bool CreateOrUpdateSentenceByKey(std::string key, std::string text, int posX, int posY, float red, float green, float blue);
	bool SetSentenceByKey(std::string key, std::string text, 
		                  int posX, int posY,
		                  float red, float green, float blue);
	

	

	// memory allocation
	void* operator new(size_t i);
	void  operator delete(void* ptr);

private:
	bool BuildEmptySentence(SentenceType** ppSentence, size_t maxLength);

	bool UpdateSentenceVertexBuffer(SentenceType* sentence, std::string text, int posX, int posY);
	bool UpdateSentence(SentenceType* pSentence, std::string text,
		                int posX, int posY,
		                float red, float green, float blue);
	void ReleaseSentence(SentenceType** ppSentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, 
		                SentenceType* pSentence,
		                DirectX::XMMATRIX worldMatrix, 
		                DirectX::XMMATRIX orthoMatrix);
	
private:
	DirectX::XMMATRIX baseViewMatrix_;

	// intenral copies to the device and device context because they are used too often
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;

	FontClass* pFont_ = nullptr;
	FontShaderClass* pFontShader_ = nullptr;;

	std::map<std::string, TextClass::SentenceType*> sentences_;
	std::vector<RawSentenceLine*> rawSentences_; // a vector of raw sentences lines

	int screenWidth_ = 0;
	int screenHeight_ = 0;
	size_t sentencesCount_ = 0;
	size_t maxStringSize_ = 50;
};