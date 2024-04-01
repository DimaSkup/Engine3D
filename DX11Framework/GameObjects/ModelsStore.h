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
#include "../Render/frustumclass.h"
#include "../Light/LightHelper.h"

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
	enum RENDERING_SHADERS
	{
		COLOR_SHADER,
		TEXTURE_SHADER,
		LIGHT_SHADER,
		POINT_LIGHT_SHADER
	};

public:
	ModelsStore();
	~ModelsStore();

	
	////////////////////////////   Public modification API   ////////////////////////////
	void Initialize(Settings & settings);

	const UINT CreateModelFromFile(ID3D11Device* pDevice,
		const std::string & filePath,           // a path to the data file of this model
		const std::string & textID,
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
		const std::string & textID,                   // a text identifier for this model
		const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		const std::vector<TextureClass*> & texturesArr,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification factors
		const DirectX::XMVECTOR & inRotModification); // rotation modification factors

	// create a model using vertex/index buffers
	const UINT CreateNewModelWithData(ID3D11Device* pDevice,
		const std::string & textID,                   // a text identifier for this model
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		VertexBuffer<VERTEX> & vertexBuffer,
		IndexBuffer & indexBuffer,
		const std::vector<TextureClass*> & texturesArr,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	const UINT CreateOneCopyOfModelByIndex(ID3D11Device* pDevice, const UINT index);
	const std::vector<UINT> ModelsStore::CreateBunchCopiesOfModelByIndex(const UINT indexOfOrigin, const UINT numOfCopies);

	void ComputeRelationsModelsToChunks(const UINT chunksCount,
		const UINT numOfModels,
		//const std::vector<uint32_t> & modelsIDs,
		const std::vector<DirectX::XMVECTOR> & minChunksDimensions,
		const std::vector<DirectX::XMVECTOR> & maxChunksDimensions,
		std::vector<std::vector<uint32_t>> & outRelationsChunksToModels);

	void FillInDataArrays(const uint32_t index,
		const std::string & textID,                   // a text identifier for this model
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,   // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification);  // rotation modification; if we don't set this param the model won't rotate

	////////////////////////////   Public update API   ////////////////////////////
	void SetModelAsModifiable(const UINT model_idx);

	void SetPosition(const UINT model_idx, const DirectX::XMVECTOR & newPos);
	void SetRotation(const UINT model_idx, const DirectX::XMVECTOR & newRot);
	void SetScale(const UINT model_idx, const DirectX::XMVECTOR & newScale);

	void SetPositionModificator(const UINT model_idx, const DirectX::XMVECTOR & newPosModificator);
	void SetRotationModificator(const UINT model_idx, const DirectX::XMVECTOR & newRotModificator);
	void SetScaleModificator(const UINT model_idx, const DirectX::XMVECTOR & newScaleModificator);

	void SetWorldForModelByIdx(
		const UINT model_idx,
		const DirectX::XMVECTOR & scaleFactors,
		const DirectX::XMVECTOR & rotationOrigin,
		const DirectX::XMVECTOR & rotationQuaternion,
		const DirectX::XMVECTOR & translationFactors);

	void UpdateWorldMatrixForModelByIdx(const UINT model_idx);
	void UpdateWorldMatricesForModelsByIdxs(const std::vector<UINT> & model_idxs);

	void UpdateModels(const float deltaTime);
	void SetTextureByIndex(const UINT index, const std::string & texturePath, aiTextureType type);


	////////////////////////////   Public rendering API   ////////////////////////////
	void RenderModels(ID3D11DeviceContext* pDeviceContext,
		FrustumClass & frustum,
		ColorShaderClass & colorShader,
		TextureShaderClass & textureShader,
		LightShaderClass & lightShader,
		const LightStore & lightsStore,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMFLOAT3 & cameraPos,
		UINT & renderedModelsCount,
		UINT & renderedVerticesCount,
		const float cameraDepth,                  // how far we can see
		const float totalGameTime);               // time passed since the start of the application


	////////////////////////////   Public query API   //////////////////////////// 
	const UINT GetIdxByTextID(const std::string & textID);
	const bool IsModelModifiable(const UINT model_idx);

	inline const std::vector<DirectX::XMVECTOR> & GetChunksCenterPositions() const
	{
		return chunksCenterPositions_;
	}

	const UINT GetRelatedVertexBufferByModelIdx(const uint32_t modelIdx);

#if 0
	inline const UINT GetFirstModelIdxToVertexBufferByIdx(const UINT vertexBufferIdx)
	{
		// return an index of the first related model to this vertex buffer by its index (vertexBufferIdx)
		return 0;
	}
#endif

	inline void SetRenderingShaderForVertexBufferByIdx(const UINT vertexBufferIdx, const ModelsStore::RENDERING_SHADERS renderingShader)
	{
		useShaderForBufferRendering_[vertexBufferIdx] = renderingShader;
	}

	inline void SetPrimitiveTopologyForVertexBufferByIdx(const UINT vertexBufferIdx, const D3D11_PRIMITIVE_TOPOLOGY topologyType)
	{
		usePrimTopologyForBuffer_[vertexBufferIdx] = topologyType;
	}

private:
	////////////////////////////  Private modification API  ////////////////////////////

	const uint32_t GenerateIndex();

	inline void AddNewRelationsModelsToBuffer(const UINT bufferIdx, const std::vector<uint32_t> & modelIndices)
	{
		relatedToVertexBufferByIdx_.insert(relatedToVertexBufferByIdx_.end(), modelIndices.size(), bufferIdx);
	}

public:
	// MODELS RELATED STUFF
	UINT numOfModels_;
	std::vector<uint32_t>                 IDs_;
	std::vector<uint32_t>                 modelsToUpdate_;              // contains IDs of models which must be updated each frame (its positions/rotations/scale/etc.)
	
	std::vector<std::string>              textIDs_;                     // text ID (name) of the model
	std::vector<DirectX::XMVECTOR>        positions_;
	std::vector<DirectX::XMVECTOR>        rotations_;
	std::vector<DirectX::XMVECTOR>        scales_;
	std::vector<DirectX::XMVECTOR>        positionModificators_;
	std::vector<DirectX::XMVECTOR>        rotationQuatModificators_;    // contains an array of rotation quaternions which are used for updating models directions each frame
	std::vector<DirectX::XMVECTOR>        scaleModificators_;
	std::vector<DirectX::XMMATRIX>        worldMatrices_;
	std::vector<Material>                 materials_;
	//std::vector<DirectX::XMMATRIX>        worldModificators_;
	
	std::vector<UINT>                     relatedToVertexBufferByIdx_;   // [index: model_idx => value: vertex_buffer_idx] (to what vertex buffer is related a model)
	std::vector<TextureClass*>            textures_;
	//std::vector<float>                    velocities_;
	//std::vector<std::vector<uint32_t>>    relationsVertexBuffersToModels_;   // each element contains an array of indices to models which are related to the vertex buffer by particular index 
	//std::vector<VERTEX>                   verticesData_;


	// VERTEX/INDEX BUFFERS RELATED STUFF
	std::vector<VertexBuffer<VERTEX>>     vertexBuffers_;
	std::vector<IndexBuffer>              indexBuffers_;
	std::vector<RENDERING_SHADERS>        useShaderForBufferRendering_;  // [index: vertex_buff_idx => value: ModelsStore::RENDERING_SHADERS] (what kind of rendering shader will we use for this vertex buffer)
	std::vector<D3D11_PRIMITIVE_TOPOLOGY> usePrimTopologyForBuffer_;     // [index: vertex_buff_idx => value: primitive_topology] (what kind of primitive topology will we use for this vertex buffer)

	// CHUNKS RELATED STUFF
	UINT chunksCount_ = 0;
	std::vector<std::vector<uint32_t>>    relationsChunksToModels_;  // each element of array is responsible to chunk index and contains indices of related models
	std::vector<DirectX::XMVECTOR>        chunksCenterPositions_;
	std::vector<DirectX::XMVECTOR>        minChunksDimensions_;
	std::vector<DirectX::XMVECTOR>        maxChunksDimensions_;
	std::vector<DirectX::XMFLOAT4>        colorsForChunks_;

	// stores one frame transient data. This is intermediate data used by the
	// update pipeline every frame and discarded at the end of the frame
	std::unique_ptr<Details::ModelsStoreTransientData> modelsTransientData_;
};