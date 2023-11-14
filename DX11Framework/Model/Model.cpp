////////////////////////////////////////////////////////////////////
// Filename:      Model.cpp
// Description:   a main abstraction for models
//
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
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



/////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////


bool Model::Initialize(const std::string & filePath,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// check input params
	//assert(filePath.empty() != true);
	COM_ERROR_IF_FALSE(filePath.empty() == false, "the input filePath is empty");

	try
	{
		if (!pModelInitializer_->InitializeFromFile(pDevice, pModelData_, filePath))
			COM_ERROR_IF_FALSE(false, "can't load a model from file: " + filePath);
		
		

		// initialize meshes of the model
		//meshes_.push_back(Mesh(pDevice,
		//	pDeviceContext,
		//	pModelData_->GetVertices(),
		//	pModelData_->GetIndices()));

		Mesh* pMesh = new Mesh(pDevice, pDeviceContext, pModelData_->GetVertices(), pModelData_->GetIndices());
		meshes_.push_back(pMesh);
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
	COM_ERROR_IF_FALSE((bool)(another.meshes_.size() > 0), "the another model isn't initialized correctly");

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
		this->GetModelDataObj()->SetID(another.GetModelDataObj()->GetID());

		// copy vertex / index count
		this->GetModelDataObj()->SetVertexCount(another.GetModelDataObj()->GetVertexCount());
		this->GetModelDataObj()->SetIndexCount(another.GetModelDataObj()->GetIndexCount());

		// make a relation between the model and some shader which will be used for
		// rendering this model 
		this->SetModelToShaderMediator(another.GetModelToShaderMediator());
		this->SetRenderShaderName(another.GetRenderShaderName());
	}
	catch (COMException & e)
	{
		Log::Error(e);
		COM_ERROR_IF_FALSE(false, "can't copy a model: " + another.GetModelDataObj()->GetID());
	}

	return *this;
}

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
	_DELETE(pModelData_);         // release all the model data (vertices/indices/etc.)

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

void Model::Render(ID3D11DeviceContext* pDeviceContext,
	D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	// Put the vertex buffer data and index buffer data on the video card 
	// to prepare this data for rendering;
	// after that we call the shader rendering function through the model_to_shader mediator;


	// check input params
	//COM_ERROR_IF_FALSE(this->pModelToShaderMediator_ == nullptr, std::string("mediator == nullptr for model: ") + this->GetModelDataObj()->GetID());

	// go through each mesh and render it
	for (Mesh* pMesh : meshes_)
	{
		// prepare a mesh for rendering
		pMesh->Draw(topologyType);

		// render this mesh using a HLSL shader
		this->pModelToShaderMediator_->Render(pDeviceContext, this);
	}
	

	return;
} // end Render




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


void Model::AllocateMemoryForElements()
{
	try
	{
		pModelData_ = new ModelData();                 // allocate memory for a model data object
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
