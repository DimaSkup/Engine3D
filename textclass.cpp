////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////
#include "textclass.h"
#include <iostream>

TextClass::TextClass(void)
{
	m_pFont = nullptr;
	m_pFontShader = nullptr;

	m_maxStringSize = 40;
}



TextClass::TextClass(const TextClass& copy) {}
TextClass::~TextClass(void) {}

// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //
bool TextClass::Initialize(ID3D11Device* device, 
	                       ID3D11DeviceContext* deviceContext,
	                       HWND hwnd,
	                       int screenWidth, 
	                       int screenHeight, 
	                       DirectX::XMMATRIX baseViewMatrix)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// store the screen width and height
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the base view matrix
	m_baseViewMatrix = baseViewMatrix;

	m_pDevice = device;
	m_pDeviceContext = deviceContext;


	// ------------------------------- FONT CLASS --------------------------------------- //

	// create the font object
	m_pFont = new FontClass();
	if (!m_pFont)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the font object");
		return false;
	}

	// initialize the font object
	result = m_pFont->Initialize(device, "data/fontdata.txt", L"data/font.dds");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font object");
		return false;
	}

	// ---------------------------- FONT SHADER CLASS ----------------------------------- //
	// create the font shader object
	m_pFontShader = new FontShaderClass();
	if (!m_pFontShader)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the font shader object");
		return false;
	}

	// initialize the font shader object
	result = m_pFontShader->Initialize(device, hwnd);
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
	if (!sentences.empty())  
	{
		std::map<std::string, TextClass::SentenceType*>::iterator i;

		for (i = sentences.begin(); i != sentences.end(); i++) 
		{
			// release the vertex buffer, index buffer, ect. of the sentence
			ReleaseSentence(&(i->second));   
		}

		sentences.clear(); // release the map of the sentences
	}


	_SHUTDOWN(m_pFont);       // release the font object
	_SHUTDOWN(m_pFontShader); // release the font shader object

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
	for (i = sentences.begin(); i != sentences.end(); i++)
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


// adds a new text line for output on the screen;
bool TextClass::SetSentenceByKey(std::string key, std::string text,
	                             int posX, int posY,
	                             float red, float green, float blue)
{
	bool result = false;
	SentenceType* pSentence = nullptr;

	// if we want to create a new text sentence
	if (!sentences[key]) // if we don't have any data by this key yet
	{
		// allocate the memory for a sentence
		pSentence = new(std::nothrow) SentenceType;
		if (!pSentence)
		{
			Log::Get()->Error(THIS_FUNC, "can't allocate the memory for a SentenceType object");
			return false;
		}


		// initialize the sentence
		result = BuildSentence(&pSentence, m_maxStringSize);
		if (!result)
		{
			_DELETE(pSentence);
			Log::Get()->Error(THIS_FUNC, "can't initialize the sentence");
			return false;
		}

		// update the sentence
		result = UpdateSentence(pSentence, text,
			                    posX, posY,
			                    red, green, blue);
		if (!result)
		{
			_DELETE(pSentence);  // free the memory of the sentence because we can't update it
			Log::Get()->Error(THIS_FUNC, "can't update the sentence");
			return false;
		}

		sentences[key] = pSentence;  // set a pair [key => pointer]
	} // if
	else // we have some sentence by this key
	{
		// we have to do some changes about this sentence
		result = UpdateSentence(sentences[key], text,
				                posX, posY,
				                red, green, blue);
		if (!result)
		{
			//_DELETE(pSentence);  // free the memory of the sentence because we can't update it
			Log::Get()->Error(THIS_FUNC, "can't update the sentence");
			return false;
		}
	
	} // else

	return true;
} // SetSentenceByKey()

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
bool TextClass::BuildSentence(SentenceType** ppSentence, size_t maxLength)
{
	HRESULT hr = S_OK;
	VERTEX* vertices = nullptr;
	ULONG* indices = nullptr;
	D3D11_SUBRESOURCE_DATA initData;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;

	// ------------------------ INITIALIZE THE EMPTY SENTENCE --------------------------// 
	(*ppSentence) = new(std::nothrow) SentenceType;
	if (!*ppSentence)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the SentenceType object");
		return false;
	}

	(*ppSentence)->vertexBuffer = nullptr;
	(*ppSentence)->indexBuffer = nullptr;

	(*ppSentence)->maxLength = maxLength;

	(*ppSentence)->vertexCount = maxLength * 6;
	(*ppSentence)->indexCount = (*ppSentence)->vertexCount;

	// -------------------------- VERTEX AND INDEX ARRAYS ----------------------------- //

	// create a vertices array (it's already set to zeros during the creation)
	vertices = new(std::nothrow) VERTEX[(*ppSentence)->vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}

	// create an indices array 
	indices = new(std::nothrow) ULONG[(*ppSentence)->indexCount];
	if (!indices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the indices array");
		_DELETE(vertices);

		return false;
	}

	// initialize the indices array
	for (size_t i = 0; i < (*ppSentence)->indexCount; i++)
	{
		indices[i] = static_cast<ULONG>(i);
	}


	// ----------------------- VERTEX AND INDEX BUFFERS -------------------------------- //

	// set up the vertex buffer description
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(VERTEX) * ((*ppSentence)->vertexCount));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.MiscFlags = 0;
	
	// prepare data for the vertex buffer
	// fill in the vertex array with vertices data of the new sentence
	//m_pFont->BuildVertexArray((void*)vertices, text, static_cast<float>(posX), static_cast<float>(posY));

	initData.pSysMem = vertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	

	// create the vertex buffer
	hr = m_pDevice->CreateBuffer(&vertexBufferDesc, &initData, &(*ppSentence)->vertexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
		_DELETE(vertices);
		_DELETE(indices);

		return false;
	}


	// set up the index buffer description
	indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(ULONG) * ((*ppSentence)->indexCount));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.MiscFlags = 0;

	// prepare data for the index buffer
	initData.pSysMem = indices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// create the index buffer
	hr = m_pDevice->CreateBuffer(&indexBufferDesc, &initData, &(*ppSentence)->indexBuffer);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the index buffer");
		_DELETE(vertices);
		_DELETE(indices);

		return false;
	}

	// release the vertices and indices arrays 
	_DELETE(vertices);
	_DELETE(indices);

	return true;
} // BuildSentence()


// UpdateSentence() changes the contents of the vertex buffer for the input sentence.
// It uses the Map and Unmap functions along with memcpy to update the contents 
// of the vertex buffer
bool TextClass::UpdateSentence(SentenceType* pSentence, std::string text,
	                           int posX, int posY,                   // position to draw at
	                           float red, float green, float blue)   // text colour
{
	HRESULT hr = S_OK;
	bool result = false;
	size_t textLength = text.length();
	int drawX = 0, drawY = 0;          // upper left position of the sentence

	// check if the text buffer overflow
	if (pSentence->maxLength < textLength)
	{
		Log::Get()->Print("text: %s ::: size: %d", text, textLength);
		Log::Get()->Error(THIS_FUNC, "the text buffer is overflow");
		return false;
	}

	// set up the text colour
	pSentence->red = red;
	pSentence->green = green;
	pSentence->blue = blue;

	// -------------------- BUILD THE VERTEX ARRAY ------------------------------------ // 

	// calculate the position of the sentence on the screen
	drawX = (m_screenWidth / -2) + posX;
	drawY = m_screenHeight / 2 - posY;

	result = this->UpdateSentenceVertexBuffer(pSentence, text, drawX, drawY);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the sentence vertex buffer");
	}

	return true;
} // UpdateSentence()


// updates the sentence's text content or its position on the screen
bool TextClass::UpdateSentenceVertexBuffer(SentenceType* pSentence, 
	                                       std::string text,
	                                       int posX, int posY)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	VERTEX* vertices = nullptr;           // a pointer to vertices of the updated sentence
	VERTEX* verticesPtr = nullptr;        // a pointer to the mapped vertex buffer of the sentence object

	// ----------------------- REBUILD THE VERTEX ARRAY ------------------------------ //

	// create a vertices array (it is already filled with zeros during the creation)
	vertices = new(std::nothrow) VERTEX[pSentence->vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}


	// fill in the vertex array with vertices data of the new sentence
	m_pFont->BuildVertexArray((void*)vertices, text.c_str(), static_cast<float>(posX), static_cast<float>(posY));


	// --------------------- FILL IN THE VERTEX BUFFER WITH DATA --------------------- //

	// locking of the vertex buffer to get access to it
	hr = m_pDeviceContext->Map(pSentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		_DELETE(vertices); // clean the vertices
		Log::Get()->Error(THIS_FUNC, "can't Map() the vertex buffer of the sentence object");
		return false;
	}

	// get a pointer to the vertex buffer data
	verticesPtr = static_cast<VERTEX*>(mappedData.pData);

	// write down the data 
	for (size_t i = 0; i < pSentence->vertexCount; i++)
	{
		verticesPtr[i] = vertices[i];
	}

	// unlocking of the vertex buffer
	m_pDeviceContext->Unmap(pSentence->vertexBuffer, 0);

	// releasing of  the vertices array as it is no longer needed
	_DELETE(vertices); 
	verticesPtr = nullptr;


	return true;
}


// The ReleaseSentence() releases the vertex and index buffer of the sentence
// and the sentence as well
void TextClass::ReleaseSentence(SentenceType** ppSentence)
{
	if (*ppSentence)
	{
		_RELEASE((*ppSentence)->vertexBuffer);  // release the vertex buffer of the sentence
		_RELEASE((*ppSentence)->indexBuffer);   // release the index buffer of the sentence
		_DELETE(*ppSentence);                   // release the sentence
	}

	Log::Get()->Debug(THIS_FUNC_EMPTY);
	return;
}

// This function puts the sentence vertex and index buffer on the input assembler and
// then calls the FontShaderClass object to draw the sentence that was given as input
// to this function.
bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext,  SentenceType* pSentence,
	                           DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX orthoMatrix)
{
	bool result = false;
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	// set the vertices and indices buffers as active
	deviceContext->IASetVertexBuffers(0, 1, &(pSentence->vertexBuffer), &stride, &offset);

	// set the primitive topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetIndexBuffer(pSentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	

	// set the text colour
	DirectX::XMFLOAT4 pixelColor(pSentence->red, pSentence->green, pSentence->blue, 1.0f);

	// render the sentence using the FontShaderClass and HLSL shaders
	result = m_pFontShader->Render(deviceContext, static_cast<int>(pSentence->indexCount), 
		                           worldMatrix, m_baseViewMatrix, orthoMatrix,
		                           m_pFont->GetTexture(), pixelColor);
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