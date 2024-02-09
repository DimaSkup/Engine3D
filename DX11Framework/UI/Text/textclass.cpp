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

	//_DELETE(pSentence_);
	pFont_ = nullptr;
}






////////////////////////////////////////////////////////////////////////////////////////////
// 
//                              PUBLIC METHODS 
//
////////////////////////////////////////////////////////////////////////////////////////////


bool TextClass::Initialize(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	const int stringSize,                  // maximal size of the string
	FontClass* pFont,                      // font for the text
	FontShaderClass* pFontShader,          // font shader for rendering text onto the screen
	const std::string & text,              // the content of the text
	const POINT & drawAt)                  // upper left position of the text in the window
{
	// check input params
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);
	assert(pFont != nullptr);
	assert(pFontShader != nullptr);
	assert(text.empty() != true);

	try
	{
		// initialize the font object with external one
		pFont_ = pFont;

		// initialize the font shader pointer with a pointer to a FontShaderClass instance
		pFontShader_ = pFontShader;

		// build a sentence
		this->BuildSentence(pDevice,
			stringSize,
			text,
			drawAt);
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

void TextClass::Render(ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX & WVO,
	const DirectX::XMFLOAT3 & color)
{
	// this function renders the sentence onto the screen

	try
	{
		// render the sentence
		this->RenderSentence(pDeviceContext,
			WVO,
			color, //pSentence_->GetColor(),
			pFont_->GetTextureResourceViewAddress());
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't render the sentence");
	}
	
	return;
}

///////////////////////////////////////////////////////////

bool TextClass::Update(ID3D11DeviceContext* pDeviceContext, 
	_Inout_ std::vector<VERTEX_FONT> & verticesArr,  // a temporal buffer for vertices data for updating
	const std::string & newText,
	const POINT & newPosition)              // position to draw at
{
	// Update() changes the contents of the vertex buffer for the input sentence.
	// It uses the Map and Unmap functions along with memcpy to update the contents 
	// of the vertex buffer

	try
	{
		// update the vertex buffer
		UpdateSentenceVertexBuffer(pDeviceContext,
			verticesArr,  // a temporal buffer for vertices data for updating
			newText,
			newPosition);
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
	const UINT maxStrSize,            // maximal size of the string
	const std::string & text,         // the content of the text
	const POINT & position)           // upper left position of the text in the window
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
	const UINT verticesCountInSentence = maxStrSize * verticesCountInSymbol;

	// arrays for vertices and indices data
	std::vector<VERTEX_FONT> verticesArr(verticesCountInSentence);
	std::vector<UINT> indicesArr;

	// ------------------------ INITIALIZE THE SENTENCE --------------------------//

	// try to create a sentence object and initialize it with some initial data
	try
	{
		// build the vertices and indices data for this sentence
		pFont_->BuildVertexArray(verticesArr,
			text.c_str(), 
			position);

		pFont_->BuildIndexArray(indicesArr, maxStrSize);

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
	_Inout_ std::vector<VERTEX_FONT> & verticesArr,  // a temporal buffer for vertices data for updating
	const std::string & newText,
	const POINT & position)
{
	//
	// updates the sentence's text content or its position on the screen
	//

	try
	{
		// rebuild the vertex array
		pFont_->BuildVertexArray(verticesArr,
			newText,
			position);

		// update the sentence vertex buffer with new data
		vertexBuffer_.UpdateDynamic(pDeviceContext, verticesArr);

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