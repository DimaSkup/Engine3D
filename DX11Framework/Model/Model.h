////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      Model.h
// Description:   a main abstraction for models
//
// Created:       02.07.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>                                // for using unique_ptr


#include "../Engine/Settings.h"
#include "../Model/TextureArrayClass.h"          // for using multiple textures for models
#include "../Model/ModelInitializerInterface.h"  // an interface for model initialization

#include "../ShaderClass/ModelToShaderMediatorInterface.h"
#include "../ShaderClass/DataContainerForShaders.h"

#include "Mesh.h"


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class Model : public ModelToShaderComponent
{
public:
	Model(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	Model(const Model & another);
	virtual ~Model();

	void Shutdown();
	Model & operator=(const Model & another);

	// init a signle mesh with data and push it at the end of the mehses array
	void InitializeOneMesh(const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr);

	/////////////////////////////  VIRTUAL FUNCTIONS  /////////////////////////////

	virtual bool Initialize(const std::string & filePath);

	virtual void Render(D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	virtual const std::string & GetModelType() const { return modelType_; }

	// set/get initializer which we will use for initialization of models objects
	virtual void SetModelInitializer(ModelInitializerInterface* pModelInitializer) _NOEXCEPT;
	virtual ModelInitializerInterface* GetModelInitializer() const _NOEXCEPT;

	
	/////////////////////////////  COMMON GETTERS  /////////////////////////////

	UINT GetVertexCount() const;
	UINT GetIndexCount() const;
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;


	/////////////////////////////  INLINE GETTERS  /////////////////////////////
	inline virtual TextureArrayClass* GetTextureArrayObj() const _NOEXCEPT
	{
		// returns a pointer to the object which represents an array of textures objects
		return pTexturesList_;
	}

	inline virtual std::vector<Mesh*> & GetMeshesArray()
	{
		// returns an array of pointers to the model's meshes
		return meshes_;
	}


protected:
	ID3D11Device*              pDevice_ = nullptr;
	ID3D11DeviceContext*       pDeviceContext_ = nullptr;
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	TextureArrayClass*         pTexturesList_ = nullptr;     // for work with multiple textures

	std::vector<Mesh*>         meshes_;                      // an array of all the meshes of this model
	std::string modelType_{ "" };                            // a type name of the current model (it can be: "cube", "sphere", etc.)
};