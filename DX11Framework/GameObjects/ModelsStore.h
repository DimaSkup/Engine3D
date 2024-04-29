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
		const std::map<aiTextureType, TextureClass*> & textures);

	// create a model using raw vertices/indices data
	const UINT CreateNewModelWithRawData(ID3D11Device* pDevice,
		const std::string & textID,                   // a text identifier for this model
		const std::vector<VERTEX> & verticesArr,
		const std::vector<UINT> & indicesArr,
		const std::map<aiTextureType, TextureClass*> & textures,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification factors
		const DirectX::XMVECTOR & inRotModification); // rotation modification factors

	// create a model using vertex/index buffers
	const UINT CreateNewModelWithBuffers(ID3D11Device* pDevice,
		VertexBuffer<VERTEX> & vertexBuffer,
		IndexBuffer & indexBuffer,
		const std::string & textID,                   // a text identifier for this model
		const std::map<aiTextureType, TextureClass*> & textures,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	const UINT CreateOneCopyOfModelByIndex(ID3D11Device* pDevice, const UINT index);
	const std::vector<UINT> ModelsStore::CreateBunchCopiesOfModelByIndex(const UINT indexOfOrigin, const UINT numOfCopies);

	void ComputeRelationsModelsToChunks(const UINT chunksCount,
		const UINT numOfModels,
		//const std::vector<uint32_t> & modelsIDs,
		const std::vector<DirectX::XMVECTOR> & minChunksDimensions,
		const std::vector<DirectX::XMVECTOR> & maxChunksDimensions,
		_Inout_ std::vector<std::vector<uint32_t>> & outRelationsChunksToModels);

	void FillInDataArrays(const uint32_t index,
		const std::string & textID,                   // a text identifier for this model
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,   // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification);  // rotation modification; if we don't set this param the model won't rotate

	// *****************************************************************************
	//                        Public update API
	// *****************************************************************************
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

	void SetTextureForVB_ByIdx(
		const UINT vb_idx,                           // index of a vertex buffer             
		const std::string & texturePath,             // path to the texture (aka. texture_name)
		const aiTextureType type);                   // type of a texture: diffuse/normal/etc.

	void ModelsStore::SetTextureForVB_ByIdx(
		const UINT vb_idx,                           // index of a vertex buffer             
		TextureClass* pTexture,                      // ptr to a texture object
		const aiTextureType type);                   // type of a texture: diffuse/normal/etc.

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
	const UINT GetIndexOfModelByTextID(const std::string & textID);
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
	const uint32_t GenerateIndex();
	void AddNewRelationsModelsToBuffer(const UINT bufferIdx, const std::vector<uint32_t>& modelIndices);

	uint32_t PushBackEmptyModel()
	{
		const uint32_t index = GenerateIndex();

		IDXs_.push_back(index);
		textIDs_.push_back({ "empty_model" });

		// position/rotation/scale of the model
		positions_.push_back(DirectX::XMVectorZero());
		rotations_.push_back(DirectX::XMVectorZero());
		scales_.push_back({ 1, 1, 1, 1 });  // default scale

		// setup modificators for this model
		positionModificators_.push_back(DirectX::XMVectorZero());  // data for position changing
		rotationQuatModificators_.push_back(DirectX::XMVectorZero());   // data for rotation changing
		scaleModificators_.push_back({ 1, 1, 1, 1 });  // default scale

		worldMatrices_.push_back(DirectX::XMMatrixIdentity());
		texTransform_.push_back(DirectX::XMMatrixIdentity());
		texOffset_.push_back({ 0, 0 });

		// create a default material for this model
		materials_.push_back(Material());

		return index;
	}
	
	void ShiftRightRangeOfModels(
		const UINT shiftFactor,
		const UINT fromIdx,
		const UINT toIdx)
	{

	}

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
	std::vector<uint32_t>                 modelsToUpdate_;               // contains IDs of models which must be updated each frame (its positions/rotations/scale/etc.)
	
	std::vector<std::string>              textIDs_;                      // text ID (name) of the model
	std::vector<DirectX::XMVECTOR>        positions_;                    // current position of model
	std::vector<DirectX::XMVECTOR>        rotations_;                    // current rotation of model
	std::vector<DirectX::XMVECTOR>        scales_;                       // current scale factor of model
	std::vector<DirectX::XMVECTOR>        positionModificators_;         // modificators for models positions
	std::vector<DirectX::XMVECTOR>        rotationQuatModificators_;     // contains an array of rotation quaternions which are used for updating models directions each frame
	std::vector<DirectX::XMVECTOR>        scaleModificators_;            // modificators for models scale factors
	std::vector<DirectX::XMMATRIX>        worldMatrices_;                // world matrix of each model
	std::vector<DirectX::XMMATRIX>        texTransform_;                 // for texture animations: movement, scale, rotation
	std::vector<DirectX::XMFLOAT2>        texOffset_;                    // offset of textures
	std::vector<Material>                 materials_;
	std::vector<UINT>                     relatedToVertexBufferByIdx_;   // [index: model_idx => value: vertex_buffer_idx] (to what vertex buffer is related a model)

	// VERTEX/INDEX BUFFERS RELATED STUFF
	std::vector<std::map<aiTextureType, TextureClass*>> textures_;                     // textures set for each vertex buffer
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

	Waves waves_;
};