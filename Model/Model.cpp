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

Model::~Model(void)
{
	//std::string debugMsg{ "destroyment of the " + this->GetID() + " model" };
	//Log::Debug(THIS_FUNC, debugMsg.c_str());

	_SHUTDOWN(pTexturesList_);    // release the texture objects of this model
	_DELETE(pVertexBuffer_);      // release the vertex/index buffers
	_DELETE(pIndexBuffer_);      
	_DELETE(pModelData_);         // release all the model data (vertices/indices/etc.)

	pModelInitializer_ = nullptr;
}




/////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////



// set initializer which we will use for initialization/copying of models objects
void Model::SetModelInitializer(ModelInitializerInterface* pModelInitializer)  _NOEXCEPT
{
	assert(pModelInitializer != nullptr);

	pModelInitializer_ = pModelInitializer;
	return;
}

// get initializer which we will use for initialization/copying of models objects
ModelInitializerInterface* Model::GetModelInitializer() const _NOEXCEPT
{
	return pModelInitializer_;
}


// initialize a new model which is based on the another model;
//
// NOTE: this function initializes only DEFAULT vertex buffer but NOT DYNAMIC
bool Model::InitializeCopyOf(Model* pOriginModel,
	ID3D11Device* pDevice,
	const std::string & modelType)
{
	assert(pOriginModel != nullptr);
	assert(!modelType.empty());

	// try to initialize the copy of some model
	bool result = pModelInitializer_->InitializeCopyOf(pModelData_, 
		pOriginModel->GetModelDataObj(), 
		pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize a new " + modelType);


	// initialize the vertex and index buffer that hold the geometry for the model
	result = pModelInitializer_->InitializeDefaultBuffers(pDevice,
		pVertexBuffer_,
		pIndexBuffer_,
		pModelData_);
	COM_ERROR_IF_FALSE(result, "can't initialize the buffers");


	// after all we need to set the model's ID
	pModelData_->SetID(modelType);

	return true;
}


bool Model::InitializeFromFile(ID3D11Device* pDevice, 
	const std::string & modelFilename,
	const std::string & modelID)
{
	assert(!modelFilename.empty());
	assert(!modelID.empty());

	try
	{
		bool result = false;

		// load model data from the file
		result = pModelInitializer_->InitializeFromFile(pDevice,
			pModelData_,
			modelFilename);
		COM_ERROR_IF_FALSE(result, "can't load model data from the file: " + modelFilename);


		// initialize the vertex and index buffer that hold the geometry for the model
		result = pModelInitializer_->InitializeDefaultBuffers(pDevice,
			pVertexBuffer_,
			pIndexBuffer_,
			pModelData_);
		COM_ERROR_IF_FALSE(result, "can't initialize the buffers");


		// after all we need to set the model's ID
		pModelData_->SetID(modelID);

		return true;
	}
	catch (COMException & e)
	{
		Log::Error(e);
		return false;
	}
}


// initialize a vertex and index buffer with model's data
bool Model::InitializeDefaultBuffers(ID3D11Device* pDevice, ModelData* pModelData)
{
	assert(pModelData != nullptr);
	assert(pModelData->GetVerticesData() != nullptr);  // check if we have any vertices data
	assert(pModelData->GetIndicesData() != nullptr);   // check if we have any indices data

	// initialize the vertex and index buffer that hold the geometry for the model
	bool result = pModelInitializer_->InitializeDefaultBuffers(pDevice,
		pVertexBuffer_,
		pIndexBuffer_,
		pModelData);
	COM_ERROR_IF_FALSE(result, "can't initialize the buffers");

	return true;
}


// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void Model::Render(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType)
{
	this->RenderBuffers(pDeviceContext, topologyType);  // prepare buffers for rendering
	
	return;
}








/////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////


void Model::AllocateMemoryForElements()
{
	try
	{
		pModelData_ = new ModelData();                 // allocate memory for a model data object
		pTexturesList_ = new TextureArrayClass();      // create an empty textures array object
		pVertexBuffer_ = new VertexBuffer<VERTEX>();
		pIndexBuffer_ = new IndexBuffer();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for some element of the class");
	}

	return;
}


// This function prepares the vertex and index buffers for rendering
// sets up of the input assembler (IA) state
void Model::RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType)
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

