////////////////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////////////////////////////
#include "textclass.h"
#include <iostream>



TextClass::TextClass() 
{
}

TextClass::~TextClass() 
{
	Log::Debug(LOG_MACRO); 

	_DELETE(pSentence_);
	pFont_ = nullptr;
}






////////////////////////////////////////////////////////////////////////////////////////////
// 
//                              PUBLIC METHODS 
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TextClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	const int screenWidth, 
	const int screenHeight,
	const int stringSize,                        // maximal size of the string
	FontClass* pFont,                      // font for the text
	FontShaderClass* pFontShader,          // font shader for rendering text onto the screen
	const std::string text,                // the content of the text
	const POINT & position,                // upper left position of the text in the window
	const DirectX::XMFLOAT3 & color)       // colour of the text
{
	// check input params
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);
	assert(pFont != nullptr);
	assert(pFontShader != nullptr);
	assert(text.empty() != true);

	try
	{
		bool result = false;
		POINT drawAt{ 0, 0 };          // default upper left position of the sentence

									   // store the screen width and height
		screenWidth_ = screenWidth;
		screenHeight_ = screenHeight;

		// initialize the font object with external one
		pFont_ = pFont;

		// initialize the font shader pointer with a pointer to a FontShaderClass instance
		pFontShader_ = pFontShader;

		// calculate the position of the sentence on the screen
		drawAt.x = static_cast<int>((screenWidth_ / -2) + position.x);
		drawAt.y = static_cast<int>((screenHeight_ / 2) - position.y);

		// build a sentence
		this->BuildSentence(pDevice,
			stringSize,
			text,
			drawAt,
			color);
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't initialize text class obj with the text: " + text };

		Log::Error(e);
		Log::Error(LOG_MACRO, errorMsg.c_str());
		return false;
	}

	return true;

} // end Initialize

///////////////////////////////////////////////////////////

bool TextClass::Render(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & WVO)
{
	// this function renders the sentences on the screen

	// render the sentence
	bool result = this->RenderSentence(pDeviceContext, 
		WVO,
		pSentence_->GetColor(),
		pFont_->GetTextureResourceViewAddress());
	COM_ERROR_IF_FALSE(result, "can't render the sentence");

	return true;
}

///////////////////////////////////////////////////////////

bool TextClass::Update(ID3D11DeviceContext* pDeviceContext, 
	const std::string & newText,
	const POINT & newPosition,              // position to draw at
	const DirectX::XMFLOAT3 & newColor)     // new text colour
{
	// Update() changes the contents of the vertex buffer for the input sentence.
	// It uses the Map and Unmap functions along with memcpy to update the contents 
	// of the vertex buffer

	try
	{
		POINT drawAt{ 0, 0 };          // default upper left position of the sentence
		bool result = false;

		// check if the text buffer overflow
		if (pSentence_->GetMaxTextLength() < newText.length())
		{
			COM_ERROR_IF_FALSE(false, "the text buffer is overflow");
		}

		// calculate the position of the sentence on the screen
		drawAt.x = -(screenWidth_ >> 1)  + newPosition.x;  // (width >> 1) means division by 2
		drawAt.y =  (screenHeight_ >> 1) - newPosition.y;

		// update the vertex buffer
		UpdateSentenceVertexBuffer(pDeviceContext, newText, drawAt);

		pSentence_->SetText(newText);     // update the sentence text content 		
		pSentence_->SetColor(newColor);   // update the sentence text colour
	
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't update the sentence");
		return false;
	}

	return true;
} // Update()





////////////////////////////////////////////////////////////////////////////////////////////
// 
//                               PRIVATE METHODS 
//
////////////////////////////////////////////////////////////////////////////////////////////




void TextClass::BuildSentence(ID3D11Device* pDevice,
	const UINT stringSize,            // maximal size of the string
	const std::string & text,         // the content of the text
	const POINT & position,           // upper left position of the text in the window
	const DirectX::XMFLOAT3 & color)  // the colour of the sentence  
{
	// The BuildSentence() creates a SentenceType with an empty vertex buffer which will
	// be used to store and render sentences. The maxLenght input parameters determines
	// how large the vertex buffer will be. All sentences have a vertex and index buffer
	// associated with them which is initialize first in this function


	// check input params
	//COM_ERROR_IF_FALSE(stringSize, "input string size == 0");
	//COM_ERROR_IF_FALSE(text.empty() != true, "input text is empty");

	//////////////////////////////////////

	HRESULT hr = S_OK;
	bool result = false;

	const UINT verticesCountInSymbol = 4;
	const UINT indicesCountInSymbol = 6;

	const UINT verticesCountInSentence = stringSize * verticesCountInSymbol;
	const UINT indicesCountInSentence = stringSize * indicesCountInSymbol;

	// arrays for vertices and indices data
	std::vector<VERTEX_FONT> verticesArr(verticesCountInSentence);
	std::vector<UINT> indicesArr(indicesCountInSentence);

	// ------------------------ INITIALIZE THE SENTENCE --------------------------//

	// try to create a sentence object and initialize it with some initial data
	try
	{
		pSentence_ = new SentenceType(stringSize, 
			text.c_str(), 
			position.x, 
			position.y,
			color.x,      // red
			color.y,      // green
			color.z);     // blue


		// build the vertices and indices data for this sentence
		pFont_->BuildVertexArray(verticesArr,
			indicesArr,
			text.c_str(), 
			position);

		// initialize the vertex buffer with vertices
		vertexBuffer_.Initialize(pDevice, verticesArr, true);

		// initialize the index buffer
		indexBuffer_.Initialize(pDevice, indicesArr);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the sentence object");
	}
	catch (COMException & e)
	{
		Log::Error(e);
		COM_ERROR_IF_FALSE(false, "can't build a sentence");
	}

	return;
} // end BuildEmptySentence

///////////////////////////////////////////////////////////

void TextClass::UpdateSentenceVertexBuffer(ID3D11DeviceContext* pDeviceContext, 
	const std::string & newText,
	const POINT & position)
{
	try
	{
		// updates the sentence's text content or its position on the screen
		std::vector<VERTEX_FONT> verticesArr(vertexBuffer_.GetVertexCount());

		// rebuild the vertex array
		//pFont_->BuildVertexArray(verticesArr,
		//	newText.c_str(),
		//	position);

		// update the sentence vertex buffer with new data
		//vertexBuffer_.UpdateDynamic(pDeviceContext, verticesArr);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "failed to update the text vertex buffer with new data");
	}

	return;
}

///////////////////////////////////////////////////////////

bool TextClass::RenderSentence(ID3D11DeviceContext* pDeviceContext, 
	const DirectX::XMMATRIX & WVO,          // world * basic_view * ortho
	const DirectX::XMFLOAT3 & textColor,
	ID3D11ShaderResourceView* const* ppTexture)
{
	// This function puts the sentence vertex and index buffer on the input assembler and
	// then calls the FontShaderClass object to draw the sentence that was given as input
	// to this function.

	UINT offset = 0;

	// set the vertices and indices buffers as active
	pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), vertexBuffer_.GetAddressOfStride(), &offset);
	pDeviceContext->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	try
	{
		// render the sentence using the FontShaderClass and HLSL shaders
		pFontShader_->Render(pDeviceContext,
			indexBuffer_.GetIndexCount(),
			WVO,
			textColor,
			ppTexture);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't render the sentence");
	}

	return true;
}

///////////////////////////////////////////////////////////