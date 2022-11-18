#include "model3D.h"

// The function here handle initializing of the model's vertex and 
// index buffers using some model data and texture
bool Model3D::Initialize(ID3D11Device* pDevice, std::string modelName, WCHAR* textureFilename)
{
	// if we want to convert .obj file model data into the internal model format
	if (true)
	{
		if (!this->modelConverter.ConvertFromObj(modelName + ".obj"))
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
	if (!InitializeBuffers(pDevice))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the buffers");
		return false;
	}

	// Load the texture for this model
	if (!LoadTexture(pDevice, textureFilename))
	{
		Log::Get()->Error(THIS_FUNC, "can't load texture for the model");
		return false;
	}

	return true;
} // Initialize()



  // Put the vertex buffer data and index buffer data on the video card 
  // to prepare this data for rendering
void Model3D::Render(ID3D11DeviceContext* deviceContext)
{
	this->RenderBuffers(deviceContext);

	return;
}

  // memory allocation (we need it because we use DirectX::XM-objects)
void* Model3D::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for object");
		return nullptr;
	}

	return ptr;
}

void Model3D::operator delete(void* p)
{
	_aligned_free(p);
}




  // Initialization of the vertex and index buffers for some 3D model
bool Model3D::InitializeBuffers(ID3D11Device* pDevice)
{
	Log::Debug(THIS_FUNC_EMPTY);

	HRESULT hr = S_OK;
	VERTEX_3D* vertices = nullptr;
	unsigned long* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexBufferData, indexBufferData;

	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// allocate the memory for the vertices and indices
	vertices = new(std::nothrow) VERTEX_3D[vertexCount_];
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
		vertices[i].texture = DirectX::XMFLOAT2(pModelType_[i].tu, pModelType_[i].tv);
		vertices[i].normal = DirectX::XMFLOAT3(pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz);

		indices[i] = static_cast<unsigned long>(i);
	}

	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //

	// Setup the vertex buffer description
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * vertexCount_;
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
	hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pVertexBuffer_);
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
	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIndexBuffer_);
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


// Handles loading the model data from the text file into the m_model array variable.
// This model data must have an engine internal model type which was converted from some
// other model type (obj, fbx, 3dx, etc.)
bool Model3D::LoadModel(std::string modelName)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	std::string modelFilename = { modelName + ".txt" }; // prepare the path to a model data file
	std::ifstream fin;
	char input = ' ';
	int i = 0;

	// Open the model file
	fin.open(modelFilename.c_str());

	// If it could not open the file then exit
	if (fin.fail())
	{
		Log::Error(THIS_FUNC, "can't open the text file with model data");
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
		Log::Error(THIS_FUNC, "can't create the model using the vertex count");
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
void Model3D::ReleaseModel(void)
{
	_DELETE(pModelType_);

	return;
}