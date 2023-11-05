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


#include "../Engine/Settings.h"
#include "../Model/ModelData.h"
#include "../Model/TextureArrayClass.h"          // for using multiple textures for models
#include "../Model/ModelInitializerInterface.h"  // an interface for model initialization
//#include "../Model/ModelInitializer.h"           // a concrete implementation of the ModelInitializerInterface
#include "../Model/Vertex.h"
#include "../Model/VertexBuffer.h"               // for using a vertex buffer's functional
#include "../Model/IndexBuffer.h"                // for using an index buffer's functional


#include "../ShaderClass/ModelToShaderMediatorInterface.h"
#include "../ShaderClass/DataContainerForShaders.h"


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class Model : public ModelToShaderComponent
{
public:
	Model();
	virtual ~Model();

	void AllocateMemoryForElements();   // ATTENTION: each inherited class must call this function within its constructors
	
	virtual bool Initialize(ID3D11Device* pDevice) = 0;

	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// set/get initializer which we will use for initialization/copying of models objects
	virtual void SetModelInitializer(ModelInitializerInterface* pModelInitializer) _NOEXCEPT;
	virtual ModelInitializerInterface* GetModelInitializer() const _NOEXCEPT;

	// initialize a new model which is based on the another model
	virtual bool InitializeCopyOf(Model* pOriginModel,
		ID3D11Device* pDevice, 
		const std::string & modelType);


	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		const std::string & modelFilename,
		const std::string & modelID);

	// initialize a vertex and index buffer with model's data
	virtual bool InitializeDefaultBuffers(ID3D11Device* pDevice,
		ModelData* pModelData);


	//
	// INLINE GETTERS
	//
	inline virtual ModelData* GetModelDataObj() const _NOEXCEPT
	{
		// returns a pointer to an object which contains the model's data
		return pModelData_;
	}

	inline virtual TextureArrayClass* GetTextureArray() const _NOEXCEPT
	{
		// returns a pointer to the object which represents an array of textures objects
		return pTexturesList_;
	}
	


protected: 

	void RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType);

protected: 
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	ModelData*                 pModelData_ = nullptr;        // data object which contains all the model properties
	TextureArrayClass*         pTexturesList_ = nullptr;     // for work with multiple textures
	VertexBuffer<VERTEX>*      pVertexBuffer_ = nullptr;     // for work with a model vertex buffer
	IndexBuffer*               pIndexBuffer_ = nullptr;      // for work with a model index buffer						
};