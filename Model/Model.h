////////////////////////////////////////////////////////////////////
// Filename:      Model.h
// Description:   a main abstraction for models
//
// Created:       02.07.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>                                // for using unique_ptr


#include "../Model/ModelMediator.h"              // for using a mediator between models and shaders
#include "../Model/ModelData.h"
#include "../Model/TextureArrayClass.h"          // for using multiple textures for models
#include "../Model/ModelInitializerInterface.h"  // an interface for model initialization
#include "../Model/ModelInitializer.h"           // a concrete implementation of the ModelInitializerInterface
#include "../Model/Vertex.h"
#include "../Model/VertexBuffer.h"               // for using a vertex buffer's functional
#include "../Model/IndexBuffer.h"                // for using an index buffer's functional


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class Model : public GraphicsComponent
{
public:
	Model();
	virtual ~Model();

	virtual bool Initialize(ID3D11Device* pDevice) = 0;
	virtual void Render(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		// initialize a new model which is based on the another model
	virtual bool InitializeCopyOf(Model* pOriginModel,
		ID3D11Device* pDevice, 
		const std::string & modelType);


	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		const std::string & modelFilename,
		const std::string & modelID);

	// initialize a vertex and index buffer with model's data
	virtual bool InitializeBuffers(ID3D11Device* pDevice,
		ModelData* pModelData);

	//
	//  GETTERS
	//
	virtual ModelData* GetModelDataObj() const _NOEXCEPT;
	virtual TextureArrayClass* GetTextureArray() const _NOEXCEPT;

	void AllocateMemoryForElements();   // ATTENTION: each inherited class must call this function within its constructors

private: 

	void RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType);

private: 
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	ModelData*                 pModelData_ = nullptr;        // data object which contains all the model properties
	TextureArrayClass*         pTexturesList_ = nullptr;     // for work with multiple textures
	VertexBuffer<VERTEX>*      pVertexBuffer_ = nullptr;     // for work with a model vertex buffer
	IndexBuffer*               pIndexBuffer_ = nullptr;      // for work with a model index buffer
};