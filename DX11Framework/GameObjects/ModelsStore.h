////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      ModelsStore.h
// Description:   
//
// Created:       02.07.23
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <memory>        // for using unique_ptr
#include <vector>
#include <DirectXMath.h>

#include "MeshObject.h"

namespace Mesh
{
	struct MeshData;
	struct MeshStoreTransientData;
}


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class ModelsStore
{
public:
	ModelsStore();
	~ModelsStore();

	
	// Public modification API
	void CreateModel(const uint64_t inID,
		             const std::string & filePath,           // a path to the data file of this model
		             const DirectX::XMVECTOR & inPosition,
		             const DirectX::XMVECTOR & inDirection);

	// Public update API
	void UpdateModels(const float deltaTime);

	// Public rendering API
	void RenderModels();

#if 0
	// init a signle mesh with data and push it at the end of the mehses array
	void InitializeOneMesh(const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		const std::map<std::string, aiTextureType> & texturesPaths,
		const bool isVertexBufferDynamic = false);
#endif


private:
	// store data
	UINT numOfModels_;
	std::vector<uint64_t> IDs_;
	std::vector<DirectX::XMVECTOR> positions_;
	std::vector<DirectX::XMVECTOR> directions_;
	std::vector<float>             velocities_;
	//std::vector<Mesh>    meshes_;        // an array of all the meshes of all the models

	// stores one frame transient data. This is intermediate data used by the
	// update pipeline every frame and discarded at the end of the frame
	std::unique_ptr<Mesh::MeshStoreTransientData> meshesTransientData_;
};