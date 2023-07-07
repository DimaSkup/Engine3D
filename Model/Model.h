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

	virtual bool Initialize(ID3D11Device* pDevice) { return false; };
	virtual void Render(ID3D11DeviceContext* pDeviceContext);


	// initialize a new model which is based on the another model
	bool InitializeCopyOf(Model* pOriginModel,
		ID3D11Device* pDevice, 
		const std::string & modelType);


	bool InitializeFromFile(ID3D11Device* pDevice, 
		const std::string & modelFilename,
		const std::string & modelID);

	

	//
	//  GETTERS
	//
	ModelData* GetModelDataObj() const _NOEXCEPT;
	TextureArrayClass const* GetTextureArray() const _NOEXCEPT;

private: 
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

private: 
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	ModelData*                 pModelData_ = nullptr;        // data object which contains all the model properties
	TextureArrayClass*         pTexturesList_ = nullptr;     // for work with multiple textures
	VertexBuffer<VERTEX>*      pVertexBuffer_ = nullptr;     // for work with a model vertex buffer
	IndexBuffer*               pIndexBuffer_ = nullptr;      // for work with a model index buffer
};