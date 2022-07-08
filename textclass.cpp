////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
// Revising: 04.07.22
////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass(void)
{
	m_pFont = nullptr;
	m_pFontShader = nullptr;

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
		Log::Get()->Error(THIS_FUNC, "can't initialize the font object");
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



// The Shutdown() will release the sentences, font class object and font shader object
void TextClass::Shutdown(void)
{
	for (size_t i = 0; i < m_sentencesCount; i++)
	{
		ReleaseSentence(m_ppSentences + i); // release the sentece
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

	// render sentences
	for (size_t i = 0; i < m_sentencesCount; i++)
	{
		result = RenderSentence(deviceContext, m_ppSentences[i], worldMatrix, orthoMatrix);
		if (!result)
		{
			Log::Get()->Error("%s()::%d %s %d", __FUNCTION__, __LINE__, "can't render the sentence #", i);
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

	HRESULT hr = S_OK;
	VERTEX* vertices = nullptr;
	ULONG* indices = nullptr;
	D3D11_SUBRESOURCE_DATA initData;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;

	// ------------------------ INITIALIZE THE EMPTY SENTENCE --------------------------// 
	*ppSentence = new(std::nothrow) SentenceType;

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
		indices[i] = i;
	}


	// ----------------------- VERTEX AND INDEX BUFFERS -------------------------------- //

	// set up the vertex buffer description
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * (*ppSentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.MiscFlags = 0;
	
	// prepare data for the vertex buffer
	initData.pSysMem = vertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// create the vertex buffer
	hr = device->CreateBuffer(&vertexBufferDesc, &initData, &((*ppSentence)->vertexBuffer));
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
		_DELETE(vertices);
		_DELETE(indices);

		return false;
	}


	// set up the index buffer description
	indexBufferDesc.ByteWidth = sizeof(ULONG) * (*ppSentence)->indexCount;
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
	hr = device->CreateBuffer(&indexBufferDesc, &initData, &((*ppSentence)->indexBuffer));
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


	Log::Get()->Debug(THIS_FUNC, "the sentence is initialized");

	return true;
}


// UpdateSentence() changes the contents of the vertex buffer for the input sentence.
// It uses the Map and Unmap functions along with memcpy to update the contents 
// of the vertex buffer
bool TextClass::UpdateSentence(SentenceType* pSentence, char* text,
	                           int posX, int posY,                   // position to draw at
	                           float red, float green, float blue,   // text colour
	                           ID3D11DeviceContext* deviceContext)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	bool result = false;
	int textLength = sizeof(text);
	VERTEX* vertices = nullptr;     // points at vertices of the updated sentence
	VERTEX* verticesPtr = nullptr;  // points at the mapped vertex buffer of the sentence object
	D3D11_MAPPED_SUBRESOURCE mappedData;
	int drawX = 0, drawY = 0;


	// set up the text colour
	pSentence->red = red;
	pSentence->green = green;
	pSentence->blue = blue;

	// check if the text buffer overflow
	if (pSentence->maxLength < textLength)
	{
		Log::Get()->Error(THIS_FUNC, "the text buffer is overflow");
		return false;
	}

	// -------------------- BUILD THE VERTEX ARRAY ------------------------------------ // 

	// create a vertices array (it is already filled with zeros during the creation)
	vertices = new(std::nothrow) VERTEX[pSentence->vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}


	// calculate the position of the sentence on the screen
	drawX = static_cast<int>((m_screenWidth / -2) + posX);
	drawY = static_cast<int>(m_screenHeight / 2 - posY);

	// fill in the vertex array with vertices data of the new sentence
	m_pFont->BuildVertexArray((void*)vertices, text, drawX, drawY);

	


	// --------------------- FILL IN THE VERTEX BUFFER WITH DATA ---------------------- //

	// locking of the vertex buffer to get access to it
	hr = deviceContext->Map(pSentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		Log::Get()->Error(THIS_FUNC, "can't Map() the vertex buffer of the sentence object");
		_DELETE(vertices); // clean the vertices

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
	deviceContext->Unmap(pSentence->vertexBuffer, 0);

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
	deviceContext->IASetIndexBuffer(pSentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// set the primitive topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set the text colour
	DirectX::XMFLOAT4 pixelColor(pSentence->red, pSentence->green, pSentence->blue, 1.0f);

	// render the sentence using the FontShaderClass and HLSL shaders
	result = m_pFontShader->Render(deviceContext, pSentence->indexCount, 
		                           worldMatrix, m_baseViewMatrix, orthoMatrix,
		                           m_pFont->GetTexture(), pixelColor);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the sentence");
		return false;
	}

	return true;
}