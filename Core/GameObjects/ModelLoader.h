////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.h
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <algorithm>                      // for using std::replace()
#include <DirectXMath.h>
#include <d3d11.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "MeshHelperTypes.h"




//////////////////////////////////
// Class name: ModelLoader
//////////////////////////////////
class ModelLoader final
{
public:
	ModelLoader();

	// initialize a new model from the file of type .blend, .fbx, .3ds, .obj, etc.
	void LoadFromFile(
		ID3D11Device* pDevice,
		std::vector<Mesh::MeshData>& rawMeshes,
		const std::string & filePath);

private:
	void ProcessNode(
		ID3D11Device* pDevice,
		std::vector<Mesh::MeshData>& rawMeshes,
		aiNode* pNode, const aiScene* pScene, 
		const DirectX::XMMATRIX & parentTrasformMatrix,
		const std::string & filePath);

	void ProcessMesh(
		ID3D11Device* pDevice, 
		std::vector<Mesh::MeshData>& rawMeshes,
		aiMesh* pMesh, 
		const aiScene* pScene, 
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & filePath);

	void LoadMaterialTextures(
		ID3D11Device* pDevice,
		aiMaterial* pMaterial,
		const aiScene* pScene,
		const aiTextureType& textureType,
		const std::string& filePath,
		std::vector<TextureClass*>& materialTextures);

	void GetVerticesAndIndicesFromMesh(
		const aiMesh* pMesh,
		std::vector<VERTEX> & verticesArr, 
		std::vector<UINT> & indicesArr);

	void ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr);

private:

	void SetDefaultMaterialTexture(
		aiMaterial* pMaterial,
		std::vector<TextureClass*>& materialTextures,
		const aiTextureType textureType);
};