////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.h
// Description:   loads a new model from the file of type:
//                .blend, .fbx, .3ds, .obj, etc.
// 
// Created:       05.07.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////

#include <DirectXMath.h>
#include <d3d11.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "MeshHelperTypes.h"
#include <assimp/material.h>



//////////////////////////////////
// Class name: ModelLoader
//////////////////////////////////
class ModelLoader final
{
public:
	ModelLoader() {};

	
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

	void SetMeshName(
		aiMesh* pMesh,
		std::vector<Mesh::MeshData>& meshes);

	void LoadMaterialColors(aiMaterial* pMaterial, Mesh::Material& mat);

	void LoadMaterialTextures(
		ID3D11Device* pDevice,
		aiMaterial* pMaterial,
		const aiScene* pScene,
		const std::string& filePath,
		Mesh::MeshData& meshData);

	void GetVerticesAndIndicesFromMesh(const aiMesh* pMesh, Mesh::MeshData& meshData);

	void ExecuteModelMathCalculations(std::vector<VERTEX>& vertices);
};