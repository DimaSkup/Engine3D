/////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
// Last revising: 29.03.22
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"

using namespace std;

ModelClass::ModelClass(void)
{
	modelWorldMatrix_ = DirectX::XMMatrixIdentity(); // by default we set the model at the beginning of the world

	// setup the model's default position and scale
	position_ = { 0.0f, 0.0f, 0.0f };
	scale_ = { 1.0f, 1.0f, 1.0f };
	radianAngle_ = { 0.0f, 0.0f };
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


// initialize a model using only custom vertices data (position, texture, normal)
bool ModelClass::Initialize(ID3D11Device* pDevice, const VERTEX* verticesData,
							const int vertexCount,
							string modelName)
{
	Log::Debug(THIS_FUNC, modelName.c_str());

	vertexCount_ = vertexCount;
	indexCount_ = vertexCount_;

	// Create the model using the vertex count
	pModelType_ = new(std::nothrow) ModelType[vertexCount_];
	if (!pModelType_)
	{
		Log::Error(THIS_FUNC, "can't create the model using the vertex count");
		return false;
	}

	// make model data structure
	for (size_t i = 0; i < vertexCount_; i++)
	{
		// setup the position coords
		pModelType_[i].x = verticesData[i].position.x;
		pModelType_[i].y = verticesData[i].position.y;
		pModelType_[i].z = verticesData[i].position.z;

		// setup the texture coords
		pModelType_[i].tu = verticesData[i].texture.x;
		pModelType_[i].tv = verticesData[i].texture.y;

		// setup the normals
		pModelType_[i].nx = verticesData[i].normal.x;
		pModelType_[i].ny = verticesData[i].normal.y;
		pModelType_[i].nz = verticesData[i].normal.z;

		// setup the colour
		pModelType_[i].cr = verticesData[i].color.x;  // red
		pModelType_[i].cg = verticesData[i].color.y;  // green
		pModelType_[i].cb = verticesData[i].color.z;  // blue
		pModelType_[i].ca = verticesData[i].color.w;  // alpha
	}

	// Initialize the vertex and index buffer that hold the geometry for the model
	if (!InitializeBuffers(pDevice))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the buffers");
		return false;
	}


	return true;
}


// The function here handle initializing of the model's vertex and 
// index buffers using some model data and texture
bool ModelClass::Initialize(ID3D11Device* pDevice, std::string modelName, WCHAR* textureFilename)
{
	// if we want to convert .obj file model data into the internal model format
	if (false)
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
	if (!AddTexture(pDevice, textureFilename))
	{
		Log::Get()->Error(THIS_FUNC, "can't load texture for the model");
		return false;
	}

	return true;
} // Initialize()



  // Put the vertex buffer data and index buffer data on the video card 
  // to prepare this data for rendering
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	this->RenderBuffers(deviceContext);

	return;
}


// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	ReleaseModel();     // release the model vertices data
	ReleaseTexture();   // Release the model texture

	return;
}


// Creates the texture object and then initialize it with the input file name provided.
bool ModelClass::AddTexture(ID3D11Device* device, WCHAR* filename)
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


// Get the number of indices
int ModelClass::GetIndexCount(void)
{
	return indexBuffer_.GetBufferSize();
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return pTexture_->GetTexture();
}

// returns a model world matrix
const DirectX::XMMATRIX & ModelClass::GetWorldMatrix()
{
	DirectX::XMMATRIX beginPosition = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(radianAngle_.y, radianAngle_.x, 0.0f);
	DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);
	
	modelWorldMatrix_ = beginPosition * rotation * scale * translate;

	return modelWorldMatrix_;
}


// modifies a model's position in the world
void ModelClass::SetPosition(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
	
	return;
}

// model's scaling
void ModelClass::SetScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
	
	return;
}

// model's rotation
void ModelClass::SetRotation(float radiansX, float radiansY)
{
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;

	return;
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



// Handles loading the model data from the text file into the m_model array variable.
// This model data must have an engine internal model type which was converted from some
// other model type (obj, fbx, 3dx, etc.)
bool ModelClass::LoadModel(std::string modelName)
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
void ModelClass::ReleaseModel(void)
{
	_DELETE(pModelType_);

	return;
}

// Initialization of the vertex and index buffers for some 3D model
bool ModelClass::InitializeBuffers(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	std::unique_ptr<VERTEX[]> pVertices = std::make_unique<VERTEX[]>(vertexCount_);
	std::unique_ptr<UINT[]>  pIndices  = std::make_unique<UINT[]>(indexCount_);


	// ----------------------------------------------------------------------- // 
	//             PREPARE DATA OF VERTICES AND INDICES                        //
	// ----------------------------------------------------------------------- //

	// Load the vertex array and index array with data
	for (size_t i = 0; i < vertexCount_; i++)
	{
		pVertices[i].position = { pModelType_[i].x, pModelType_[i].y, pModelType_[i].z };
		pVertices[i].texture  = { pModelType_[i].tu, pModelType_[i].tv };
		pVertices[i].normal   = { pModelType_[i].nx, pModelType_[i].ny, pModelType_[i].nz };
		pVertices[i].color    = { pModelType_[i].cr, pModelType_[i].cg, pModelType_[i].cb, pModelType_[i].ca };

		pIndices[i] = static_cast<UINT>(i);
	}


	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //

	// load vertex data
	hr = vertexBuffer_.InitializeDefault(pDevice, pVertices.get(), vertexCount_);
	if (FAILED(hr))
		return false;


	// load index data
	hr = indexBuffer_.Initialize(pDevice, pIndices.get(), indexCount_);
	if (FAILED(hr))
		return false;

	Log::Get()->Debug(THIS_FUNC, "model is initialized successfully");

	return true;
}

// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT offset = 0;

	// set the vertex buffer as active
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), vertexBuffer_.GetAddressOfStride(), &offset);

	// set the index buffer as active
	deviceContext->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// set which type of primitive topology we want to use
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}



// Releases the texture object that was created and loaded during the AddTexture function
void ModelClass::ReleaseTexture(void)
{
	// Release the texture object
	_SHUTDOWN(pTexture_);

	return;
}
