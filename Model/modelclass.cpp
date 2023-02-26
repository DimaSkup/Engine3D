/////////////////////////////////////////////////////////////////////
// Filename:        modelclass.cpp
// Description:     an implementation of the ModelClass class
// Last revising:   09.01.23
/////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass(void)
{
	modelWorldMatrix_ = DirectX::XMMatrixIdentity(); // by default we set the model at the beginning of the world

	// setup the model's default position and scale
	position_ = { 0.0f, 0.0f, 0.0f };
	scale_ = { 1.0f, 1.0f, 1.0f };
	radianAngle_ = { 0.0f, 0.0f };
}

// the copy constructor
ModelClass::ModelClass(const ModelClass& copy) 
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

// The function here handle initializing of the model's vertex and 
// index buffers using some model data and texture
bool ModelClass::Initialize(ID3D11Device* pDevice, 
							const std::string& modelId)
{
	try
	{
		std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); // for calculations of the model's normal vector, binormal, etc.
		bool result = false;
		bool executeModelConvertation = true;

		// if we want to convert .obj file model data into the internal model format
		if (executeModelConvertation)
		{
			std::string pathToModelFile { ModelConverterClass::Get()->GetPathToModelDir() + modelFilename_ + ".obj"};

			result = ModelConverterClass::Get()->ConvertFromObj(pathToModelFile);
			COM_ERROR_IF_FALSE(result, "can't convert .obj into the internal model format");
		}

		// Load in the model data from a file (internal type)
		result = this->LoadModel(modelFilename_);
		COM_ERROR_IF_FALSE(result, "can't load in the model data");

		// after the model data has been loaded we now call the CalculateModelVectors() to
		// calculate the tangent and binormal. It also recalculates the normal vector;
		pModelMath->CalculateModelVectors((void*)pModelData_, this->GetVertexCount());

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
bool ModelClass::Initialize(ModelClass* pModel, ID3D11Device* pDevice, const std::string& modelId)
{
	try
	{
		bool result = false;
		modelID_ = modelId;  // initialize an identifier of the model

		// copy data from the original
		this->vertexCount_ = pModel->vertexCount_;
		this->indexCount_ = pModel->indexCount_;
		this->pModelData_ = pModel->pModelData_;
		this->pIndicesData_ = pModel->pIndicesData_;


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

	pMediator_->Render(pDeviceContext, this);

	return;
}


// Shutting down of the model class, releasing of the memory, etc.
void ModelClass::Shutdown(void)
{
	_DELETE(pModelData_);         // release the model vertices data
	_DELETE(pIndicesData_);       // release the model indices data
	textureArray_.Shutdown();     // release the texture objects
	_DELETE(pMediator_);          // release the model mediator

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


// set what kind of model this object is
void ModelClass::SetModelType(const std::string& modelFilename)
{
	
	modelFilename_ = modelFilename;
	Log::Print(THIS_FUNC, modelFilename_.c_str());
}


// set an identifier of the model
void ModelClass::SetID(const std::string& modelId)
{
	modelID_ = modelId;
}



// Get the path to the directory with the default models
std::string ModelClass::GetPathToDefaultModelsDir() const
{
	return "internal/";
}


// Get the number of vertices
int ModelClass::GetVertexCount(void) const
{
	return vertexCount_;
}

// Get the number of indices
int ModelClass::GetIndexCount(void) const
{
	return indexCount_;
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


// returns an identifier of the model
const std::string & ModelClass::GetID()
{
	return modelID_;
}


// set model's position in the world
void ModelClass::SetPosition(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
	
	return;
}

// set model's scaling
void ModelClass::SetScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
	
	return;
}

// set model's rotation
void ModelClass::SetRotation(float radiansX, float radiansY)
{
	radianAngle_.x = radiansX;
	radianAngle_.y = radiansY;

	return;
}

// set model's color
void ModelClass::SetColor(float red, float green, float blue, float alpha)
{
	color_.x = red;
	color_.y = green;
	color_.z = blue;
	color_.w = alpha;
}


// getters
const DirectX::XMFLOAT3 & ModelClass::GetPosition() const { return position_; }
const DirectX::XMFLOAT3 & ModelClass::GetScale() const    { return scale_; }
const DirectX::XMFLOAT2 & ModelClass::GetRotation() const { return radianAngle_; }
const DirectX::XMFLOAT4 & ModelClass::GetColor() const    { return color_; };






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

	ModelLoader* pModelLoader = new ModelLoader();

	result = pModelLoader->Load(modelName, &pModelData_, &pIndicesData_);
	COM_ERROR_IF_FALSE(result, "can't load model");


	if (true)
	{
		Log::Error(THIS_FUNC, "AFTER MODEL LOADING:");
		for (size_t i = 0; i < indexCount_; i++)
		{
			cout.setf(ios::fixed | ios::showpoint);
			cout << '\t';
			cout << '[' << i << "]: ";
			cout << setprecision(4);
			cout << setw(6) << pModelData_[i].position.x << ' '
				<< setw(6) << pModelData_[i].position.y << ' '
				<< setw(6) << pModelData_[i].position.z << '\t'
				<< setw(6) << pModelData_[i].texture.x << ' '
				<< setw(6) << pModelData_[i].texture.y << endl;
		}
		cout << endl << endl;

		Log::Error(THIS_FUNC, "INDICES: ");
		cout << '\t';
		for (size_t i = 0; i < indexCount_; i++)
			cout << pIndicesData_[i] << ' ';
		cout << endl << endl << endl;
	}


	Log::Print(THIS_FUNC, "the model was read in successfully");
	_DELETE(pModelLoader);

	return true;
} /* LoadModel() */




// Initialization of the vertex and index buffers for some 3D model
bool ModelClass::InitializeBuffers(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	//std::unique_ptr<VERTEX[]> pVertices = std::make_unique<VERTEX[]>(vertexCount_);
	//std::unique_ptr<UINT[]>  pIndices  = std::make_unique<UINT[]>(indexCount_);

	// ----------------------------------------------------------------------- // 
	//             CREATE THE VERTEX AND INDEX BUFFERS                         //
	// ----------------------------------------------------------------------- //





	// load vertex data
	hr = vertexBuffer_.InitializeDefault(pDevice, pModelData_, indexCount_);
	COM_ERROR_IF_FAILED(hr, "can't initialize a default vertex buffer for the model");

	// load index data
	hr = indexBuffer_.Initialize(pDevice, pIndicesData_, indexCount_);
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