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

//#include "MeshObject.h"
#include "../ShaderClass/textureshaderclass.h"
#include "../ShaderClass/LightShaderClass.h"
#include "../ShaderClass/PointLightShaderClass.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "textureclass.h"


namespace Mesh
{
	struct MeshData;
	struct MeshStoreTransientData;
}


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class ModelsStore final
{
public:
	ModelsStore();
	~ModelsStore();

	
	// Public modification API
	const UINT CreateModel(ID3D11Device* pDevice,
		                   const std::string & filePath,           // a path to the data file of this model
		                   const DirectX::XMVECTOR & inPosition,
		                   const DirectX::XMVECTOR & inDirection);

	// create a model using raw vertices/indices data
	const UINT CreateModelWithData(ID3D11Device* pDevice,
		                           const DirectX::XMVECTOR & inPosition,
		                           const DirectX::XMVECTOR & inDirection,
		                           const std::vector<VERTEX> & verticesArr,
		                           const std::vector<UINT> & indicesArr,
		                           std::vector<TextureClass> & texturesArr);

	// create a model using vertex/index buffers
	const UINT CreateModelWithData(ID3D11Device* pDevice,
		                           const DirectX::XMVECTOR & inPosition,
		                           const DirectX::XMVECTOR & inDirection,
		                           VertexBuffer<VERTEX> & vertexBuffer,
		                           IndexBuffer & indexBuffer,
		                           std::vector<TextureClass> & texturesArr);

	// Public update API
	void UpdateModels(const float deltaTime);
	void SetTextureByIndex(const UINT index, const std::string & texturePath, aiTextureType type);

	// Public rendering API
	void RenderModels(ID3D11DeviceContext* pDeviceContext,
		TextureShaderClass & textureShader,
		LightShaderClass & lightShader,
		PointLightShaderClass & pointLightShader,
		const LightStore & lightsStore,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMFLOAT3 & cameraPos);

#if 0
	// init a signle mesh with data and push it at the end of the mehses array
	void InitializeOneMesh(const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		const std::map<std::string, aiTextureType> & texturesPaths,
		const bool isVertexBufferDynamic = false);
#endif


public:
	// store data
	UINT numOfModels_;
	std::vector<uint64_t>             IDs_;
	std::vector<DirectX::XMVECTOR>    positions_;
	std::vector<DirectX::XMVECTOR>    rotations_;
	std::vector<DirectX::XMVECTOR>    positionsModificators_;
	std::vector<DirectX::XMVECTOR>    rotationModificators_;
	std::vector<DirectX::XMMATRIX>    worldMatrices_;
	
	std::vector<float>                velocities_;
	std::vector<VertexBuffer<VERTEX>> vertexBuffers_;
	std::vector<IndexBuffer>          indexBuffers_;
	std::vector<TextureClass>         textures_;
	//std::vector<MeshObject>        meshes_;        // an array of all the meshes of all the models

	// stores one frame transient data. This is intermediate data used by the
	// update pipeline every frame and discarded at the end of the frame
	std::unique_ptr<Mesh::MeshStoreTransientData> meshesTransientData_;
};