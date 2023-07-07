////////////////////////////////////////////////////////////////////
// Filename:      Model.cpp
// Description:   a main abstraction for models
//
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
#include "Model.h"

Model::Model(void)
{
	try
	{
		pModelInitializer_ = new ModelInitializer();
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
}


Model::~Model(void)
{
	//std::string debugMsg{ "destroyment of the " + this->GetID() + " model" };
	//Log::Debug(THIS_FUNC, debugMsg.c_str());
	_DELETE(pModelInitializer_);  // release the model initializer
	_SHUTDOWN(pTexturesList_);    // release the texture objects 
	_DELETE(pMediator_);          // release the model mediator
	_DELETE(pVertexBuffer_);      // release the vertex/index buffers
	_DELETE(pIndexBuffer_);      
	_DELETE(pModelData_);         // release all the model data
}




/////////////////////////////////////////////////////////////////////
//
//                      PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////


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
	result = pModelInitializer_->InitializeBuffers(pDevice,
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
		result = pModelInitializer_->InitializeBuffers(pDevice,
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


// Put the vertex buffer data and index buffer data on the video card 
// to prepare this data for rendering
void Model::Render(ID3D11DeviceContext* pDeviceContext)
{
	this->RenderBuffers(pDeviceContext);  // prepare buffers for rendering
	pMediator_->Render(pDeviceContext);   // render buffers using a shader

	return;
}



//
//  GETTERS
//

// returns a pointer to an object which contains the model's data
ModelData* Model::GetModelDataObj() const _NOEXCEPT
{
	return pModelData_;
}

// returns a pointer to the object which represents an array of textures objects
const TextureArrayClass const* Model::GetTextureArray() const _NOEXCEPT
{
	return pTexturesList_;
}




/////////////////////////////////////////////////////////////////////
//
//                      PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////


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

