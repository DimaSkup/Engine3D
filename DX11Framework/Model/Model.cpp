///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      Model.cpp
// Description:   a main abstraction for models;
//                this class has some common functions for work with models;
//
//                also this class is a basic class for other models classes
//                (for instance: for the Cube, Sphere, Triangle classes, etc.)
//
// Created:       05.07.23
///////////////////////////////////////////////////////////////////////////////////////////////
#include "Model.h"


Model::Model()
{
}


Model::Model(const Model & another)
{
	// check if we allocated memory for the current model object
	COM_ERROR_IF_FALSE(this, "this == nullptr");

	// if everything is ok just execute copying using a copying operator
	*this = another;
}


Model::~Model(void)
{
	//std::string debugMsg{ "destroyment of the " + this->GetID() + " model" };
	//Log::Debug(THIS_FUNC, debugMsg.c_str());

	this->Shutdown();
}



///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////


bool Model::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// check input params
	//assert(filePath.empty() != true);
	COM_ERROR_IF_FALSE(filePath.empty() == false, "the input filePath is empty");

	try
	{
		if (!pModelInitializer_->InitializeFromFile(pDevice, meshes_, filePath))
			COM_ERROR_IF_FALSE(false, "can't load a model from file: " + filePath);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize a model");
		return false;
	}

	// make local pointers to the device and device context
	this->pDevice_ = pDevice;
	this->pDeviceContext_ = pDeviceContext;

	return true;

} // end Initialize

///////////////////////////////////////////////////////////

Model & Model::operator=(const Model & another)
{
	// guard self assignment
	if (this == &another)
		return *this;


	// check if we have any meshes in another model
	COM_ERROR_IF_FALSE(another.meshes_.size(), "the another model has no meshes");

	try
	{
		// how many meshes does the origin model have?
		UINT meshesCount = static_cast<UINT>(another.meshes_.size());

		
		for (UINT i = 0; i < meshesCount; i++)
		{
			// allocate memory for a new mesh and copy mesh data
			meshes_.push_back(new Mesh(*another.meshes_[i]));
		}

		// copy model's common data
		this->pDevice_ = another.pDevice_;
		this->pDeviceContext_ = another.pDeviceContext_;

		// make a relation between the model and some shader which will be used for
		// rendering this model 
		this->SetModelToShaderMediator(another.GetModelToShaderMediator());
		this->SetRenderShaderName(another.GetRenderShaderName());
	}
	catch (COMException & e)
	{
		Log::Error(e);
		COM_ERROR_IF_FALSE(false, "can't copy a model");
	}

	return *this;
} // end operator=

///////////////////////////////////////////////////////////

void Model::Shutdown()
{
	// this function releases the memory from the model's elements

	// go through each mesh of the model and delete it
	if (!meshes_.empty())
	{
		for (Mesh* pMesh : meshes_)
		{
			_DELETE(pMesh);
		}
		meshes_.clear();
	}

	_SHUTDOWN(pTexturesList_);    // release the texture objects of this model

	pDevice_ = nullptr;
	pDeviceContext_ = nullptr;
	pModelInitializer_ = nullptr;
}

///////////////////////////////////////////////////////////

void Model::SetModelInitializer(ModelInitializerInterface* pModelInitializer)  _NOEXCEPT
{
	// set initializer which we will use for initialization/copying of models objects
	assert(pModelInitializer != nullptr);
	pModelInitializer_ = pModelInitializer;

	return;
}

///////////////////////////////////////////////////////////

ModelInitializerInterface* Model::GetModelInitializer() const _NOEXCEPT
{
	// get initializer which we will use for initialization/copying of models objects
	return pModelInitializer_;
}

///////////////////////////////////////////////////////////

void Model::Render(D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// Put the vertex buffer data and index buffer data on the video card 
	// to prepare this data for rendering;
	// after that we call the shader rendering function through the model_to_shader mediator;


	// check input params
	//COM_ERROR_IF_FALSE(this->pModelToShaderMediator_ == nullptr, std::string("mediator == nullptr for model: ") + this->GetModelDataObj()->GetID());

	DataContainerForShaders* pDataContainer = this->pModelToShaderMediator_->GetDataContainerForShaders();

	// go through each mesh and render it
	for (Mesh* pMesh : meshes_)
	{
		// prepare a mesh for rendering
		pMesh->Draw(topologyType);

		// set that we want to render this count of the mesh vertices (num_vertices == num_indices)
		pDataContainer->indexCount = pMesh->GetIndexCount();
		pDataContainer->ppTextures = this->GetTextureArrayObj()->GetTextureResourcesArray();


		// render this mesh using a HLSL shader
		this->pModelToShaderMediator_->Render(this->pDeviceContext_, this);
	}
	

	return;
} // end Render

///////////////////////////////////////////////////////////

void Model::InitializeOneMesh(const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT> & indicesArr)
{
	// this function initializes one mesh with vertices/indices data
	// and pushes this mesh into the meshes array of the model

	try
	{
		Mesh* pMesh = new Mesh(this->pDevice_, this->pDeviceContext_,
			verticesArr,
			indicesArr);

		this->meshes_.push_back(pMesh);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't create a mesh obj");
	}

	return;

} // end InitializeOneMesh

///////////////////////////////////////////////////////////

UINT Model::GetVertexCount() const
{
	// returns a sum of all vertices counts of all the meshes

	UINT sumVertexCount = 0;
	
	for (const Mesh* pMesh : this->meshes_)
	{
		sumVertexCount += pMesh->GetVertexCount();
	}

	return sumVertexCount;
}

///////////////////////////////////////////////////////////

UINT Model::GetIndexCount() const
{
	// returns a sum of all indices counts of all the meshes

	UINT sumIndicesCount = 0;

	for (const Mesh* pMesh : this->meshes_)
	{
		sumIndicesCount += pMesh->GetIndexCount();
	}

	return sumIndicesCount;
}





///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////


void Model::AllocateMemoryForElements()
{
	try
	{
		pTexturesList_ = new TextureArrayClass();      // create an empty textures array object									
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some element of the class");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(THIS_FUNC, "can't initialize the mesh");
		COM_ERROR_IF_FALSE(false, "can't initialize the mesh");
	}

	return;
}
