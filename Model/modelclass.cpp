/////////////////////////////////////////////////////////////////////
// Filename:        modelclass.cpp
// Description:     an implementation of the ModelClass class
// Last revising:   09.01.23
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

ModelClass::ModelClass(const ModelClass& copy) {}
ModelClass::~ModelClass(void) {}




// ------------------------------------------------------------------------------ //
//
//                           PUBLIC METHODS
//
// ------------------------------------------------------------------------------ //

/*


// initialize a model using only custom vertices data (position, texture, normal)
bool ModelClass::Initialize(ID3D11Device* pDevice, const VERTEX* verticesData,
							const int vertexCount,
							string modelName)
{
	Log::Debug(THIS_FUNC, modelName.c_str());

	bool result = false;

	vertexCount_ = vertexCount;
	indexCount_ = vertexCount_;

	// Create the model using the vertex count
	pModelType_ = new(std::nothrow) ModelType[vertexCount_];
	COM_ERROR_IF_FALSE(pModelType_, "can't create the model using the vertex count");

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
	result = InitializeBuffers(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the buffers");

	return true;
} /* Initialize() 

*/


// The function here handle initializing of the model's vertex and 
// index buffers using some model data and texture
bool ModelClass::Initialize(ID3D11Device* pDevice, 
							const std::string& modelId)
{
	bool result = false;
	bool executeModelConvertation = true;

	// if we want to convert .obj file model data into the internal model format
	if (executeModelConvertation)
	{
		std::string fileFormat = ".obj";
		std::string pathToModelFile = { MODEL_FILE_PATH + modelFilename_ + fileFormat };

		Log::Error("convert from file: %s: ", pathToModelFile.c_str());

		result = this->modelConverter_.ConvertFromObj(pathToModelFile);
		COM_ERROR_IF_FALSE(result, "can't convert .obj into the internal model format");
	}

	// Load in the model data from a file (internal type)
	result = this->LoadModel(modelFilename_);
	COM_ERROR_IF_FALSE(result, "can't load in the model data");

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	this->CalculateModelVectors();

	// Initialize the vertex and index buffer that hold the geometry for the model
	result = this->InitializeBuffers(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the buffers");


	string debugMsg = modelId + " is initialized!";
	Log::Debug(THIS_FUNC, debugMsg.c_str());


	return true;
} /* Initialize() */


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
	_DELETE(pModelType_);         // release the model vertices data
	textureArray_.Shutdown();     // release the texture objects

	return;
}


// initializes a texture with the input file names provided.
bool ModelClass::AddTexture(ID3D11Device* device, WCHAR* texture)
{
	if (texture != nullptr)
	{
		bool result = false;

		// add a new texture
		result = this->textureArray_.AddTexture(device, texture);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture object");
	}

	return true;
}


// set a model for this ModelClass object
void ModelClass::SetModel(const std::string& modelFilename)
{
	modelFilename_ = modelFilename;
}


// Get the number of indices
int ModelClass::GetIndexCount(void)
{
	return indexBuffer_.GetBufferSize();
}


// returns a pointer to the array of textures
ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return this->textureArray_.GetTextureArray();
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


// getters
const DirectX::XMFLOAT3& ModelClass::GetPosition() const { return position_; }
const DirectX::XMFLOAT3& ModelClass::GetScale() const    { return scale_; }
const DirectX::XMFLOAT2& ModelClass::GetRotation() const { return radianAngle_; }







// memory allocation (we need it because we use DirectX::XM-objects)
void* ModelClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	if (!ptr)
	{
		Log::Error(THIS_FUNC, "can't allocate the memory for object");
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
	Log::Debug(THIS_FUNC_EMPTY);

	std::string modelFilename = { MODEL_FILE_PATH + modelName + ".txt" }; // prepare the path to a model data file
	std::ifstream fin(modelFilename, std::ios::in);
	float tempValue = 0.0f;
	char input = ' ';

	Log::Print("filename: %s", modelFilename.c_str());

	// Open the model file
	//fin.open(modelFilename.c_str());

	// If it could not open the file then exit
	if (fin.fail())
	{
		std::string errorMsg = "can't open the text file \"" + modelFilename + "\" with model data";
		Log::Error(THIS_FUNC, errorMsg.c_str());
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
	COM_ERROR_IF_FALSE(pModelType_, "can't create the model using the vertex count");

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
		//fin >> pModelType_[i].nx >> pModelType_[i].ny >> pModelType_[i].nz;
		fin >> tempValue >> tempValue >> tempValue;
		//pModelType_[i].nx = pModelType_[i].ny = pModelType_[i].nz = 0.0f;
	}

	// Close the model file
	fin.close();

	//Log::Debug(THIS_FUNC, "the model was read in successfully");

	return true;
} /* LoadModel() */



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
		pVertices[i].tangent  = { pModelType_[i].tx, pModelType_[i].ty, pModelType_[i].tz };
		pVertices[i].binormal = { pModelType_[i].bx, pModelType_[i].by, pModelType_[i].bz };
		pVertices[i].color    = { pModelType_[i].cr, pModelType_[i].cg, pModelType_[i].cb, pModelType_[i].ca };

		pIndices[i] = static_cast<UINT>(i);
	}


	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //

	// load vertex data
	hr = vertexBuffer_.InitializeDefault(pDevice, pVertices.get(), vertexCount_);
	COM_ERROR_IF_FAILED(hr, "can't initialize a default vertex buffer for the model");

	// load index data
	hr = indexBuffer_.Initialize(pDevice, pIndices.get(), indexCount_);
	COM_ERROR_IF_FAILED(hr, "can't initialize an index buffer for the model");

	return true;
} /* InitializeBuffers() */



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







// CalculateModelVectors() generates the tangent and binormal for the model as well as 
// a recalculated normal vector. To start it calculates how many faces (triangles) are
// in the model. Then for each of those triangles it gets the three vertices and uses
// that to calculate the tangent, binormal, and normal. After calculating those three
// normal vectors it then saves them back into the model structure.
void ModelClass::CalculateModelVectors()
{
	//Log::Debug(THIS_FUNC_EMPTY);

	int faceCount = 0;	// the number of faces in the model
	int index = 0;		// the index to the model data

	TempVertexType vertices[3];

	VectorType tangent;
	VectorType binormal;
	VectorType normal;


	// calculate the number of faces in the model
	faceCount = this->vertexCount_ / 3;  // ATTENTION: don't use "this->vertexBuffer_.GetBufferSize()" because at this point we haven't initialized the vertex buffer yet

	// go throught all the faces and calculate the tangent, binormal, and normal vectors
	for (size_t i = 0; i < faceCount; i++)
	{
		// get the three vertices for this face from the model
		for (size_t vertexIndex = 0; vertexIndex < 3; vertexIndex++)
		{
			vertices[vertexIndex].x = pModelType_[index].x;
			vertices[vertexIndex].y = pModelType_[index].y;
			vertices[vertexIndex].z = pModelType_[index].z;
			vertices[vertexIndex].tu = pModelType_[index].tu;
			vertices[vertexIndex].tv = pModelType_[index].tv;
			vertices[vertexIndex].nx = pModelType_[index].nx;
			vertices[vertexIndex].ny = pModelType_[index].ny;
			vertices[vertexIndex].nz = pModelType_[index].nz;
			index++;
		}


		// calculate the tangent and binormal of that face
		this->CalculateTangentBinormal(vertices[0], vertices[1], vertices[2], tangent, binormal);

		// calculate the new normal using the tangent and binormal
		this->CalculateNormal(tangent, binormal, normal);

		// store the normal, tangent, and binormal for this face back in the model structure;
	
		for (size_t backIndex = 3; backIndex > 0; backIndex--)
		{
			pModelType_[index - backIndex].nx = normal.x;
			pModelType_[index - backIndex].ny = normal.y;
			pModelType_[index - backIndex].nz = normal.z;
			pModelType_[index - backIndex].tx = tangent.x;
			pModelType_[index - backIndex].ty = tangent.y;
			pModelType_[index - backIndex].tz = tangent.z;
			pModelType_[index - backIndex].bx = binormal.x;
			pModelType_[index - backIndex].by = binormal.y;
			pModelType_[index - backIndex].bz = binormal.z;
		}
		
		


	}


	return;
} /* CalculateModelVectors() */


// the CalculateTangentBinormal() takes in three vertices and then
// calculates and returns the tangent and binormal of those three vertices
void ModelClass::CalculateTangentBinormal(TempVertexType vertex1,
										  TempVertexType vertex2,
										  TempVertexType vertex3,
										  VectorType& tangent,
										  VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;    // denominator of the tangent/binormal equation
	float length; // length of the tangent/binormal



	// calculate the two vectors for this face
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// calculate the tu and tv texture space vectors
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// calculate the denominator of the tangent/binormal equation
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// calculate the cross products and multiply by the coefficient to get 
	// the tangent and binormal;
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	 
	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// calculate the length of the tangent
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// normalize the tangent components and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;


	// calculate the length of the binormal
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// normalize the binormal components and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
} /* CalculateTangentBinormal() */


// the CalculateNormal() takes in the tangent and binormal and the does a cross product
// to give back the normal vector
void ModelClass::CalculateNormal(VectorType tangent,
								 VectorType binormal,
								 VectorType& normal)
{
	float length = 0.0f;  // the length of the normal vector

	// calculate the cross product of the tangent and binormal which will give
	// the normal vector
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// calculate the length of the normal
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// normalize the normal
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
} 