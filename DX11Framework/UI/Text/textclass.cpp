////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////
#include "textclass.h"
#include <iostream>

TextClass::TextClass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) 
{
	try
	{
		//pFontShader_ = new FontShaderClass();   // create the font shader object
		pVertexBuffer_ = new VertexBuffer<VERTEX_FONT>(pDevice, pDeviceContext);
		pIndexBuffer_ = new IndexBuffer(pDevice, pDeviceContext);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the text class elements");
	}
}

TextClass::~TextClass() 
{
	Log::Debug(THIS_FUNC_EMPTY); 

	_DELETE(pVertexBuffer_);
	_DELETE(pIndexBuffer_);
	_DELETE(pSentence_);
	//_DELETE(pFontShader_);
	pFont_ = nullptr;
}






// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //
bool TextClass::Initialize(ID3D11Device* pDevice, 
	                       ID3D11DeviceContext* pDeviceContext,
	                       int screenWidth, 
	                       int screenHeight,
						   int stringSize,
	                       FontClass* pFont,
						   FontShaderClass* pFontShader,
						   const char* textContent,
						   int posX, int posY,
						   float red, float green, float blue)
{
	assert(pDevice != nullptr);
	assert(pDeviceContext != nullptr);
	assert(pFont != nullptr);
	assert(pFontShader != nullptr);
	assert((textContent != nullptr) && (textContent[0] != '\0'));

	bool result = false;
	int drawX = 0, drawY = 0;          // upper left position of the sentence

	// store the screen width and height
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// initialize the font object with external one
	pFont_ = pFont;

	// initialize the font shader pointer with a pointer to a FontShaderClass instance
	pFontShader_ = pFontShader;

	// calculate the position of the sentence on the screen
	drawX = static_cast<int>((screenWidth_ / -2) + posX);
	drawY = static_cast<int>((screenHeight_ / 2) - posY);

	// build a sentence
	result = this->BuildSentence(pDevice, stringSize, textContent, drawX, drawY, red, green, blue);
	COM_ERROR_IF_FALSE(result, "can't build a new sentence");

	return true;
} // Initialize()





// The Render() renders the sentences on the screen
bool TextClass::Render(ID3D11DeviceContext* pDeviceContext,
	                   const DirectX::XMMATRIX & worldMatrix,
					   const DirectX::XMMATRIX & baseViewMatrix,
	                   const DirectX::XMMATRIX & orthoMatrix)
{
	// render the sentence
	 bool result = this->RenderSentence(pDeviceContext, worldMatrix, baseViewMatrix, orthoMatrix);
	COM_ERROR_IF_FALSE(result, "can't render the sentence");

	return true;
}


  // Update() changes the contents of the vertex buffer for the input sentence.
  // It uses the Map and Unmap functions along with memcpy to update the contents 
  // of the vertex buffer
bool TextClass::Update(ID3D11DeviceContext* pDeviceContext, 
	const std::string & newText,
	const DirectX::XMFLOAT2 & newPosition,  // position to draw at
	const DirectX::XMFLOAT4 & newColor)     // text colour
{
	// if we try to update the sentence with the same text and position (etc.) we won't update it
	if (CheckSentence(pSentence_, newText, newPosition))
	{
		// there can be a different colour (in this case we shouldn't rebuild all the sentence)
		if (CheckColor(pSentence_->GetColor(), newColor))
		{
			pSentence_->SetColor(newColor); // update the sentence text colour
		}

		return true;
	}
	else // else we want to update the sentence (its position, text content, etc.)
	{
		int drawX = 0, drawY = 0;          // upper left position of the sentence
		bool result = false;
		
		// check if the text buffer overflow
		if (pSentence_->GetMaxTextLength() < newText.length())
		{
			COM_ERROR_IF_FALSE(false, "the text buffer is overflow");
		}

		// calculate the position of the sentence on the screen
		drawX = static_cast<int>((screenWidth_ / -2) + newPosition.x);
		drawY = static_cast<int>((screenHeight_ / 2) - newPosition.y);

		// update the vertex buffer
		result = this->UpdateSentenceVertexBuffer(pDeviceContext, newText, drawX, drawY);
		COM_ERROR_IF_FALSE(result, "can't update the sentence vertex buffer");

		
		pSentence_->SetText(newText);     // update the sentence text content 		
		pSentence_->SetColor(newColor);   // update the sentence text colour
	}

	return true;
} // Update()





// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

// The BuildSentence() creates a SentenceType with an empty vertex buffer which will
// be used to store and render sentences. The maxLenght input parameters determines
// how large the vertex buffer will be. All sentences have a vertex and index buffer
// associated with them which is initialize first in this function

bool TextClass::BuildSentence(ID3D11Device* pDevice,
	int stringSize,   // maximal size of the string
	const char* textContent,                    // the content of the text
	int posX, int posY,                         // upper left position of the text in the window
	float red, float green, float blue)         // the colour of the sentence
{
	HRESULT hr = S_OK;
	bool result = false;

	constexpr UINT verticesCountInSymbol = 6;
	UINT verticesCountInSentence = static_cast<UINT>(stringSize) * verticesCountInSymbol;
	UINT indicesCountInSentence = verticesCountInSentence;

	// arrays for vertices and indices data
	std::unique_ptr<VERTEX_FONT[]> pVertices = std::make_unique<VERTEX_FONT[]>(verticesCountInSentence);
	std::unique_ptr<UINT[]>        pIndices  = std::make_unique<UINT[]>(indicesCountInSentence);



	// ------------------------ INITIALIZE THE SENTENCE --------------------------//

	// try to create a sentence object and initialize it with some initial data
	try
	{
		pSentence_ = new SentenceType(stringSize, textContent, posX, posY, red, green, blue);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the sentence object");
	}

	
	// try to initialize the vertex and index buffer
	try
	{
		// build the vertex array
		pFont_->BuildVertexArray((void*)pVertices.get(), textContent, static_cast<float>(posX), static_cast<float>(posY));

		// initialize the verte buffer
		hr = pVertexBuffer_->Initialize(pDevice, pVertices.get(), verticesCountInSentence);
		COM_ERROR_IF_FAILED(hr, "can't initialize the vertex buffer");

		// make indices data
		for (UINT i = 0; i < indicesCountInSentence; i++)
		{
			pIndices[i] = i;
		}

		// initialize the index buffer
		hr = pIndexBuffer_->Initialize(pIndices.get(), indicesCountInSentence);
		COM_ERROR_IF_FAILED(hr, "can't initialize the index buffer");
	}
	catch (const COMException & e)
	{
		Log::Error(THIS_FUNC, e);
		COM_ERROR_IF_FALSE(false, "can't build a sentence");
	}

	return true;
} // BuildEmptySentence()





// updates the sentence's text content or its position on the screen
bool TextClass::UpdateSentenceVertexBuffer(ID3D11DeviceContext* pDeviceContext, 
	const std::string & newText,
	int posX, int posY)
{
	bool result = false;
	HRESULT hr = S_OK;
	std::unique_ptr<VERTEX_FONT[]> pVertices = std::make_unique<VERTEX_FONT[]>(pVertexBuffer_->GetBufferSize());

	// rebuild the vertex array
	pFont_->BuildVertexArray((void*)pVertices.get(), newText.c_str(), static_cast<float>(posX), static_cast<float>(posY));

	// update the sentence vertex buffer with new data
	result = pVertexBuffer_->UpdateDynamic(pDeviceContext, pVertices.get());
	COM_ERROR_IF_FALSE(result, "failed to update the text vertex buffer with new data");

	return true;
}


// This function puts the sentence vertex and index buffer on the input assembler and
// then calls the FontShaderClass object to draw the sentence that was given as input
// to this function.
bool TextClass::RenderSentence(ID3D11DeviceContext* pDeviceContext,  
	const DirectX::XMMATRIX & worldMatrix,
	const DirectX::XMMATRIX & baseViewMatrix,
	const DirectX::XMMATRIX & orthoMatrix)
{
	bool result = false;
	UINT offset = 0;

	// set the vertices and indices buffers as active
	pDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffer_->GetAddressOf(), pVertexBuffer_->GetAddressOfStride(), &offset);

	// set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeviceContext->IASetIndexBuffer(pIndexBuffer_->Get(), DXGI_FORMAT_R32_UINT, 0);


	// render the sentence using the FontShaderClass and HLSL shaders
	result = pFontShader_->Render(pDeviceContext,
		static_cast<int>(pIndexBuffer_->GetBufferSize()),              
		worldMatrix, 
		baseViewMatrix, 
		orthoMatrix,
		pFont_->GetTexture(), 
		pSentence_->GetColor());
	COM_ERROR_IF_FALSE(result, "can't render the sentence");

	return true;
}



//
// HELPERS
//

// checks if we must update the current sentence because of new different params
bool TextClass::CheckSentence(SentenceType* pPrevSentence, 
	const std::string & newText, 
	const DirectX::XMFLOAT2 & newPosition)
{
	bool isTextSame = (pSentence_->GetText() == newText);
	bool isPositionSame = ((pSentence_->GetPosition().x == newPosition.x) && (pSentence_->GetPosition().y == newPosition.y));

	return isTextSame && isPositionSame;
}


// checks if both input colours are the same
bool TextClass::CheckColor(const DirectX::XMFLOAT4 & prevColor, const DirectX::XMFLOAT4 & newColor)
{
	return (prevColor.x == newColor.x) && (prevColor.y == newColor.y) && (prevColor.z == newColor.z);
}