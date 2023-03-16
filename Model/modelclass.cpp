/////////////////////////////////////////////////////////////////////
// Filename:        modelclass.cpp
// Description:     an implementation of the ModelClass class
// Last revising:   09.01.23
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass(void)
{
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

		// if we want to convert .obj file model data into the internal model format
		if (executeModelConvertation)
		{
			std::string pathToModelFile { ModelConverterClass::Get()->GetPathToModelDir() + GetModelType() + ".obj"};

			result = ModelConverterClass::Get()->ConvertFromObj(pathToModelFile);
			COM_ERROR_IF_FALSE(result, "can't convert .obj into the internal model format");
		}

		// Load in the model data from a file (internal type)
		result = this->LoadModel(GetModelType());
		COM_ERROR_IF_FALSE(result, "can't load in the model data");

		// after the model data has been loaded we now call the CalculateModelVectors() to
		// calculate the tangent and binormal. It also recalculates the normal vector;
		pModelMath->CalculateModelVectors((void*)GetModelData(), this->GetVertexCount());

		// Initialize the vertex and index buffer that hold the geometry for the model
		result = this->InitializeBuffers(pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize the buffers");


		this->SetID(modelId);

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
		this->SetID(modelId);
		this->SetIndexCount(pModel->GetIndexCount());
		this->SetModelData(pModel->GetModelData(), pModel->GetVertexCount());
		this->SetIndexData(pModel->GetIndicesData(), pModel->GetIndexCount());

		// Initialize the vertex and index buffer that hold the geometry for the model
		result = this->InitializeBuffers(pDevice);
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
	textureArray_.Shutdown();     // release the texture objects
	_DELETE(pMediator_);          // release the model mediator


	return;
}


// initializes a new texture with the input file names provided.
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


// Get the path to the directory with the default models
std::string ModelClass::GetPathToDefaultModelsDir() const
{
	return defaultModelsDirPath_;
}

// returns a pointer to the array of textures
ID3D11ShaderResourceView* const* ModelClass::GetTextureArray() const
{
	return this->textureArray_.GetTextureArray();
}


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
	bool result = false;
	std::unique_ptr<ModelLoader> pModelLoader = std::make_unique<ModelLoader>();

	result = pModelLoader->Load(modelName, GetAddressOfModelData(), GetAddressOfIndicesData());
	COM_ERROR_IF_FALSE(result, "can't load model");

	// set the number of the indices
	SetIndexCount(pModelLoader->GetIndexCount());

	Log::Print(THIS_FUNC, "the model was read in successfully");

	return true;
} /* LoadModel() */




// Initialization of the vertex and index buffers for some model
bool ModelClass::InitializeBuffers(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //

	// load vertex data
	hr = vertexBuffer_.InitializeDefault(pDevice, GetModelData(), GetIndexCount());
	COM_ERROR_IF_FAILED(hr, "can't initialize a default vertex buffer for the model");

	// load index data
	hr = indexBuffer_.Initialize(pDevice, GetIndicesData(), GetIndexCount());
	COM_ERROR_IF_FAILED(hr, "can't initialize an index buffer for the model");

	return true;
} /* InitializeBuffers() */



// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void ModelClass::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	UINT offset = 0;

	// set the vertex buffer as active
	pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), vertexBuffer_.GetAddressOfStride(), &offset);

	// set the index buffer as active
	pDeviceContext->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// set which type of primitive topology we want to use
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	return;
}