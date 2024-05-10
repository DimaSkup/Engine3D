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

#include "../Render/frustumclass.h"
#include "../Engine/Settings.h"

// rendering shaders
#include "../EffectsAndShaders/colorshaderclass.h"
#include "../EffectsAndShaders/textureshaderclass.h"
#include "../EffectsAndShaders/LightShaderClass.h"

// types of light sources
#include "../Light/LightHelper.h"

#include "Waves.h"
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
		LIGHT_SHADER
	};

public:
	ModelsStore();
	~ModelsStore();

	// *****************************************************************************
	//                         Public modification API
	// *****************************************************************************
	void Initialize(Settings& settings);

	// create a model loading its vertices/indices/texture data/etc. from file
	const uint32_t CreateModelFromFile(ID3D11Device* pDevice,
		const std::string& filePath,                 // a path to the data file of this model
		const std::string& textID);

	// create a model using raw vertices/indices data
	const uint32_t CreateNewModelWithRawData(ID3D11Device* pDevice,
		const std::string& textID,                   // a text identifier for this model
		const std::vector<VERTEX>& verticesArr,
		const std::vector<UINT>& indicesArr,
		const std::map<aiTextureType, TextureClass*>& textures);

	// create a model using vertex/index buffers
	const uint32_t CreateNewModelWithBuffers(ID3D11Device* pDevice,
		VertexBuffer<VERTEX>& vertexBuffer,
		IndexBuffer& indexBuffer,
		const std::string& textID,                   // a text identifier for this model
		const std::map<aiTextureType, TextureClass*>& textures);

	const uint32_t CreateOneCopyOfModelByIndex(ID3D11Device* pDevice, const UINT index);
	const std::vector<uint32_t> CreateBunchCopiesOfModelByIndex(const UINT indexOfOrigin, const UINT numOfCopies);

	void ComputeRelationsModelsToChunks(const UINT chunksCount,
		const UINT numOfModels,
		const std::vector<DirectX::XMVECTOR>& minChunksDimensions,
		const std::vector<DirectX::XMVECTOR>& maxChunksDimensions,
		_Inout_ std::vector<std::vector<uint32_t>>& outRelationsChunksToModels);

	const uint32_t FillInDataArraysForOneModel(const std::string& textID);


	// *****************************************************************************
	//                        Public setters API
	// *****************************************************************************
	void SetAsModifiableModelsByTextID(const std::string & textID);

	void SetPosRotScaleForModelsByIdxs(
		const std::vector<UINT>& modelsIdxs,
		const std::vector<DirectX::XMVECTOR>& inPositions,
		const std::vector<DirectX::XMVECTOR>& inRotations,
		const std::vector<DirectX::XMVECTOR>& inScales);

	void SetPositionsForModelsByIdxs(
		const std::vector<UINT> & models_idxs, 
		const std::vector<DirectX::XMVECTOR> & inPositions);

	void SetRotationsForModelsByIdxs(
		const std::vector<UINT>& models_idxs,
		const std::vector<DirectX::XMVECTOR>& inRotations);

	void SetScalesForModelsByIdxs(
		const std::vector<UINT>& models_idxs,
		const std::vector<DirectX::XMVECTOR>& inScales);

	void SetPositionModificator(const UINT model_idx, const DirectX::XMVECTOR& newPosModificator);
	void SetRotationModificator(const UINT model_idx, const DirectX::XMVECTOR& newRotModificator);
	void SetScaleModificator(const UINT model_idx, const DirectX::XMVECTOR& newScaleModificator);

	void SetWorldForModelByIdx(
		const UINT model_idx,
		const DirectX::XMVECTOR& scaleFactors,
		const DirectX::XMVECTOR& rotationOrigin,
		const DirectX::XMVECTOR& rotationQuaternion,
		const DirectX::XMVECTOR& translationFactors);

	void SetTexturesForVB_ByIdx(
		const UINT vb_idx,                                       // index of a vertex buffer             
		const std::map<aiTextureType, TextureClass*> textures);  // pairs: ['texture_type' => 'ptr_to_texture']

	void SetDefaultParamsForModelByIdx(const UINT model_idx);
	void SetDefaultRenderingParamsForVB(const UINT vb_idx);


	// *****************************************************************************
	//                        Public updating API
	// *****************************************************************************

	void UpdateModels(const float deltaTime);
	void UpdateWorldMatrixForModelByIdx(const UINT model_idx);
	void UpdateWorldMatricesForModelsByIdxs(const std::vector<UINT>& model_idxs);


	// *****************************************************************************
	//                        Public rendering API
	// *****************************************************************************
	void RenderModels(ID3D11DeviceContext* pDeviceContext,
		FrustumClass & frustum,
		ColorShaderClass& colorShader,
		TextureShaderClass& textureShader,
		LightShaderClass & lightShader,
		const std::vector<DirectionalLight> & dirLights,
		const std::vector<PointLight> & pointLights,
		const std::vector<SpotLight> & spotLights,
		const DirectX::XMMATRIX & viewProj,
		const DirectX::XMFLOAT3 & cameraPos,
		UINT & renderedModelsCount,
		UINT & renderedVerticesCount,
		const float cameraDepth,                  // how far we can see
		const float totalGameTime);               // time passed since the start of the application

	// *****************************************************************************
	//                          Public query API
	// *****************************************************************************
	const UINT GetIndexByTextID(const std::string & textID);
	const std::string GetTextIdByIdx(const UINT idx);
	const bool IsModelModifiable(const UINT model_idx);

	const std::vector<DirectX::XMVECTOR> & GetChunksCenterPositions() const;
	const UINT GetRelatedVertexBufferByModelIdx(const uint32_t modelIdx);

	void SetRenderingShaderForVertexBufferByIdx(const UINT vertexBuffer_idx, const ModelsStore::RENDERING_SHADERS renderingShader);
	void SetRenderingShaderForVertexBufferByModelIdx(const UINT model_idx, const ModelsStore::RENDERING_SHADERS renderingShader);
	void SetPrimitiveTopologyForVertexBufferByIdx(const UINT vertexBuffer_idx, const D3D11_PRIMITIVE_TOPOLOGY topologyType);

private:

	// *****************************************************************************
	//                      Private modification API
	// *****************************************************************************

	const UINT CreateModelHelper(ID3D11Device* pDevice,
		const std::vector<VERTEX>& verticesArr,
		const std::vector<UINT>& indicesArr,
		const std::map<aiTextureType, TextureClass*>& textures,
		const std::string& VB_GeometryType);

	const uint32_t GenerateIndex();
	void SetRelationsModelsToBuffer(const UINT bufferIdx, const std::vector<uint32_t>& modelIndices);

	uint32_t PushBackEmptyModels(const UINT modelsCountToPush);
	
	void ShiftRightDataOfModels(
		const UINT shiftFactor,
		const UINT fromIdx);

	

	// *****************************************************************************
	//                      Private rendering API
	// *****************************************************************************
	void PrepareIAStageForRendering(
		ID3D11DeviceContext* pDeviceContext,
		const UINT vb_buffer_idx,                               // index of the vertex buffer
		const VertexBufferStorage::VertexBufferData & vbData,   // vertex buffer data
		const IndexBufferStorage::IndexBufferData & ibData);    // index buffer data

public:
	// MODELS RELATED STUFF
	UINT numOfModels_;
	std::vector<uint32_t>                 IDXs_;                          // INDEX of model
	std::vector<std::string>              modelsToUpdate_;               // contains IDs of models which must be updated each frame (its positions/rotations/scale/etc.)
	
	std::vector<std::string>              textIDs_;                      // text ID (name) of the model
	std::vector<DirectX::XMVECTOR>        positions_;                    // current position of model
	std::vector<DirectX::XMVECTOR>        rotations_;                    // current rotation of model
	std::vector<DirectX::XMVECTOR>        scales_;                       // current scale factor of model
	std::vector<DirectX::XMVECTOR>        positionModificators_;         // modificators for models positions
	std::vector<DirectX::XMVECTOR>        rotationQuatModificators_;     // contains an array of rotation quaternions which are used for updating models directions each frame
	std::vector<DirectX::XMVECTOR>        scaleModificators_;            // modificators for models scale factors
	std::vector<DirectX::XMMATRIX>        worldMatrices_;                // world matrix of each model
	std::vector<DirectX::XMMATRIX>        texTransform_;                 // for texture animations: movement, scale, rotation
	//std::vector<DirectX::XMFLOAT2>        texOffset_;                    // offset of textures
	std::vector<Material>                 materials_;
	std::vector<UINT>                     relationsModelsToVB_;          // [index: model_idx => value: vertex_buffer_idx] (to what vertex buffer is related a model)

	// VERTEX/INDEX BUFFERS RELATED STUFF
	std::vector<VertexBuffer<VERTEX>>     vertexBuffers_;
	std::vector<IndexBuffer>              indexBuffers_;
	std::vector<RENDERING_SHADERS>        useShaderForBufferRendering_;  // [index: vertex_buff_idx => value: ModelsStore::RENDERING_SHADERS] (what kind of rendering shader will we use for this vertex buffer)
	std::vector<D3D11_PRIMITIVE_TOPOLOGY> usePrimTopologyForBuffer_;     // [index: vertex_buff_idx => value: primitive_topology] (what kind of primitive topology will we use for this vertex buffer)
	std::vector<std::map<aiTextureType, TextureClass*>> textures_;       // textures set for each vertex buffer

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

	Waves waves_;
};