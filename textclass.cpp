////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass(void)
{
	m_pFont = nullptr;
	m_pFontShader = nullptr;
	
	m_pSentence1 = nullptr;
	m_pSentence2 = nullptr;

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

TextClass::TextClass(const TextClass& copy)
{
}

TextClass::~TextClass(void)
{
}



// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //
bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// store the screen width and heihgt
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// store the base view matrix
	m_baseViewMatrix = baseViewMatrix;

	// create the font object
	m_pFont = new(std::nothrow) FontClass;
	if (!m_pFont)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the FontClass object");
		return false;
	}

	// initialize the font object
	result = m_pFont->Initialize(device, "data/fontdata.txt", L"data/font.dds");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font object");
	}

	// create the font shader object
	m_pFontShader = new(std::nothrow) FontShaderClass;
	if (!m_pFontShader)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the FontShaderClass object");
		return false;
	}

	// initalize the font shader object
	result = m_pFontShader->Initialize(device, hwnd);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the font shader object");
		return false;
	}


	// --------------------- CREATION OF SENTENCES ----------------------------------- //

	// initialize the first sentence
	result = InitializeSentence(&m_pSentence1, 16, device);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the first sentence");
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_pSentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the first sentence");
		return false;
	}



	// initialize the second sentence
	result = InitializeSentence(&m_pSentence2, 16, device);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the second sentence");
		return false;
	}

	// now update the sentence vertex buffer with the new string information
	result = UpdateSentence(m_pSentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't update the second sentence");
		return false;
	}

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
	                   D3DXMATRIX worldMatrix, 
	                   D3DXMATRIX orthoMatrix)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;

	// draw the first sentence
	result = RenderSentence(deviceContext, m_pSentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the first sentence");
		return false;
	}

	// draw the second sentence
	result = RenderSentence(deviceContext, m_pSentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the second sentence");
		return false;
	}

	return true;
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
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
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
	*sentence = new(std::nothrow) SentenceType;
	if (!*sentence)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate memory for a new SentenceType object");
		return false;
	}

	// initialize the sentence buffers to null
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;

	// set the maximum length of the sentence
	(*sentence)->maxLength = maxLength;

	// set the number of vertices in the vertex and the index arrays
	(*sentence)->vertexCount = 6 * maxLength;
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// create the vertex array
	vertices = new(std::nothrow) VERTEX[(*sentence)->vertexCount];
	if (!vertices)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the VERTEX array");
		return false;
	}

	// create the index array
	indices = new(std::nothrow) ULONG[(*sentence)->indexCount];
	if (!indices)
	{
		_DELETE(vertices);
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the index array");
		return false;
	}

	// initialize vertex array to zeros at first
	memset(vertices, 0, (sizeof(VERTEX) * (*sentence)->vertexCount));

	// initialize the index array 
	for (i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}



	// ------------------------- VERTEX AND INDEX BUFFERS ------------------------------ //

	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * (*sentence)->vertexCount;
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
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(hr))
	{
		_DELETE(vertices);
		_DELETE(indices);
		Log::Get()->Error(THIS_FUNC, "can't create the vertex buffer");
	}


	// set up the description of the static index buffer
	indexBufferDesc.ByteWidth = sizeof(ULONG) * (*sentence)->indexCount;
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
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
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

	// calculate the X and Y pixel position on the screen to start drawing to
	drawX = 

	return true;
}