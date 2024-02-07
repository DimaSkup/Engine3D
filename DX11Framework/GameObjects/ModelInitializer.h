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
#include <algorithm>                      // for using std::replace()

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Mesh.h"

#include <DirectXMath.h>


//////////////////////////////////
// Class name: ModelInitializer
//////////////////////////////////
class ModelInitializer final
{
public:
	ModelInitializer();

	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	void InitializeFromFile(
		ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		std::vector<Mesh> & meshesArr,       // an array of meshes which have vertices/indices buffers that will be filled with vertices/indices data                  
		const std::string & filePath);

private:
	void ProcessNode(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		std::vector<Mesh> & meshesArr,
		aiNode* pNode, const aiScene* pScene, 
		const DirectX::XMMATRIX & parentTrasformMatrix,
		const std::string & filePath);

	void ProcessMesh(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		std::vector<Mesh> & meshesArr, 
		aiMesh* pMesh, 
		const aiScene* pScene, 
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & filePath);

	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, 
		aiMaterial* pMaterial,
		const UINT i, 
		const aiTextureType textureType);

	void LoadMaterialTextures(std::vector<TextureClass> & materialTextures, 
		ID3D11Device* pDevice, 
		aiMaterial* pMaterial,
		aiTextureType textureType, 
		const aiScene* pScene,
		const std::string & filePath);

	UINT GetTextureIndex(aiString* pStr);

	void GetVerticesAndIndicesFromMesh(const aiMesh* pMesh, std::vector<VERTEX> & verticesArr, std::vector<UINT> & indicesArr);
	void ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr);

private:
	// a path to directory which contains a data file for the model which are being currently 
	// loaded (is necessary for loading all the related data files: textures, materials, etc.)
	//std::string modelDirPath_{ "" };  
	//std::string filePath_{ "" };
};