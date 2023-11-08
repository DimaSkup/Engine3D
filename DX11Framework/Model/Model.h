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

#include "../ShaderClass/ModelToShaderMediatorInterface.h"
#include "../ShaderClass/DataContainerForShaders.h"

#include "Mesh.h"


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class Model : public ModelToShaderComponent
{
public:
	Model();
	Model(const Model & another);
	virtual ~Model();

	void AllocateMemoryForElements();   // ATTENTION: each inherited class must call this function within its constructors
	
	virtual bool Initialize(const std::string & filePath, 
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);

	virtual void Render(ID3D11DeviceContext* pDeviceContext,
		D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// set/get initializer which we will use for initialization of models objects
	virtual void SetModelInitializer(ModelInitializerInterface* pModelInitializer) _NOEXCEPT;
	virtual ModelInitializerInterface* GetModelInitializer() const _NOEXCEPT;

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

	inline virtual std::vector<Mesh> & GetMeshesArray()
	{
		return meshes_;
	}

protected: 
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;

	std::vector<Mesh>          meshes_;
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	ModelData*                 pModelData_ = nullptr;        // data object which contains all the model properties
	TextureArrayClass*         pTexturesList_ = nullptr;     // for work with multiple textures
};