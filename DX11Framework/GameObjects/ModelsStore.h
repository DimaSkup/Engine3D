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
#include "../Engine/Settings.h"
#include "../EffectsAndShaders/colorshaderclass.h"
#include "../EffectsAndShaders/textureshaderclass.h"
#include "../EffectsAndShaders/LightShaderClass.h"
#include "../EffectsAndShaders/PointLightShaderClass.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "textureclass.h"


namespace Details
{
	struct ModelsStoreTransientData;
}


//////////////////////////////////
// Class name: Model
//////////////////////////////////
class ModelsStore final
{
public:
	ModelsStore();
	~ModelsStore();

	
	////////////////////////////   Public modification API   ////////////////////////////
	void Initialize(Settings & settings);

	const UINT CreateModelFromFile(ID3D11Device* pDevice,
		const std::string & filePath,           // a path to the data file of this model
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	void CreateModelFromFileHelper(ID3D11Device* pDevice,
		const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT>   & indicesArr,
		const std::vector<TextureClass*> & texturesArr);

	// create a model using raw vertices/indices data
	const UINT CreateNewModelWithData(ID3D11Device* pDevice,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		const std::vector<TextureClass*> & texturesArr,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	// create a model using vertex/index buffers
	const UINT CreateNewModelWithData(ID3D11Device* pDevice,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		VertexBuffer<VERTEX> & vertexBuffer,
		IndexBuffer & indexBuffer,
		const std::vector<TextureClass*> & texturesArr,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	const UINT CreateCopyOfModelByIndex(ID3D11Device* pDevice, const UINT index);

	void CreateTerrainFromSetupFile(const std::string & terrainSetupFilename);

	void FillInDataArrays(const uint32_t index,
		const uint32_t vertexCount,
		const float velocity,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,   // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification);  // rotation modification; if we don't set this param the model won't rotate

	////////////////////////////   Public update API   ////////////////////////////
	void UpdateModels(const float deltaTime);
	void SetTextureByIndex(const UINT index, const std::string & texturePath, aiTextureType type);


	////////////////////////////   Public rendering API   ////////////////////////////
	void RenderModels(ID3D11DeviceContext* pDeviceContext,
		ColorShaderClass & colorShader,
		TextureShaderClass & textureShader,
		LightShaderClass & lightShader,
		PointLightShaderClass & pointLightShader,
		const LightStore & lightsStore,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMFLOAT3 & cameraPos);




private:
	////////////////////////////  Private modification API  ////////////////////////////
	const uint32_t GenerateIndex();

public:
	// store data
	UINT numOfModels_;
	std::vector<uint32_t>             IDs_;
	std::vector<uint32_t>             relatedToVertexBufferByIdx_;
	std::vector<uint32_t>             relatedToIndexBufferByIdx_;
	std::vector<std::string>          textIDs_;                     // text ID (name) of the model
	std::vector<DirectX::XMVECTOR>    positions_;
	std::vector<DirectX::XMVECTOR>    rotations_;
	std::vector<DirectX::XMVECTOR>    positionsModificators_;
	std::vector<DirectX::XMVECTOR>    rotationModificators_;
	std::vector<DirectX::XMMATRIX>    worldMatrices_;
	
	std::vector<float>                velocities_;
	std::vector<VertexBuffer<VERTEX>> vertexBuffers_;
	std::vector<IndexBuffer>          indexBuffers_;
	std::vector<TextureClass*>        textures_;
	std::vector<uint32_t>             vertexCounts_;           // contains counts of vertices of each model
	std::vector<VERTEX>               verticesData_;

	// CHUNK DATA
	std::vector<DirectX::XMFLOAT3> minChunksDimensions_;
	std::vector<DirectX::XMFLOAT3> maxChunksDimensions_;
	std::vector<DirectX::XMFLOAT4> colorsForChunks_;

	// stores one frame transient data. This is intermediate data used by the
	// update pipeline every frame and discarded at the end of the frame
	std::unique_ptr<Details::ModelsStoreTransientData> modelsTransientData_;
};