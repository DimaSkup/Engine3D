/////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
// Last revising: 29.03.22
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"


using namespace std;

ModelClass::ModelClass(void)
{
	pVertexBuffer_ = nullptr;
	pIndexBuffer_ = nullptr;
	pTexture_ = nullptr;
	pModelType_ = nullptr;
}

ModelClass::ModelClass(const ModelClass& another)
{
}

ModelClass::~ModelClass(void)
{
}

// ------------------------------------------------------------------------------ //
//
//                           PUBLIC METHODS
//
// ------------------------------------------------------------------------------ //

// Initialization of the model
bool ModelClass::Initialize(ID3D11Device* device, std::string modelName, WCHAR* textureFilename)
{
	// if we want to convert .obj file model data into the internal model format
	if (true)
	{
		if (!this->modelConverter.ConvertFromObj(modelName += ".obj"))
		{
			Log::Get()->Error(THIS_FUNC, "can't convert .obj into the internal model format");
			return false;
		}
	}

	// Load in the model data from a file (internal type)
	if (!LoadModel(modelName))
	{
		Log::Get()->Error(THIS_FUNC, "can't load in the model data");
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the model
	if (!InitializeBuffers(device))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the buffers");
		return false;
	}

	// Load the texture for this model
	if (!LoadTexture(device, textureFilename))
	{
		Log::Get()->Error(THIS_FUNC, "can't load texture for the model");
		return false;
	}

	return true;
} // Initialize()


// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	ReleaseTexture();   // Release the model texture
	ShutdownBuffers();  // release the memory from the vertex and index buffer
	ReleaseModel();     // release the model data

	return;
}

// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

// Get the number of indices
int ModelClass::GetIndexCount(void)
{
	return indexCount_;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return pTexture_->GetTexture();
}



// memory allocation (we need it because we use DirectX::XM-objects)
void* ModelClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void ModelClass::operator delete(void* p)
{
	_aligned_free(p);
}





// ------------------------------------------------------------------------------ //
//
//                           PRIVATE METHODS
//
// ------------------------------------------------------------------------------ //

// Initialization of the vertex and index buffers
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	Log::Print(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	VERTEX* vertices = nullptr;
	unsigned long* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexBufferData, indexBufferData;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// allocate the memory for the vertices and indices
	vertices = new(std::nothrow) VERTEX[vertexCount_];
	if (!vertices)
	{
		Log::Error(THIS_FUNC, "can't allocate the memory for the vertices array");
		return false;
	}

	indices = new(std::nothrow) unsigned long[indexCount_];
	if (!indices)
	{
		Log::Error(THIS_FUNC, "can't allocate the memory for the indices array");
		return false;
	}

	// Load the vertex array and index array with data
	for (size_t i = 0; i < vertexCount_; i++)
	{
		vertices[i].position = DirectX::XMFLOAT3(pModelType_[i].x, pModelType_[i].y, pModelType_[i].z);
		vertices[i].texture  = DirectX::XMFLOAT2(pModelType_[i].tu, pModelType_[i].tv);
		vertices[i].normal   = DirectX::XMFLOAT3(pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz);

		indices[i] = static_cast<unsigned long>(i);
	}

	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //

					// Setup the vertex buffer description
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * vertexCount_;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Fill in initial vertices data 
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	// Create and initialize a vertex buffer using the vertex buffer description and vertex data
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pVertexBuffer_);
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't create the vertex buffer");
		return false;
	}

	// Setup the index buffer description
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount_;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Fill in initial indices data
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	// Create an index buffer using the index buffer description
	hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIndexBuffer_);
	if (FAILED(hr))
	{
		Log::Error(THIS_FUNC, "can't create the index buffer");
		return false;
	}


	// release the vertex and index arrays because we already have buffers are initialized
	_DELETE(vertices);
	_DELETE(indices);


	Log::Get()->Debug(THIS_FUNC, "model is initialized successfully");

	return true;
}

// Releasing of the allocated memory from the vertex and index buffers
void ModelClass::ShutdownBuffers(void)
{
	_RELEASE(pIndexBuffer_);
	_RELEASE(pVertexBuffer_);

	return;
}


// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	// set the vertex buffer as active
	deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// set the index buffer as active
	deviceContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// set which type of primitive topology we want to use
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

// Creates the texture object and then initialize it with the input file name provided.
bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	// Create the texture object
	pTexture_ = new(std::nothrow) TextureClass;
	if (!pTexture_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the texture object");
		return false;
	}

	// Initialize the texture object
	if (!pTexture_->Initialize(device, filename))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture object");
		return false;
	}

	return true;
}

// Releases the texture object that was created and loaded during the LoadTexture function
void ModelClass::ReleaseTexture(void)
{
	// Release the texture object
	_SHUTDOWN(pTexture_);

	return;
}

// Handles loading the model data from the text file into the m_model array variable.
// This model data must have an engine internal model type which was converted from some
// other model type (obj, fbx, 3dx, etc.)
bool ModelClass::LoadModel(std::string modelName)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	std::string modelFilename = { modelName + ".txt" };
	std::ifstream fin;
	char input = ' ';
	int i = 0;
	
	// Open the model file
	fin.open(modelFilename.c_str());

	// If it could not open the file then exit
	if (fin.fail())
	{
		Log::Get()->Error(THIS_FUNC, "can't open the text file with model data");
		return false;
	}

	// Read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count
	fin >> vertexCount_;

	// Set the number of indices to be the same as the vertex count
	indexCount_ = vertexCount_;

	// Create the model using the vertex count that was read in
	pModelType_ = new(std::nothrow) ModelType[vertexCount_];
	if (!pModelType_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the model using the vertex count");
		return false;
	}

	// Read up to the beginning of the data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data
	for (size_t i = 0; i < vertexCount_; i++)
	{
		fin >> pModelType_[i].x >> pModelType_[i].y >> pModelType_[i].z;
		fin >> pModelType_[i].tu >> pModelType_[i].tv;
		fin >> pModelType_[i].nx >> pModelType_[i].ny >> pModelType_[i].nz;
	}

	// Close the model file
	fin.close();

	Log::Get()->Debug(THIS_FUNC, "the model was read in successfully");

	return true;
}

// handles deleting the model data array
void ModelClass::ReleaseModel(void)
{
	_DELETE(pModelType_);

	return;
}