////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass(void)
{
	m_pFont = nullptr;
	m_pFontShader = nullptr;

	m_pSentence1 = nullptr;
	m_pSentence2 = nullptr;
	m_ppSentences = nullptr;
	m_sentencesCount = 5;

	Log::Get()->Print(THIS_FUNC_EMPTY);
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


	// -------------------- FONT AND FONT SHADER CLASSES ---------------------------- //

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
		Log::Get()->Error(THIS_FUNC, "can't initialzie the font object");
		return false;
	}


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

	// --------------------- CREATION OF SENTENCES ----------------------------------- //

	char* pSentence[5] = { "first", "second", "third", "fourth", "fifth" };
	char** ppText = new char*[m_sentencesCount]; // a pointer to an array of pointer to char
	m_ppSentences = new(std::nothrow) SentenceType*[m_sentencesCount];
	if (!m_ppSentences)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for an array of pointers to SentenceType objects");
		return false;
	}
	
	// initialize an array of pointers to char
	for (size_t i = 0; i < m_sentencesCount; i++)
	{
		int sentenceLength = sizeof(pSentence[i]);
		ppText[i] = new char[sentenceLength];
		memcpy(ppText[i], pSentence[i], sentenceLength);
	}

	
	for (size_t i = 0; i < m_sentencesCount; i++)
	{
		// initialize the sentence
		result = InitializeSentence(&(m_ppSentences[i]), 16, device);
		if (!result)
		{
			Log::Get()->Error(THIS_FUNC, "can't initialize the sentence");
			return false;
		}

		// update the sentence
		result = UpdateSentence(m_ppSentences[i], ppText[i], 10, i * 50, i * 0.1f, i * 0.2f, i * 0.3f, deviceContext);
		if (!result)
		{
			Log::Get()->Error(THIS_FUNC, "can't update the sentece");
			return false;
		}
	}

	Log::Get()->Print(THIS_FUNC, "is initialized");

	return true;
} // Initialize()


// The Shutdown() will release the sentences, the font object, and the font shader object
void TextClass::Shutdown(void)
{
	ReleaseSentence(&m_pSentence1);  // release the first sentence
	ReleaseSentence(&m_pSentence2);  // release the second sentence
	
	_SHUTDOWN(m_pFontShader);        // release the font shader object
	_SHUTDOWN(m_pFont);              // release the font object

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, 
	                   DirectX::XMMATRIX worldMatrix,
	                   DirectX::XMMATRIX orthoMatrix)
{
	bool result = false;


	for (size_t i = 0; i < m_sentencesCount; i++)
	{
		// draw the sentence
		result = RenderSentence(deviceContext, m_ppSentences[i], worldMatrix, orthoMatrix);
		if (!result)
		{
			Log::Get()->Error("%s()::%d: %s %d %s", __FUNCTION__, __LINE__, "can't render the", i, "sentence");
			return false;
		}

	}

	return true;
}


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

// The InitializeSentence() creates a SentenceType with an empty vertex buffer which will
// be used to store and render sentences. The maxLenght input parameters determines
// how large the vertex buffer will be. All sentences have a vertex and index buffer
// associated with them which is initialize first in this function
bool TextClass::InitializeSentence(SentenceType** ppSentence, int maxLength, ID3D11Device* device)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	VERTEX* vertices = nullptr;
	ULONG* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr = S_OK;
	int i = 0;


	// ----------------------- VERTEX AND INDEX ARRAYS --------------------------------- // 

	// create a new sentence object
	*ppSentence = new(std::nothrow) SentenceType;
	if (!*ppSentence)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate memory for a new SentenceType object");
		return false;
	}

	// initialize the sentence buffers to null
	(*ppSentence)->vertexBuffer = nullptr;
	(*ppSentence)->indexBuffer = nullptr;

	// set the maximum length of the sentence
	(*ppSentence)->maxLength = maxLength;

	// set the number of vertices in the vertex and the index arrays
	(*ppSentence)->vertexCount = 6 * maxLength;
	(*ppSentence)->indexCount = (*ppSentence)->vertexCount;
	

	// create the vertex array (we don't need to initialize it to zeros because it is already done)
	vertices = new(std::nothrow) VERTEX[(*ppSentence)->vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the VERTEX array");
		return false;
	}

	// create the index array
	indices = new(std::nothrow) ULONG[(*ppSentence)->indexCount];
	if (!indices)
	{
		_DELETE(vertices);
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the index array");
		return false;
	}

	// initialize the index array 
	for (i = 0; i < (*ppSentence)->indexCount; i++)
	{
		indices[i] = i;
	}


	// ------------------------- VERTEX AND INDEX BUFFERS ------------------------------ //

	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * (*ppSentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create the vertex buffer
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*ppSentence)->vertexBuffer);
	if (FAILED(hr))
	{
		_DELETE(vertices);
		_DELETE(indices);
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
	}

	// set up the description of the static index buffer
	indexBufferDesc.ByteWidth = sizeof(ULONG) * (*ppSentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create the index buffer
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &(*ppSentence)->indexBuffer);
	if (FAILED(hr))
	{
		_DELETE(vertices);
		_DELETE(indices);
		Log::Get()->Error(THIS_FUNC, "can't create the index buffer");
		return false;
	}


	// release the vertex and index arrays as they are no loonger needed
	_DELETE(vertices);
	_DELETE(indices);

	return true;
}


// UpdateSentence() changes the contents of the vertex buffer for the input sentence.
// It uses the Map and Unmap functions along with memcpy to update the contents 
// of the vertex buffer
bool TextClass::UpdateSentence(SentenceType* sentence, char* text, 
	                           int posX, int posY, float red, float green, float blue,
	                           ID3D11DeviceContext* deviceContext)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	int numLetters = 0;
	VERTEX* vertices = nullptr;
	float drawX = 0.0f, drawY = 0.0f;
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	VERTEX* verticesPtr = nullptr;
	void* fontClassVertices = nullptr;

	// --------------- SET THE COLOUR AND SIZE OF THE SENTENCE -------------------------- //

	// store the colour of the sentence
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// get the number of letters in the sentence
	numLetters = static_cast<int>(strlen(text));

	// check for possible buffer overflow
	if (numLetters > sentence->maxLength)
	{
		Log::Get()->Error(THIS_FUNC, "there is a buffer overflow in the sentence!");
		return false;
	}

	// create the VERTEX array, this array is already initialized to zeros after creation
	vertices = new(std::nothrow) VERTEX[sentence->vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the VERTEX array");
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VERTEX) * sentence->vertexCount));


	// -------------------- BUILD THE VERTEX ARRAY ------------------------------------ // 

	// calculate the X and Y pixel position on the screen to start drawing to
	drawX = static_cast<float>(((m_screenWidth / 2) * -1) + posX);
	drawY = static_cast<float>(((m_screenHeight / 2) - posY));

	// use the font class to build the vertex array from the sentence text and sentence draw location
	//fontClassVertices = vertices;  // we need to get the pointer to vertices as void* in order to pass it into BuildVertexArray() 
	m_pFont->BuildVertexArray((void*)vertices, text, drawX, drawY);



	// --------------------- FILL IN THE VERTEX BUFFER WITH DATA ---------------------- //

	// lock the vertex buffer so it can be written to
	hr = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(hr))
	{
		_DELETE(vertices);
		Log::Get()->Error(THIS_FUNC, "can't Map() the vertex buffer of the sentence");
		return false;
	}

	// get a pointer to the data in the vertex buffer
	verticesPtr = static_cast<VERTEX*>(mappedSubresource.pData);

	// copy the data into the vertex buffer
/*
for (size_t i = 0; i < sentence->vertexCount; i++)
{
verticesPtr[0] = vertices[0];
}
*/

	memcpy(verticesPtr, (void*)vertices, (sizeof(VERTEX) * sentence->vertexCount));


	// unlock the vertex buffer
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// release the vertex array
	_DELETE(vertices);

	return true;
}


// ReleaseSentence() is used to release the sentence vertex and index buffer as well 
// as the sentence itself
void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		_RELEASE((*sentence)->vertexBuffer); // release the sentence vertex buffer
		_RELEASE((*sentence)->indexBuffer);  // release the sentence index buffer
		_DELETE(*sentence);                  // release the sentence
	}

	return;
}


// This function puts the sentence vertex and index buffer on the input assembler and
// then calls the FontShaderClass object to draw the sentence that was given as input
// to this function.
bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence,
	                           DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX orthoMatrix)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	bool result = false;
	DirectX::XMFLOAT4 pixelColor{ 1.0f, 1.0f, 1.0f, 1.0f };


	// set the vertex and index buffers to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// create a pixel color vector with the input sentence color
	pixelColor = DirectX::XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// render the text using the font shader
	result = m_pFontShader->Render(deviceContext, sentence->indexCount, 
		                           worldMatrix, m_baseViewMatrix,
		                           orthoMatrix, m_pFont->GetTexture(), pixelColor);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the sentence");
		return false;
	}

	return true;
}