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

#include <map>
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
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
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

	//bool CreateOrUpdateSentenceByKey(std::string key, std::string text, int posX, int posY, float red, float green, float blue);
	bool SetSentenceByKey(std::string key, std::string text, 
		                  int posX, int posY,
		                  float red, float green, float blue);

	

	// memory allocation
	void* operator new(size_t i);
	void  operator delete(void* ptr);

private:
	bool BuildSentence(SentenceType** ppSentence, size_t maxLength);
	/*
	, char* text,
	int posX, int posY,
	float red, float green, float blue
	*/
		 
	//bool UpdateSentenceByIndex(size_t index, char* newText,
	//	int posX = NULL, int posY = NULL,
	//	float red = NULL, float green = NULL, float blue = NULL);

	//bool UpdateSentenceContent(SentenceType* pSentence, char* newText);
	//bool UpdateSentencePosition(SentenceType* pSentence, int posX = NULL, int posY = NULL);
	//bool UpdateSentenceColor(SentenceType* pSentence, float red = NULL, float green = NULL, float blue = NULL);

	bool UpdateSentenceVertexBuffer(SentenceType* sentence, std::string text, int posX, int posY);

	bool UpdateSentence(SentenceType* pSentence, std::string text,
		                int posX, int posY,
		                float red, float green, float blue);
	void ReleaseSentence(SentenceType** ppSentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, 
		                SentenceType* pSentence,
		                DirectX::XMMATRIX worldMatrix, 
		                DirectX::XMMATRIX orthoMatrix);

	//bool InitializeRawSentenceLine(char* str, int posX, int posY);

	bool ReadInTextFromFile(const char* textDataFilename);

	
private:
	DirectX::XMMATRIX m_baseViewMatrix;

	// intenral copies to the device and device context
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	FontClass* m_pFont;
	FontShaderClass* m_pFontShader;
	int m_screenWidth, m_screenHeight;

	std::map<std::string, TextClass::SentenceType*> sentences;
	std::map<std::string, POINT> sentencesPos;
	//std::vector<SentenceType*> m_sentencesVector; // a vector of pointers to sentences structures
	std::vector<RawSentenceLine*> m_rawSentencesVector; // a vector of raw sentences lines
	size_t m_sentencesCount;
	size_t m_maxStringSize;



	// ------------------------------ debug data ----------------------------------- //

	// an indeces of the debug string in the sentences vector variable
	size_t m_cpuLineIndex;
	size_t m_fpsLineIndex;
	size_t m_indexMouseXPos;
	size_t m_indexMouseYPos;
	size_t m_indexDisplayWHParams;
	size_t m_cameraLineIndex;

	// debug strings upper left coordinates
	POINT m_cpuLinePos;
	POINT m_fpsLinePos;
	POINT m_mouseXLinePos;
	POINT m_mouseYLinePos;
	POINT m_displayWHParamsLinePos;
	POINT m_cameraOrientationLinePos;
};