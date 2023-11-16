////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializer.h
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "ModelInitializerInterface.h"

#include <algorithm>                      // for using std::replace()

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


//////////////////////////////////
// Class name: ModelInitializer
//////////////////////////////////
class ModelInitializer final : public ModelInitializerInterface
{
public:
	ModelInitializer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		std::vector<Mesh*> & meshesArr,
		ModelData* pModelData,                             
		const std::string & modelFilename) override;

	virtual bool InitializeMesh(Mesh** ppMesh,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<VERTEX> & verticesData,
		const std::vector<UINT> & indicesData) override;

private:
	void ProcessNode(std::vector<Mesh*> & meshesArr, aiNode* pNode, const aiScene* pScene);
	Mesh* ProcessMesh(aiMesh* pMesh, const aiScene* pScene);

	bool ConvertModelFromFile(const std::string & modelType, const std::string & modelFilename);
	bool LoadModelDataFromFile(ModelData* pModelData, const std::string & modelFilename);
	void ExecuteModelMathCalculations(ModelData* pModelData);

private:
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
};