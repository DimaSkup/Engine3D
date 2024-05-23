////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      EntityStore.h
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
#include <set>

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
class EntityStore final
{
public:


public:
	EntityStore();
	~EntityStore();

	
	const std::vector<uint32_t> CreateCopiesOfModelByIndex(const UINT indexOfOrigin, const UINT numOfCopies);


	// *****************************************************************************
	//                        Public setters API
	// *****************************************************************************
	void SetAsModifiableModelsByTextID(const std::string & textID);

	void SetPosRotScaleForModelsByIdxs(
		const std::vector<UINT>& modelsIdxs,
		const std::vector<DirectX::XMVECTOR>& inPositions,
		const std::vector<DirectX::XMVECTOR>& inRotations,
		const std::vector<DirectX::XMVECTOR>& inScales);

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

	void SetRenderingShaderForVertexBufferByIdx(const UINT vertexBuffer_idx, const EntityStore::RENDERING_SHADERS renderingShader);
	void SetRenderingShaderForVertexBufferByModelIdx(const UINT model_idx, const EntityStore::RENDERING_SHADERS renderingShader);
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

	std::vector<DirectX::XMMATRIX>        texTransform_;                 // for texture animations: movement, scale, rotation
	std::vector<Material>                 materials_;
	std::vector<UINT>                     relationsModelsToVB_;          // [index: model_idx => value: vertex_buffer_idx] (to what vertex buffer is related a model)

	// VERTEX/INDEX BUFFERS RELATED STUFF
	
	// CHUNKS RELATED STUFF
#if 0
	UINT chunksCount_ = 0;
	std::vector<std::vector<uint32_t>>    relationsChunksToModels_;  // each element of array is responsible to chunk index and contains indices of related models
	std::vector<DirectX::XMVECTOR>        chunksCenterPositions_;
	std::vector<DirectX::XMVECTOR>        minChunksDimensions_;
	std::vector<DirectX::XMVECTOR>        maxChunksDimensions_;
	std::vector<DirectX::XMFLOAT4>        colorsForChunks_;
#endif
	// stores one frame transient data. This is intermediate data used by the
	// update pipeline every frame and discarded at the end of the frame
	std::unique_ptr<Details::ModelsStoreTransientData> modelsTransientData_;

	Waves waves_;
};