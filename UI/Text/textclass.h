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
class SentenceType
{
public:
	SentenceType(int stringSize,               // maximal size of the string
		const char* textContent,               // the content of the text
		int posX, int posY,                    // upper left position of the text in the window
		float red, float green, float blue)    // colour of the text
		: text_(textContent),
		  maxLength_(stringSize),
		  pos_(static_cast<float>(posX), static_cast<float>(posY)), 
		  color_(red, green, blue, 1.0f)
	{
		try
		{
			pVertexBuffer_ = new VertexBuffer<VERTEX_FONT>();
			pIndexBuffer_ = new IndexBuffer();
		}
		catch (std::bad_alloc & e)
		{
			Log::Error(THIS_FUNC, e.what());
			COM_ERROR_IF_FALSE(false, "can't allocate memory for the sentence type elements");
		}
	}

	~SentenceType()
	{
		_DELETE(pVertexBuffer_);
		_DELETE(pIndexBuffer_);
	}

	// load up vertices data
	HRESULT InitializeVertexBuffer(ID3D11Device* pDevice, VERTEX_FONT* pVerticesData, UINT vertexCount)
	{
		HRESULT hr = pVertexBuffer_->InitializeDynamic(pDevice, pVerticesData, vertexCount);
		return hr;
	}

	// load up indices data
	HRESULT InitializeIndexBuffer(ID3D11Device* pDevice, UINT* pIndicesData, UINT indexCount)
	{
		HRESULT hr = pIndexBuffer_->Initialize(pDevice, pIndicesData, indexCount);
		return hr;
	}

	//
	// GETTERS
	//
	const std::string & GetText() const 
	{
		return text_;
	}

	const DirectX::XMFLOAT2 & GetPosition() const
	{
		return pos_;
	}

	const size_t GetMaxTextLength() const
	{
		return maxLength_;
	}

	const DirectX::XMFLOAT4 & GetColor() const
	{
		return color_;
	}

	VertexBuffer<VERTEX_FONT>* const GetVertexBuffer() const
	{
		return pVertexBuffer_;
	}

	const IndexBuffer* const GetIndexBuffer() const
	{
		return pIndexBuffer_;
	}

	//
	// SETTERS
	//
	void SetText(const std::string & newText)
	{
		text_ = newText;
	}

	void SetColor(const DirectX::XMFLOAT4 & newColor)
	{
		color_ = { newColor.x, newColor.y, newColor.z, 1.0f };
	}

private:
	VertexBuffer<VERTEX_FONT>* pVertexBuffer_ = nullptr;
	IndexBuffer*               pIndexBuffer_ = nullptr;
	std::string text_{ "" };                       // a text content
	size_t  maxLength_ = 0;                        // maximal length of this sentence
	DirectX::XMFLOAT2 pos_{ 0.0f, 0.0f };                 // the left upper position of the whole sentence on the screen
	DirectX::XMFLOAT4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };   // colour of the sentence
};



//////////////////////////////////
// Class name: TextClass
//////////////////////////////////
class TextClass
{
public:
	TextClass();
	~TextClass();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		int screenWidth, int screenHeight,
		int stringSize,                        // maximal size of the string
		FontClass* pFont,                      // font for the text
		const char* textContent,               // the content of the text
		int posX, int posY,                    // upper left position of the text in the window
		float red, float green, float blue);   // colour of the text

	bool Render(ID3D11DeviceContext* deviceContext,
		const DirectX::XMMATRIX & worldMatrix,
		const DirectX::XMMATRIX & baseViewMatrix,
		const DirectX::XMMATRIX & orthoMatrix);

	bool Update(ID3D11DeviceContext* pDeviceContext, const std::string & newText, const DirectX::XMFLOAT2 & newPosition, const DirectX::XMFLOAT4 & newColor);

	// memory allocation
	void* operator new(size_t i);
	void  operator delete(void* ptr);

private:  // restrict a copying of this class instance
	TextClass(const TextClass & obj);
	TextClass & operator=(const TextClass & obj);

private:
	bool BuildSentence(ID3D11Device* pDevice, int stringSize,
		const char* textContent,
		int posX, int posY,
		float red, float green, float blue);  // first of all we create an empty sentence (with empty vertices data) and after we update this sentence with text data

	
	bool UpdateSentenceVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::string & nextText, int posX, int posY);
	
	bool RenderSentence(ID3D11DeviceContext* pDeviceContext, 
						const DirectX::XMMATRIX & worldMatrix,
						const DirectX::XMMATRIX & baseViewMatrix,
						const DirectX::XMMATRIX & orthoMatrix);
private: // helpers
	// checks if we must update the current sentence because of new differ params
	bool CheckSentence(SentenceType* pPrevSentence, const std::string & newText, const DirectX::XMFLOAT2 & newPosition); 
	bool CheckColor(const DirectX::XMFLOAT4 & prevColor, const DirectX::XMFLOAT4 & newColor);  // checks if both input colours are the same
private:
	FontClass* pFont_ = nullptr;
	FontShaderClass* pFontShader_ = nullptr;;

	int screenWidth_ = 0;
	int screenHeight_ = 0;
	SentenceType* pSentence_ = nullptr;
};