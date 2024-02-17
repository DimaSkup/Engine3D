///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      ModelsStore.cpp
// Description:   
//
// Created:       05.07.23
///////////////////////////////////////////////////////////////////////////////////////////////
#include "ModelsStore.h"

#include "ModelInitializer.h"
#include "TextureManagerClass.h"
#include "ModelsModificationHelpers.h"

#include "../Engine/COMException.h"
#include "../Engine/log.h"







///////////////////////////////////////////////////////////////////////////////////////////////
//               Helper structs to store parts of the transient data
///////////////////////////////////////////////////////////////////////////////////////////////

struct Details::ModelsStoreTransientData
{
	// stores one frame transient data;
	// This is intermediate data used by the update pipeline every frame and discarded 
	// at the end of the frame

	std::vector<UINT> modelsToUpdate;

	// UPDATE MODELS POSITIONS/ROTATIONS DATA
	std::vector<DirectX::XMVECTOR> posModificators;             // position changes
	std::vector<DirectX::XMVECTOR> rotModificators;             // rotations changes
	std::vector<DirectX::XMVECTOR> positionsDataToUpdate;
	std::vector<DirectX::XMVECTOR> rotationsDataToUpdate;
	std::vector<DirectX::XMVECTOR> newPositionsData;
	std::vector<DirectX::XMVECTOR> newRotationsData;

	std::vector<DirectX::XMMATRIX> translationMatricesToUpdate;
	std::vector<DirectX::XMMATRIX> rotationMatricesToUpdate;
	std::vector<DirectX::XMMATRIX> worldMatricesToUpdate;       // write into it the finish matrices data which then will be used to update the world matrices of models


	void Clear()
	{
		modelsToUpdate.clear();
		posModificators.clear();
		rotModificators.clear();
		positionsDataToUpdate.clear();
		rotationsDataToUpdate.clear();
		newPositionsData.clear();
		newRotationsData.clear();

		translationMatricesToUpdate.clear();
		rotationMatricesToUpdate.clear();
		worldMatricesToUpdate.clear();
	}
};



///////////////////////////////////////////////////////////////////////////////////////////////
//                            PUBLIC MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////



ModelsStore::ModelsStore()
	: numOfModels_(0)
	, modelsTransientData_(std::make_unique<Details::ModelsStoreTransientData>())
{
}


ModelsStore::~ModelsStore()
{
}

void ComputeChunksDimensions(const UINT chunksCountInRow,
	const UINT chunkWidth,
	std::vector<DirectX::XMFLOAT3> & outMinDimensions,
	std::vector<DirectX::XMFLOAT3> & outMaxDimensions,
	std::vector<DirectX::XMFLOAT4> & outColorsForChunks)
{
	UINT chunksCount = chunksCountInRow * chunksCountInRow;               // square area of chunks
	UINT chunkIndex = 0;

	outMinDimensions.resize(chunksCount);
	outMaxDimensions.resize(chunksCount);
	outColorsForChunks.resize(chunksCount);

	UINT data_idx = 0;

	const float fChunkWidth = static_cast<float>(chunkWidth);
	const float fChunksCountInRow = static_cast<float>(chunksCountInRow);

	for (float w_idx = 0; w_idx < fChunksCountInRow; ++w_idx)
	{
		for (float h_idx = 0; h_idx < fChunksCountInRow; ++h_idx)
		{
			const DirectX::XMFLOAT3 coord1{ (w_idx * fChunkWidth), 0.0f, (h_idx * fChunkWidth) };
			//const DirectX::XMFLOAT2 coord2{ (float)((w_idx + 1) * chunkWidth), (float)(h_idx * chunkWidth) };
			//const DirectX::XMFLOAT2 coord3{ (float)(w_idx * chunkWidth), (float)((h_idx + 1) * chunkWidth) };
			const DirectX::XMFLOAT3 coord4{ (w_idx + 1.0f) * fChunkWidth, fChunkWidth, (h_idx + 1.0f) * fChunkWidth };

			const float red = (float)(rand() % 255) * 0.01f;   // rand(0, 255) / 100.0f
			const float green = (float)(rand() % 255) * 0.01f;
			const float blue = (float)(rand() % 255) * 0.01f;

			outMinDimensions[data_idx] = coord1;
			outMaxDimensions[data_idx] = coord4;
			outColorsForChunks[data_idx] = { red, green, blue, 1.0f };

			++data_idx;
		}
	}
}

const UINT DefineChunkIndexByCoords(
	const DirectX::XMVECTOR & inCoords,
	const std::vector<DirectX::XMFLOAT3> & inMinDimensions,
	const std::vector<DirectX::XMFLOAT3> & inMaxDimensions)
{
	DirectX::XMFLOAT3 coords;
	DirectX::XMStoreFloat3(&coords, inCoords);

	for (UINT data_idx = 0; data_idx < inMinDimensions.size(); data_idx++)
	{
		// define if X and Z of input point is clamped between min and max dimensions values of the chunk coords
		const bool isX_inChunk = (inMinDimensions[data_idx].x <= coords.x) && (coords.x <= inMaxDimensions[data_idx].x);
		const bool isY_inChunk = (inMinDimensions[data_idx].y <= coords.y) && (coords.y <= inMaxDimensions[data_idx].y);
		const bool isZ_inChunk = (inMinDimensions[data_idx].z <= coords.z) && (coords.z <= inMaxDimensions[data_idx].z);

		// if all three coord of the input point are inside the chunk we return an index of this chunk
		if (isX_inChunk && isY_inChunk && isZ_inChunk)
			return data_idx;
	}
}



void ModelsStore::Initialize(Settings & settings)
{
	// load width and height of a single chunk
	const int chunkWidth = settings.GetSettingIntByKey("CHUNK_WIDTH");
	//const int chunkHeight = settings.GetSettingIntByKey("CHUNK_HEIGHT");
	const int renderDepth = settings.GetSettingIntByKey("FAR_Z");       // how far we can see

	// calculate the number of chunks needed to store the models data
	UINT chunksCountInRow = (renderDepth) / (chunkWidth);

	ComputeChunksDimensions(chunksCountInRow,
		chunkWidth,
		minChunksDimensions_,
		maxChunksDimensions_,
		colorsForChunks_);


	return;
}

void ModelsStore::CreateTerrainFromSetupFile(const std::string & terrainSetupFilename)
{
	return;
}




const UINT ModelsStore::CreateModelFromFile(
	ID3D11Device* pDevice,
	const std::string & filePath,          // a path to the data file of this model
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	// this function creates a new model, setups it, load its data from the data file;
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(filePath.size(), "the input filePath is empty");

	const uint32_t index = GenerateIndex();

	try
	{
		// make an initializer object which is used for initialization of this model from file
		ModelInitializer modelInitializer;

		// initialize this model loading its data from the data file by filePath
		modelInitializer.InitializeFromFile(
			pDevice,
			*this,
			filePath);

		// fill in data arrays 
		FillInDataArrays(index,                                 // set that this model has such an index
			(uint32_t)vertexBuffers_.back().GetVertexCount(),   // set the number of vertices of this model
			0.0f,                                               // set speed for the model
			inPosition,                                         // set position for the model
			inDirection,                                        // set rotation for the model
			inPosModification,                                  // position modification value
			inRotModification);                                 // rotation modification value
		
		++numOfModels_;
		assert(numOfModels_ == textures_.size());

	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model by index: " + std::to_string(index));
	}

	return index;
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::CreateModelWithData(ID3D11Device* pDevice,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT>   & indicesArr,
	const std::vector<TextureClass*> & texturesArr,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	// this function creates a new model, setups it with the input data,
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty");
	COM_ERROR_IF_ZERO(indicesArr.size(), "the input indices array is empty");
	COM_ERROR_IF_ZERO(texturesArr.size(), "the input textures array is empty");

	const uint32_t index = GenerateIndex();


	try
	{
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		// fill in data arrays 
		FillInDataArrays(index,                                 // set that this model has such an index
			(uint32_t)vertexBuffers_.back().GetVertexCount(),   // set the number of vertices of this model
			0.0f,                                               // set speed for the model
			inPosition,                                         // set position for the model
			inDirection,                                        // set rotation for the model
			inPosModification,                                  // position modification value
			inRotModification);                                 // rotation modification value

		for (TextureClass * pTexture : texturesArr)
			textures_.push_back(pTexture);

		
		++numOfModels_;
		assert(numOfModels_ == textures_.size());
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}

	return index;
}

///////////////////////////////////////////////////////////

// create a model using vertex/index buffers
const UINT ModelsStore::CreateModelWithData(ID3D11Device* pDevice,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	VertexBuffer<VERTEX> & vertexBuffer,
	IndexBuffer & indexBuffer,
	const std::vector<TextureClass*> & texturesArr,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	// check input data
	assert(vertexBuffer.GetVertexCount() > 0);
	assert(indexBuffer.GetIndexCount() > 0);
	assert(texturesArr.size() > 0);

	const uint32_t index = GenerateIndex();

	try
	{
		vertexBuffers_.push_back(vertexBuffer);
		indexBuffers_.push_back(indexBuffer);

		// create textures for this model
		for (TextureClass* pTexture : texturesArr)
			textures_.push_back(pTexture);

		// fill in data arrays 
		FillInDataArrays(index,                                 // set that this model has such an index
			(uint32_t)vertexBuffers_.back().GetVertexCount(),   // set the number of vertices of this model
			0.0f,                                               // set speed for the model
			inPosition,                                         // set position for the model
			inDirection,                                        // set rotation for the model
			inPosModification,                                  // position modification value
			inRotModification);                                 // rotation modification value


		++numOfModels_;

		assert(numOfModels_ == textures_.size());
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}

	// return the index of the created model
	return index;
}






///////////////////////////////////////////////////////////////////////////////////////////////
//                                    PUBLIC UPDATE API
///////////////////////////////////////////////////////////////////////////////////////////////

void SelectModelsToUpdate(
	const ModelsStore & inStore,
	const UINT inNumOfModels,
	std::vector<UINT> & outModelsToUpdate)
{
	for (UINT idx = 0; idx < inNumOfModels; ++idx)
		outModelsToUpdate.push_back(idx);
}

void ModelsStore::UpdateModels(const float deltaTime)
{
	const UINT numOfModels = numOfModels_;
	assert(numOfModels > 0);

	// select models which will be updated for this frame
	SelectModelsToUpdate(*this, numOfModels, modelsTransientData_->modelsToUpdate);

	// define the number of models which will be updated
	const UINT numOfModelsToUpdate = (UINT)modelsTransientData_->modelsToUpdate.size();

	// -----------------------------------------------------------------------------------//
	// UPDATE POSITIONS/ROTATIONS OF THE MODELS

	// extract position/rotation modification data of the models which will be updated
	PrepareModificationVectors(modelsTransientData_->modelsToUpdate, positionsModificators_, modelsTransientData_->posModificators);
	PrepareModificationVectors(modelsTransientData_->modelsToUpdate, rotationModificators_, modelsTransientData_->rotModificators);

	// extract position/rotation data of the models which will be updated
	PreparePositionsToUpdate(modelsTransientData_->modelsToUpdate, positions_, modelsTransientData_->positionsDataToUpdate);
	PrepareRotationsToUpdate(modelsTransientData_->modelsToUpdate, rotations_, modelsTransientData_->rotationsDataToUpdate);

	// compute new positions for the models to update 
	ComputePositions(
		numOfModelsToUpdate,
		deltaTime,
		modelsTransientData_->positionsDataToUpdate, 
		modelsTransientData_->posModificators, 
		modelsTransientData_->newPositionsData);

	// compute new rotations for the models to update 
	ComputeRotations(
		numOfModelsToUpdate,
		deltaTime,
		modelsTransientData_->rotationsDataToUpdate, 
		modelsTransientData_->rotModificators, 
		modelsTransientData_->newRotationsData);

	// apply new positions data to the models
	ApplyPositions(
		modelsTransientData_->modelsToUpdate, 
		modelsTransientData_->newPositionsData, 
		positions_);

	// apply new rotations data to the models
	ApplyRotations(
		modelsTransientData_->modelsToUpdate, 
		modelsTransientData_->newRotationsData, 
		rotations_);

	// -----------------------------------------------------------------------------------//
	// UPDATE WORLD MATRICES OF THE MODELS

	// compute new translation/rotation matrices according to the updated positions/rotations
	PrepareTranslationMatrices(numOfModelsToUpdate, 
		modelsTransientData_->newPositionsData, 
		modelsTransientData_->translationMatricesToUpdate);

	PrepareRotationMatrices(numOfModelsToUpdate,
		modelsTransientData_->newRotationsData, 
		modelsTransientData_->rotationMatricesToUpdate);

	// compute new world matrices for the models to update
	ComputeWorldMatricesToUpdate(
		numOfModelsToUpdate,
		modelsTransientData_->translationMatricesToUpdate,
		modelsTransientData_->rotationMatricesToUpdate,
		modelsTransientData_->worldMatricesToUpdate);

	// apply new world matrices to the models
	ApplyWorldMatrices(
		modelsTransientData_->modelsToUpdate, 
		modelsTransientData_->worldMatricesToUpdate, 
		worldMatrices_);

	// -----------------------------------------------------------------------------------//

	// clear the transient data since we already don't need it
	modelsTransientData_->Clear();

	return;
}

///////////////////////////////////////////////////////////

void ModelsStore::SetTextureByIndex(const UINT index, 
	const std::string & texturePath, 
	aiTextureType type)
{
	// Create a new texture from the file ans set it 
	// into the textures array by particular index

	// create a new texture from the file or just get a ptr to a texture object by key (its path) if it is already exists 
	TextureClass* pOriginTexture = TextureManagerClass::Get()->GetTextureByKey(texturePath);

	// check if the textures array size is less than the index if so we push this texture
	// at the end of the array;
	if (index >= textures_.size())
		textures_.push_back(pOriginTexture);
	else
		textures_[index] = pOriginTexture;   // set texture by index
}



///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void PrepareTexturesSRV_OfModelsToRender(
	const UINT numOfModels,
	const std::vector<TextureClass*> & textures,
	std::vector<ID3D11ShaderResourceView* const*> & texturesSRVs)
{
	// allocate memory for the proper number of textures shader recource views
	//texturesSRVs.resize(numOfModels);

	assert(textures.size() == numOfModels);
	assert(texturesSRVs.size() == 0);

	for (UINT idx = 0; idx < numOfModels; ++idx)
		texturesSRVs.push_back(textures[idx]->GetTextureResourceViewAddress());
}

///////////////////////////////////////////////////////////

void PrepareVertexBuffersDataToRender(
	const UINT numOfModels,
	const std::vector<VertexBuffer<VERTEX>> & vertexBuffers,
	std::vector<ID3D11Buffer*> & outVertexBuffersPtrs,
	std::vector<UINT> & outVertexBuffersStrides)
{
	assert(vertexBuffers.size() == numOfModels);

	// allocate memory for the proper number of vertex buffers adresses and strides
	outVertexBuffersPtrs.resize(numOfModels);
	outVertexBuffersStrides.resize(numOfModels);

	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		const VertexBufferStorage::VertexBufferData & vertexBuffData = vertexBuffers[idx].GetData();
		outVertexBuffersPtrs[idx] = vertexBuffData.pBuffer_;
		outVertexBuffersStrides[idx] = vertexBuffData.stride_;
	}
	
}

///////////////////////////////////////////////////////////

void PrepareIndexBuffersDataToRender(
	const UINT numOfModels,
	const std::vector<IndexBuffer> & indexBuffers,
	std::vector<ID3D11Buffer*> & outIndexBuffersPtrs,
	std::vector<UINT> & outIndexCounts)
{
	assert(indexBuffers.size() == numOfModels);

	// // allocate memory for the proper number of index buffers addresses and index counts
	outIndexBuffersPtrs.resize(numOfModels);
	outIndexCounts.resize(numOfModels);

	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		const IndexBufferStorage::IndexBufferData & indexBuffData = indexBuffers[idx].GetData();

		outIndexBuffersPtrs[idx] = indexBuffData.pBuffer_;
		outIndexCounts[idx] = indexBuffData.indexCount_;
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::RenderModels(ID3D11DeviceContext* pDeviceContext,
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	LightShaderClass & lightShader,
	PointLightShaderClass & pointLightShader,
	const LightStore & lightsStore,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMFLOAT3 & cameraPos)
{

	const UINT numOfModels = numOfModels_;
	const DirectX::XMFLOAT3 fogColor{ 0.5f, 0.5f, 0.5f };

	// -----------------------------------------------------------------------------------//

	//std::vector<DirectX::XMMATRIX> worldMatrices;
	std::vector<ID3D11ShaderResourceView* const*> texturesSRVs;
	std::vector<ID3D11Buffer*> vertexBuffersPtrs;
	std::vector<ID3D11Buffer*> indexBuffersPtrs;
	std::vector<UINT> vertexBuffersStrides;
	std::vector<UINT> indexCounts;

	//PrepareWorldMatricesOfModelsToRender(numOfModels, positions_, rotations_, worldMatrices);
	PrepareTexturesSRV_OfModelsToRender(numOfModels, textures_, texturesSRVs);
	PrepareVertexBuffersDataToRender(numOfModels, vertexBuffers_, vertexBuffersPtrs, vertexBuffersStrides);
	PrepareIndexBuffersDataToRender(numOfModels, indexBuffers_, indexBuffersPtrs, indexCounts);



	

#if 0
	// RENDER USING THE COLOR SHADER

	UINT offset = 0;

	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// set a ptr to the vertex buffer and vertex buffer stride
		pDeviceContext->IASetVertexBuffers(0, 1,
			&vertexBuffersPtrs[idx],
			&vertexBuffersStrides[idx],
			&offset);

		// set a ptr to the index buffer
		pDeviceContext->IASetIndexBuffer(indexBuffersPtrs[idx], DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

		const UINT chunk_idx = DefineChunkIndexByCoords(
			positions_[idx],
			minChunksDimensions_,
			maxChunksDimensions_);


		colorShader.Render(pDeviceContext,
			indexCounts[idx],
			worldMatrices_[idx],
			viewProj,
			colorsForChunks_[chunk_idx]);
}

#endif

#if 0
	// RENDER USING THE TEXTURE SHADER

	// render the bunch of models using the texture shader
	textureShader.Render(pDeviceContext,
		worldMatrices_,
		viewProj,
		cameraPos,
		fogColor,
		texturesSRVs,
		vertexBuffersPtrs,
		indexBuffersPtrs,
		vertexBuffersStrides,
		indexCounts,
		numOfModels,
		5.0f,
		100.0f,
		true,   // enable fog
		false);
#endif


#if 0
	UINT offset = 0;

	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// set a ptr to the vertex buffer and vertex buffer stride
		pDeviceContext->IASetVertexBuffers(0, 1,
			&vertexBuffersPtrs[idx],
			&vertexBuffersStrides[idx],
			&offset);

		// set a ptr to the index buffer
		pDeviceContext->IASetIndexBuffer(indexBuffersPtrs[idx], DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);



		lightShader.Render(pDeviceContext,
			indexCounts[idx],
			worldMatrices_[idx],
			viewProj,
			cameraPos,
			fogColor,
			texturesSRVs[idx],
			diffuseLight,
			5.0f,
			100.0f,
			false);
	}
#endif

#if 1
	// RENDER USING THE POINT LIGHT SHADER
		pointLightShader.Render(pDeviceContext,
			lightsStore.pointLightsStore_,
			worldMatrices_,
			viewProj,
			cameraPos,
			fogColor,
			texturesSRVs,
			vertexBuffersPtrs,
			indexBuffersPtrs,
			vertexBuffersStrides,
			indexCounts,
			5.0f,
			100.0f,
			true);
#endif
	

#if 0

	// go through each mesh and render it
	for (Mesh* pMesh : meshes_)
	{
		// prepare a mesh for rendering
		pMesh->Draw(topologyType);

		// set that we want to render this count of the mesh vertices (currently num_vertices == num_indices)
		pDataContainer->indexCount = pMesh->GetIndexCount();

		//pDataContainer->world = pMesh->GetTransformMatrix() * pDataContainer->world;
	

		// 1. go through each texture of the mesh and set its resource view into the data container
		//    so later we can used this SRV for texturing / normal mapping / etc;
		// 2. we insert this texture into the map so each texture has its own key (pairs: ['texture_type' => 'pp_texture_resource_view']) 
		for (const auto & texture : pMesh->GetTexturesArr())
		{
			switch (texture->GetType())
			{
				case aiTextureType::aiTextureType_DIFFUSE:
				{
					pDataContainer->texturesMap.insert_or_assign("diffuse", texture->GetTextureResourceViewAddress());
					break;
				}
				case aiTextureType::aiTextureType_NORMALS:
				{
					pDataContainer->texturesMap.insert_or_assign("normals", texture->GetTextureResourceViewAddress());
					break;
				}
				default:
				{
					COM_ERROR_IF_FALSE(false, "UNKNOWN type of the texture");
				}

			} // switch
		} // for 

	}
	
	// since this model was rendered then increase the counts for this frame
	//pSystemState->renderedModelsCount++;
	//pSystemState->renderedVerticesCount += this->GetVertexCount();
	
	
#endif
	return;
} 





///////////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////
const uint32_t ModelsStore::GenerateIndex()
{
	return (uint32_t)IDs_.size();
}
