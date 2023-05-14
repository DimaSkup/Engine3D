////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////
#include "textclass.h"
#include <iostream>



TextClass::TextClass() 
{
}

TextClass::TextClass(const TextClass& copy) 
{
}

TextClass::~TextClass() 
{
	Log::Debug(THIS_FUNC_EMPTY); 
}






// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //
bool TextClass::Initialize(ID3D11Device* pDevice, 
	                       ID3D11DeviceContext* pDeviceContext,
	                       HWND hwnd,
	                       int screenWidth, 
	                       int screenHeight, 
	                       DirectX::XMMATRIX baseViewMatrix)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// store the screen width and height
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// store the base view matrix
	baseViewMatrix_ = baseViewMatrix;

	pDevice_ = pDevice;
	pDeviceContext_ = pDeviceContext;


	// ------------------------------- FONT CLASS --------------------------------------- //

	// create the font object
	pFont_ = new FontClass();
	if (!pFont_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the font object");
		return false;
	}

	// initialize the font object
	result = pFont_->Initialize(pDevice, "data/ui/fontdata.txt", L"data/textures/font.dds");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font object");
		return false;
	}

	// ---------------------------- FONT SHADER CLASS ----------------------------------- //
	// create the font shader object
	pFontShader_ = new FontShaderClass();
	if (!pFontShader_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the font shader object");
		return false;
	}

	// initialize the font shader object
	result = pFontShader_->Initialize(pDevice, pDeviceContext, hwnd);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font shader object");
		return false;
	}


	// ------------------------ READ IN TEXT DATA FROM FILE ----------------------------- //
	//AddSentence("", 100, 100, 1.0f, 1.0f, 1.0f);
	/*
	result = ReadInTextFromFile(textDataFilename);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't read in text data from the file");
		return false;
	}
	*/
	

	return true;
} // Initialize()


// The Shutdown() will release the sentences, font class object and font shader object
void TextClass::Shutdown(void)
{
	// if there are some sentences we clean up memory from it
	if (!sentences_.empty())  
	{
		std::map<std::string, SentenceType*>::iterator i;

		for (i = sentences_.begin(); i != sentences_.end(); i++) 
		{
			// release the vertex buffer, index buffer, ect. of the sentence
			_DELETE(i->second);                   // release the sentence
		}

		sentences_.clear(); // release the map of the sentences
	}


	_SHUTDOWN(pFont_);       // release the font object

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}


// The Render() renders the sentences on the screen
bool TextClass::Render(ID3D11DeviceContext* deviceContext,
	                   DirectX::XMMATRIX worldMatrix,
	                   DirectX::XMMATRIX orthoMatrix)
{

	bool result = false;
	std::map<std::string, SentenceType*>::iterator i;
	
	// render sentences
	for (i = sentences_.begin(); i != sentences_.end(); i++)
	{
		result = RenderSentence(deviceContext, i->second, worldMatrix, orthoMatrix);
		if (!result)
		{
			Log::Get()->Error("%s()::%d %s %d", __FUNCTION__, __LINE__, "can't render the sentence #", i);
			return false;
		}
	}

	return true;
} // Render();


// adds by particular key a new sentence for output onto the screen;
// or updates a sentence by key with new text data;
bool TextClass::SetSentenceByKey(std::string key, std::string text,
	                             int posX, int posY,
	                             float red, float green, float blue)
{
	bool result = false;
	
	// if we want to create a new text sentence
	if (!sentences_[key]) // if we don't have any data by this key yet
	{
		SentenceType* pSentence = nullptr;

		result = CreateSentenceByKey(&pSentence, key, text, posX, posY, red, green, blue);
		if (!result)
		{
			_DELETE(pSentence);
			std::string errorMsg = "can't create a new sentence by key: " + key;
			Log::Error(THIS_FUNC, errorMsg.c_str());
			return false;
		}

		sentences_[key] = pSentence;  // set a pair [key => pointer]
	}
	else // we already have some sentence by this key (we want to update it)
	{
		result = UpdateSentence(sentences_[key], text,
				                posX, posY,
				                red, green, blue);
		if (!result)
		{
			std::string errorMsg = "can't update the sentence by key: " + key;
			Log::Get()->Error(THIS_FUNC, errorMsg.c_str());
			return false;
		}
	}

	return true;
} // SetSentenceByKey()


// creates a new sentence by particular key and initializes it with text data
bool TextClass::CreateSentenceByKey(SentenceType** ppSentence, 
									std::string key, 
									std::string text, 
									int posX, int posY, 
									float red, float green, float blue)
{
	bool result = false;

	// allocate the memory for a new sentence
	*ppSentence = new(std::nothrow) SentenceType;
	if (!ppSentence)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for a SentenceType object");
		return false;
	}


	// make an empty sentence
	result = BuildEmptySentence(ppSentence, maxStringSize_);
	if (!result)
	{
		
		Log::Get()->Error(THIS_FUNC, "can't initialize the sentence");
		return false;
	}

	// update the sentence after its initialization
	result = UpdateSentence(*ppSentence, text,
		posX, posY,
		red, green, blue);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the sentence");
		return false;
	}

	return true;
} // CreateSentenceByKey()



// memory allocation
void* TextClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);

	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the object");
		return nullptr;
	}

	return ptr;
}

void TextClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}


// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

// The BuildSentence() creates a SentenceType with an empty vertex buffer which will
// be used to store and render sentences. The maxLenght input parameters determines
// how large the vertex buffer will be. All sentences have a vertex and index buffer
// associated with them which is initialize first in this function
bool TextClass::BuildEmptySentence(SentenceType** ppSentence, size_t maxLength)
{
	HRESULT hr = S_OK;
	bool result = false;

	UINT verticesCountInSymbol = 6;
	UINT verticesCountInSentence = static_cast<UINT>(maxLength) * verticesCountInSymbol;
	UINT indicesCountInSentence = verticesCountInSentence;

	// arrays for vertices and indices data
	std::unique_ptr<VERTEX_FONT[]> pVertices = std::make_unique<VERTEX_FONT[]>(verticesCountInSentence);
	std::unique_ptr<UINT[]>        pIndices  = std::make_unique<UINT[]>(indicesCountInSentence);

	// ------------------------ INITIALIZE THE EMPTY SENTENCE --------------------------// 
	(*ppSentence) = new(std::nothrow) SentenceType;
	if (!*ppSentence)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the SentenceType object");
		return false;
	}

	(*ppSentence)->maxLength = maxLength;

	// load vertex data
	hr = (*ppSentence)->vertexBuf.InitializeDynamic(pDevice_, pVertices.get(), verticesCountInSentence);
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't initialize the dynamic vertex buffer");
		_DELETE(*ppSentence);
		return false;
	}


	// load index data
	for (UINT i = 0; i < indicesCountInSentence; i++)
	{
		pIndices[i] = i;
	}

	hr = (*ppSentence)->indexBuf.Initialize(pDevice_, pIndices.get(), indicesCountInSentence);
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't initialize the index buffer for an empty sentence");
		_DELETE(*ppSentence);
		return false;
	}

	return true;
} // BuildEmptySentence()



// UpdateSentence() changes the contents of the vertex buffer for the input sentence.
// It uses the Map and Unmap functions along with memcpy to update the contents 
// of the vertex buffer
bool TextClass::UpdateSentence(SentenceType* pSentence, std::string text,
	                           int posX, int posY,                   // position to draw at
	                           float red, float green, float blue)   // text colour
{
	// if we try to update the sentence with the same text we won't update it
	if (pSentence->text == text)
	{
		return true;
	}
	else // else we want to update with some another text
	{
		HRESULT hr = S_OK;
		bool result = false;
		size_t textLength = text.length();
		int drawX = 0, drawY = 0;          // upper left position of the sentence

		// check if the text buffer overflow
		if (pSentence->maxLength < textLength)
		{
			Log::Get()->Error(THIS_FUNC, "the text buffer is overflow");
			return false;
		}

		// set up the text colour
		pSentence->red = red;
		pSentence->green = green;
		pSentence->blue = blue;

		// -------------------- UPDATE THE VERTEX BUFFER -------------------------------- // 

		// calculate the position of the sentence on the screen
		drawX = (screenWidth_ / -2) + posX;
		drawY = screenHeight_ / 2 - posY;

		result = this->UpdateSentenceVertexBuffer(pSentence, text, drawX, drawY);
		if (!result)
		{
			Log::Get()->Error(THIS_FUNC, "can't update the sentence vertex buffer");
			return false;
		}

		pSentence->text = text;
	}

	return true;
} // UpdateSentence()


// updates the sentence's text content or its position on the screen
bool TextClass::UpdateSentenceVertexBuffer(SentenceType* pSentence, 
	                                       std::string text,
	                                       int posX, int posY)
{
	bool result = false;
	HRESULT hr = S_OK;
	std::unique_ptr<VERTEX_FONT[]> pVertices = std::make_unique<VERTEX_FONT[]>(pSentence->vertexBuf.GetBufferSize());

	// rebuild the vertex array
	pFont_->BuildVertexArray((void*)pVertices.get(), text.c_str(), static_cast<float>(posX), static_cast<float>(posY));
	

	// update the sentence vertex buffer with new data
	result = pSentence->vertexBuf.UpdateDynamic(pDeviceContext_, pVertices.get());
	if (!result)
	{
		Log::Error(THIS_FUNC, "failed to update the text vertex buffer with new data");
		return false;
	}

	return true;
}


// This function puts the sentence vertex and index buffer on the input assembler and
// then calls the FontShaderClass object to draw the sentence that was given as input
// to this function.
bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext,  
								SentenceType* pSentence,
								DirectX::XMMATRIX worldMatrix, 
								DirectX::XMMATRIX orthoMatrix)
{
	bool result = false;
	UINT stride = sizeof(VERTEX_FONT);
	UINT offset = 0;

	// set the vertices and indices buffers as active
	deviceContext->IASetVertexBuffers(0, 1, pSentence->vertexBuf.GetAddressOf(), &stride, &offset);

	// set the primitive topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetIndexBuffer(pSentence->indexBuf.Get(), DXGI_FORMAT_R32_UINT, 0);

	

	// set the text colour
	DirectX::XMFLOAT4 pixelColor(pSentence->red, pSentence->green, pSentence->blue, 1.0f);

	// render the sentence using the FontShaderClass and HLSL shaders
	result = pFontShader_->Render(deviceContext, static_cast<int>(pSentence->indexBuf.GetBufferSize()), 
		                           worldMatrix, baseViewMatrix_, orthoMatrix,
		                           pFont_->GetTexture(), pixelColor);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the sentence");
		return false;
	}

	return true;
}


/*

// The ReadInTextFromFile() reads in text data from a file
bool TextClass::ReadInTextFromFile(const char* textDataFilename)
{
	Log::Get()->Debug(THIS_FUNC, textDataFilename);

	char* sentencesFromFile[5] = { "first", "second", "third", "fourth", "1234567890123456" };
	char textLineFromFile[17];

	
	// initialize the text with data from the file
	for (size_t i = 0; i < 5; i++)
	{
		memcpy(textLineFromFile, sentencesFromFile[i], m_maxStringSize);
		textLineFromFile[m_maxStringSize] = '\0';

		int posX = 10;
		int posY = static_cast<int>(i * 25);

		float red = static_cast<float>(i * 0.1f);
		float green = static_cast<float>(i * 0.2f);
		float blue = static_cast<float>(i * 0.3f);

		//this->AddSentence(textLineFromFile, posX, posY, red, green, blue);
	}

	return true;
} // ReadInTextFromFile()


*/
/*


// A UNIVERSAL FUNCTION
// updates the sentence by its index in the sentences vector;
// if you want you can also update this sentence position and its colour
bool TextClass::UpdateSentenceByIndex(size_t index, char* newText,
	                                  int posX, int posY,
	                                  float red, float green, float blue)
{
	bool result = false;
	SentenceType* sentence = m_sentencesVector[index]; // the current sentence

	UpdateSentenceContent(sentence, newText);
	UpdateSentencePosition(sentence, posX, posY);
	UpdateSentenceColor(sentence, red, green, blue);

	return true;
} // UpdateSentenceByIndex()


bool TextClass::UpdateSentenceContent(SentenceType* pSentence, char* text)
{
	bool result = false;
	size_t textLength = 0;                   // a length of a new text line

	if (pSentence != nullptr)  // if the sentence isn't initialized
	{
		if (text != nullptr)  // if the text data is initialized
		{
			// ---------- rebuild the sentence's vertices and indices arrays ----------- //
			textLength = strlen(text);

			// check if the text buffer overflow
			if (pSentence->maxLength < textLength)
			{
				Log::Get()->Error(THIS_FUNC, "the text buffer is overflow");
				return false;
			}
			
			// update the sentence's vertex buffer with new text data (use the previous sentence's position)
			result = UpdateSentenceVertexBuffer(pSentence, text, pSentence->posX, pSentence->posY);
			if (!result)
			{
				Log::Get()->Error(THIS_FUNC, "can't update the sentence text content");
				return false;
			}
		} // if (text != nullptr)
	} // if (index >= 0) 

	return true;
}  // UpdateSentenceContentByIndex()

bool TextClass::UpdateSentencePosition(SentenceType* pSentence, int posX, int posY)
{
	bool result = false;

	if (pSentence != nullptr)  // if the sentence isn't initialized
	{
		// if we want to update the sentence position (by any coordinate)
		if ((posX != NULL) || (posY != NULL)) // != NULL because we can have a zero value as a coordinate
		{
			posX = (posX) ? posX : pSentence->posX;  // use the new X position or set the previous one
			posY = (posY) ? posY : pSentence->posY;  // use the new Y position or set the previous one

			// update the sentence's vertex buffer with new position (use the previous sentence's text content)
			result = UpdateSentenceVertexBuffer(pSentence, pSentence->text, posX, posY);
			if (!result)
			{
				Log::Get()->Error(THIS_FUNC, "can't update the sentence position");
				return false;
			}
		}
	}
	else
	{
		Log::Get()->Error(THIS_FUNC, "there is an empty sentence (it == nullptr)");
	}
	
	return true;
}

bool TextClass::UpdateSentenceColor(SentenceType* pSentence, float red, float green, float blue)
{
	if (pSentence != nullptr)  // if the sentence isn't initialized
	{
		// if we want to update the sentence colour (you need have here the red, green and blue values at the same time)
		if ((red != NULL) && (green != NULL) && (blue != NULL)) // != NULL because we can have a 0.0f value as a colour
		{
			// set up the text colour of the current sentence
			pSentence->red = red;
			pSentence->green = green;
			pSentence->blue = blue;
		}
		else
		{
			Log::Get()->Error(THIS_FUNC, "there is an incorrect colour values");
			return false;
		}
	}

	return true;
}



*/