/////////////////////////////////////////////////////////////////////
// Filename:        modelclass.cpp
// Description:     an implementation of the ModelClass class
// Last revising:   09.01.23
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass(void)
{
	try
	{
		pData_ = new ModelData();                  // allocate memory for a model data object
		pTexturesList_ = new TextureArrayClass();  // create an empty textures array object
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some object of the class");
	}
}

// the copy constructor
ModelClass::ModelClass(const ModelClass& copy) 
{
}

ModelClass::~ModelClass(void) 
{
	this->Shutdown();
}




// ------------------------------------------------------------------------------ //
//
//                           PUBLIC METHODS
//
// ------------------------------------------------------------------------------ //

// The function here handle initializing of the model's vertex and 
// index buffers using some model data and texture
bool ModelClass::Initialize(ID3D11Device* pDevice, const std::string& modelId)
{
	try
	{
		std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); // for calculations of the model's normal vector, binormal, etc.
		bool result = false;
		bool executeModelConvertation = false;

		// if we want to convert file model data into the internal model format
		if (executeModelConvertation)
		{
			std::unique_ptr<ModelConverterClass> pModelConverter = std::make_unique<ModelConverterClass>(); 
			std::string pathToModelFile { SETTINGS::GetSettings()->MODEL_DIR_PATH + pData_->GetModelType() + ".obj"};

			result = pModelConverter->ConvertFromObj(pathToModelFile);
			COM_ERROR_IF_FALSE(result, "can't convert .obj into the internal model format");
		}

		// Load in the model data from a file (internal type)
		result = this->LoadModel(pData_->GetModelType());
		COM_ERROR_IF_FALSE(result, "can't load in the model data");

		// after the model data has been loaded we now call the CalculateModelVectors() to
		// calculate the tangent and binormal. It also recalculates the normal vector;
		pModelMath->CalculateModelVectors(pData_->GetVerticesData(), pData_->GetVertexCount());

		// Initialize the vertex and index buffer that hold the geometry for the model
		result = this->InitializeBuffers(pDevice, pData_->GetVerticesData(), pData_->GetIndicesData(), pData_->GetVertexCount(), pData_->GetIndexCount());
		COM_ERROR_IF_FALSE(result, "can't initialize the buffers");

		pData_->SetID(modelId);

		// print a message about the initialization process
		string debugMsg = modelId + " is initialized!";
		Log::Debug(THIS_FUNC, debugMsg.c_str());


		return true;
	}
	catch (COMException & e)
	{
		Log::Error(e);
		return false;
	}
} /* Initialize(pDevice, modelId) */


// initialize a copy of the model using the data of the original object
bool ModelClass::InitializeCopy(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId)
{
	try
	{
		bool result = false;

		// copy model's data
		pData_->SetID(modelId);
		pData_->SetIndexCount(pModel->GetIndexCount());
		pData_->SetVerticesData(pModel->GetVerticesData(), pModel->GetVertexCount());
		pData_->SetIndexData(pModel->GetIndicesData(), pModel->GetIndexCount());

		// Initialize the vertex and index buffer that hold the geometry for the model
		result = this->InitializeBuffers(pDevice, GetVerticesData(), GetIndicesData(), GetVertexCount(), GetIndexCount());
		COM_ERROR_IF_FALSE(result, "can't initialize the buffers");

		//string debugMsg = modelId + " is initialized!";
		//Log::Debug(THIS_FUNC, debugMsg.c_str());

		return true;
	}
	catch (COMException& e)
	{
		Log::Error(e);
		return false;
	}
} /* Initialize(pModel, modelId) */


// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void ModelClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	this->RenderBuffers(pDeviceContext);
	pMediator_->Render(pDeviceContext);

	return;
}


// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	_SHUTDOWN(pTexturesList_);    // release the texture objects 
	_DELETE(pMediator_);          // release the model mediator
	this->ShutdownBuffers();      // release the vertex/index buffers
	this->ClearModelData();
	_DELETE(pData_);

	return;
}


// release memory from the model vertices/indices data
void ModelClass::ClearModelData()
{
	pData_->Shutdown();

	return;
}

// add a new texture at the end of the textures list
bool ModelClass::AddTexture(ID3D11Device* pDevice, WCHAR* textureName)
{
	assert(pDevice != nullptr);
	assert(textureName != nullptr);

	// add a new texture
	bool result = this->pTexturesList_->AddTexture(pDevice, textureName);
	COM_ERROR_IF_FALSE(result, "can't add a new texture object");
	

	return true;
}


// set a new texture by some particular index
bool ModelClass::SetTexture(ID3D11Device* pDevice, WCHAR* textureName, UINT index)
{
	assert(pDevice != nullptr);
	assert(textureName != nullptr);

	// set a new texture
	bool result = this->pTexturesList_->SetTexture(pDevice, textureName, index);
	COM_ERROR_IF_FALSE(result, "can't set a new texture by the index");

	return true;
}

// returns a pointer to the array of textures
ID3D11ShaderResourceView* const* ModelClass::GetTextureResourcesArray()
{
	return this->pTexturesList_->GetTextureResourcesArray();
}



// common getters:

// returns a model's world matrix
const DirectX::XMMATRIX & ModelClass::GetWorldMatrix()
{
	return pData_->GetWorldMatrix();
}

const std::string & ModelClass::GetModelType() const
{
	return pData_->GetModelType();
}

const std::string & ModelClass::GetID() const
{
	return pData_->GetID();
}

VERTEX* ModelClass::GetVerticesData()
{
	return pData_->GetVerticesData();
}

UINT* ModelClass::GetIndicesData()
{
	return pData_->GetIndicesData();
}

UINT ModelClass::GetVertexCount() const
{
	return pData_->GetVertexCount();
}

UINT ModelClass::GetIndexCount() const
{
	return pData_->GetIndexCount();
}

VERTEX** ModelClass::GetAddressOfVerticesData()
{
	return pData_->GetAddressOfVerticesData();
}

UINT** ModelClass::GetAddressOfIndicesData()
{
	return pData_->GetAddressOfIndicesData();
}

const DirectX::XMFLOAT3 & ModelClass::GetPosition() const
{
	return pData_->GetPosition();
}

const DirectX::XMFLOAT3 & ModelClass::GetScale() const
{
	return pData_->GetScale();
}

const DirectX::XMFLOAT2 & ModelClass::GetRotation() const
{
	return pData_->GetRotation();
}

const DirectX::XMFLOAT4 & ModelClass::GetColor() const
{
	return pData_->GetColor();
}

// Get the path to the directory with the default models
std::string ModelClass::GetPathToDefaultModelsDir() const
{
	return defaultModelsDirPath_;
}








void ModelClass::SetID(const std::string& modelID)
{
	pData_->SetID(modelID);
	return;
}

void ModelClass::SetModelType(const std::string& modelFilename)
{
	pData_->SetModelType(modelFilename);
	return;
}

void ModelClass::SetVertexCount(UINT vertexCount)
{
	pData_->SetVertexCount(vertexCount);
	return;
}

void ModelClass::SetIndexCount(UINT indexCount)
{
	pData_->SetIndexCount(indexCount);
	return;
}


// modificators of the model
void ModelClass::SetPosition(float x, float y, float z)
{
	pData_->SetPosition(x, y, z);
}

void ModelClass::SetScale(float x, float y, float z)
{
	pData_->SetScale(x, y, z);
}

void ModelClass::SetRotation(float angleX, float angleY)
{
	pData_->SetRotation(angleX, angleY);
}

void ModelClass::SetColor(float red, float green, float blue, float alpha)
{
	pData_->SetColor(red, green, blue, alpha);
}




void ModelClass::AllocateVerticesAndIndicesArrays(UINT vertexCount, UINT indexCount)
{
	assert(vertexCount > 0);
	assert(indexCount > 0);

	pData_->AllocateVerticesAndIndicesArrays(vertexCount, indexCount);

	return;
}



// memory allocation (we need it because we use DirectX::XM-objects)
void* ModelClass::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error(THIS_FUNC, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

void ModelClass::operator delete(void* p) noexcept
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
	bool result = false;
	std::unique_ptr<ModelLoader> pModelLoader = std::make_unique<ModelLoader>();

	result = pModelLoader->Load(modelName, pData_->GetAddressOfVerticesData(), pData_->GetAddressOfIndicesData());
	COM_ERROR_IF_FALSE(result, "can't load model");

	// set the number of the indices
	SetIndexCount(pModelLoader->GetIndexCount());

	Log::Print(THIS_FUNC, "the model was read in successfully");

	return true;
} /* LoadModel() */




// Initialization of the vertex and index buffers for some model;
// Input params:
//	1. a pointer to the device
//	2. a pointer to the vertices array of the model
//	3. a poitner to the indices array of the model
//	4. a count of the vertices
//	5. a count of the indices
bool ModelClass::InitializeBuffers(ID3D11Device* pDevice, 
	VERTEX* pVerticesData,
	UINT* pIndicesData,
	UINT vertexCount,
	UINT indexCount)
{
	HRESULT hr = S_OK;

	try
	{
		// ----------------------------------------------------------------------- // 
		//               CREATE THE VERTEX AND INDEX BUFFERS                       //
		// ----------------------------------------------------------------------- //

		pVertexBuffer_ = new VertexBuffer<VERTEX>();
		pIndexBuffer_ = new IndexBuffer();

		// load vertex data
		hr = pVertexBuffer_->InitializeDefault(pDevice, pVerticesData, vertexCount);
		COM_ERROR_IF_FAILED(hr, "can't initialize a default vertex buffer for the model");

		// load index data
		hr = pIndexBuffer_->Initialize(pDevice, pIndicesData, indexCount);
		COM_ERROR_IF_FAILED(hr, "can't initialize an index buffer for the model");
	}
	catch (std::bad_alloc & e)
	{
		this->ShutdownBuffers();           // release the vertex/index buffers
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the vertex/index buffer object");
	}

	return true;
} /* InitializeBuffers() */


// release the vertex/index buffers
void ModelClass::ShutdownBuffers()
{
	_DELETE(pVertexBuffer_);
	_DELETE(pIndexBuffer_);

	return;
}


// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void ModelClass::RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	UINT offset = 0;

	// set the vertex buffer as active
	pDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffer_->GetAddressOf(), pVertexBuffer_->GetAddressOfStride(), &offset);

	// set the index buffer as active
	pDeviceContext->IASetIndexBuffer(pIndexBuffer_->Get(), DXGI_FORMAT_R32_UINT, 0);

	// set the type of primitive topology we want to use
	pDeviceContext->IASetPrimitiveTopology(topologyType);

	
	return;
}