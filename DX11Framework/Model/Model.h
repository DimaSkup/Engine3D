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
#include "../Model/ModelInitializerInterface.h"  // an interface for model initialization

#include "../ShaderClass/ModelToShaderMediatorInterface.h"

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
		const std::vector<UINT> & indicesArr,
		std::map<std::string, aiTextureType> texturesPaths,
		const bool isVertexBufferDynamic = false);

	/////////////////////////////  VIRTUAL FUNCTIONS  /////////////////////////////

	virtual bool Initialize(const std::string & filePath, ModelInitializerInterface* pModelInitializer);

	virtual void Render(D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	virtual const std::string & GetModelType() const { return modelType_; }

	/////////////////////////////  COMMON GETTERS  /////////////////////////////

	UINT GetVertexCount() const;
	UINT GetIndexCount() const;
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	Mesh* GetMeshByIndex(const UINT index) const;
	const std::vector<Mesh*> & GetMeshesArray() const;


protected:
	ID3D11Device*              pDevice_ = nullptr;
	ID3D11DeviceContext*       pDeviceContext_ = nullptr;

	UINT sumVertexCount_ = 0;           // the number of vertices of all the meshes from this model

	std::vector<Mesh*>  meshes_;        // an array of all the meshes of this model
	std::string modelType_{ "" };       // a type name of the current model (it can be: "cube", "sphere", etc.)
	std::string directory_{ "" };       // a path to directory which contains a data file for this model
};