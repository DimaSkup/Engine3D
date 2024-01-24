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
		std::vector<Mesh*> & meshesArr,       // an array of meshes which have vertices/indices buffers that will be filled with vertices/indices data                  
		const std::string & modelFilename,
		const std::string & modelDirPath) override;

private:
	void ProcessNode(std::vector<Mesh*> & meshesArr, aiNode* pNode, const aiScene* pScene);
	Mesh* ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, UINT i, aiTextureType textureType);
	void LoadMaterialTextures(std::vector<std::unique_ptr<TextureClass>> & materialTextures, ID3D11Device* pDevice, aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	//bool ConvertModelFromFile(const std::string & modelType, const std::string & modelFilename);
	//bool LoadModelDataFromFile(ModelData* pModelData, const std::string & modelFilename);
	void GetVerticesAndIndicesFromMesh(const aiMesh* pMesh, std::vector<VERTEX> & verticesArr, std::vector<UINT> & indicesArr);
	void ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr);

private:
	ID3D11Device* pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;

	// a path to directory which contains a data file for the model which are being currently 
	// loaded (is necessary for loading all the related data files: textures, materials, etc.)
	std::string modelDirPath_{ "" };  
	std::string filePath_{ "" };
};