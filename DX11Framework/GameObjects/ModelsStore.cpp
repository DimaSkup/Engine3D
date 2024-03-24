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
#include "ModelsRenderingHelpers.h"
#include "ChunkHelper.h"

#include "../Engine/COMException.h"
#include "../Engine/log.h"
#include "../Common/MathHelper.h"

#include <d3dx11effect.h>
#include <algorithm>
#include <functional>
#include <cmath>



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
	std::vector<DirectX::XMMATRIX> scalingMatricesToUpdate;
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
		scalingMatricesToUpdate.clear();
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

///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////

const int DefineChunkIndexByCoords(
	const DirectX::XMVECTOR & inCoords,     // some point
	const std::vector<DirectX::XMFLOAT3> & inMinDimensions,
	const std::vector<DirectX::XMFLOAT3> & inMaxDimensions)
{
	// here we define inside which chunk is placed the input point (inCoords)
	// and return the index of this chunk;

	// convert the input coords from XMVECTOR to XMFLOAT3 for easier using
	DirectX::XMFLOAT3 coords;
	DirectX::XMStoreFloat3(&coords, inCoords);

	for (int data_idx = 0; data_idx < inMinDimensions.size(); data_idx++)
	{
		// define if X, Y, and Z of input point is clamped between min and max dimensions values of the chunk coords
		const bool isX_inChunk = (inMinDimensions[data_idx].x <= coords.x) && (coords.x <= inMaxDimensions[data_idx].x);
		const bool isY_inChunk = (inMinDimensions[data_idx].y <= coords.y) && (coords.y <= inMaxDimensions[data_idx].y);
		const bool isZ_inChunk = (inMinDimensions[data_idx].z <= coords.z) && (coords.z <= inMaxDimensions[data_idx].z);

		// if all three coord of the input point are inside the chunk we return an index of this chunk
		if (isX_inChunk && isY_inChunk && isZ_inChunk)
			return data_idx;
	}

	return -1;
}

///////////////////////////////////////////////////////////

void ModelsStore::ComputeRelationsModelsToChunks(
	const UINT chunksCount,
	const UINT numOfModels,
	//const std::vector<uint32_t> & modelsIDs,
	const std::vector<XMVECTOR> & minChunksDimensions,
	const std::vector<XMVECTOR> & maxChunksDimensions,
	std::vector<std::vector<uint32_t>> & outRelationsChunksToModels)
{
	// THIS FUNCTION computes the relations between chunks and models;
	// some bunch of models can be related to paritcular chunks accodring
	// to positions of these models and position of the chunks;

	Log::Debug(LOG_MACRO, "computation of relations: chunks => models");

	outRelationsChunksToModels.resize(chunksCount);

	// get positions vectors of all models
	const std::vector<DirectX::XMVECTOR> posVectorsArr = positions_;

	// go through each model and check if it is placed inside the chunk by chunk_idx
	// if so we relate a model by such index (idx) to this chunk
	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		const DirectX::XMVECTOR & pos = posVectorsArr[idx];

		for (UINT chunk_idx = 0; chunk_idx < chunksCount; ++chunk_idx)
		{
			// check if pos is greater or equal than min and
			//       if pos is less than max;
			// if so it means that the model is between the minimal and maximal points of the chunk
			if (XMVector3GreaterOrEqual(pos, minChunksDimensions[chunk_idx]) &&
				XMVector3Less(pos, maxChunksDimensions[chunk_idx]))
			{
				// relate this model to the current chunk
				outRelationsChunksToModels[chunk_idx].push_back(idx);
			}
		}
	}

	Log::Debug(LOG_MACRO, "computation of relations: computed!");

	return;

	//Log::Debug(LOG_MACRO, "chunk " + std::to_string(chunk_idx) + " is checked");

}

///////////////////////////////////////////////////////////

void ModelsStore::Initialize(Settings & settings)
{
	Log::Debug(LOG_MACRO, "initialization of the models store");

	// load width and height of a single chunk
	const int chunkWidth = settings.GetSettingIntByKey("CHUNK_DIMENSION");  // chunks width/height/depth
	const int renderDepth = settings.GetSettingIntByKey("FAR_Z");       // how far we can see

	assert(chunkWidth > 0);

	// calculate the number of visible chunks in line (for example: from visible minimal X to maximal X)
	UINT chunksCountInRow = 2 * (renderDepth) / (chunkWidth);
	chunksCountInRow = (chunksCountInRow % 2 == 0) ? chunksCountInRow : chunksCountInRow + 1;   // we must have even number of chunks in row

	// how many chunks we want to create (the number of visible chunks around us)
	chunksCount_ = static_cast<UINT>(pow(chunksCountInRow, 3.0f));

	ChunkHelper chunkHelper;

	// compute chunks minimal/maximal points, and its center positions
	chunkHelper.ComputeChunksDimensions(chunksCount_,
		chunksCountInRow,
		chunkWidth,
		minChunksDimensions_,
		maxChunksDimensions_,
		chunksCenterPositions_);

	// create unique colour for each chunk
	chunkHelper.ComputeChunksColors(chunksCount_, colorsForChunks_);

	// allocate memory for chunksCount relations between chunks and models
	relationsChunksToModels_.resize(chunksCount_);

	return;
}

///////////////////////////////////////////////////////////

void ModelsStore::FillInDataArrays(const uint32_t index,
	const std::string & textID,                   // a text identifier for this model
	//const uint32_t vertexCount,
	//const float velocity,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{


	IDs_.push_back(index);
	textIDs_.push_back(textID);
	positions_.push_back(inPosition);
	rotations_.push_back(inDirection);
	scale_.push_back({ 1, 1, 1, 1 });    // each model has default scale factor

	// compute the world matrix which is based on input position and direction values
	worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(inPosition) * DirectX::XMMatrixRotationRollPitchYawFromVector(inDirection));
	positionsModificators_.push_back(inPosModification);
	rotationModificators_.push_back(inRotModification);

	//velocities_.push_back(0.0f);                                                // speed value
	//vertexCounts_.push_back(vertexCount);  // we will use this value later to show how much vertices were rendered onto the screen

	return;
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::CreateModelFromFile(
	ID3D11Device* pDevice,
	const std::string & filePath,                 // a path to the data file of this model
	const std::string & textID,                   // a text identifier for this model
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	// THIS FUNCTION creates a new model, setups it, load its data from the data file;
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

		// set what kind of geometry does this vertex buffer store
		vertexBuffers_.back().SetGeometryType(textID);

		// fill in data arrays 
		FillInDataArrays(index,                                 // set that this model has such an index
			textID,                                             // set text identifier for this model
			//(uint32_t)vertexBuffers_.back().GetVertexCount(),   // set the number of vertices of this model
			//0.0f,                                               // set speed for the model
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

void ModelsStore::CreateModelFromFileHelper(ID3D11Device* pDevice,
	const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT>   & indicesArr,
	const std::vector<TextureClass*> & texturesArr)
{
	// THIS FUNCTION helps to create a new model from file;
	// call chain: 
	//	1. ModelsStore::CreateModelFromFile() -> 
	//	2. ModelInitializer::InitializeFromFile() ->
	//	3. for each mesh of model from file we call this function (CreateModelFromFileHelper)


	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty");
	COM_ERROR_IF_ZERO(indicesArr.size(), "the input indices array is empty");
	COM_ERROR_IF_ZERO(texturesArr.size(), "the input textures array is empty");

	try
	{
		// create new vertex and index buffers for this model
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, "model_from_file", verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		// set that this model is related to this particular vertex and index buffer
		relatedToVertexBufferByIdx_.push_back((UINT)vertexBuffers_.size() - 1);

		// set default rendering shader type for this buffer
		useShaderForBufferRendering_.push_back(ModelsStore::COLOR_SHADER);

		// set default primitive topology for this model
		usePrimTopologyForBuffer_.push_back(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// set textures for this model
		for (TextureClass* pTexture : texturesArr)
			textures_.push_back(pTexture);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::CreateNewModelWithData(ID3D11Device* pDevice,   
	const std::string & textID,                   // a text identifier for this model
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const std::vector<VERTEX> & verticesArr,      // raw vertices data
	const std::vector<UINT>   & indicesArr,       // raw indices data
	const std::vector<TextureClass*> & texturesArr,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	// THIS FUNCTION creates a new model, setups it with the input data,
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty for model with textID: " + textID);
	COM_ERROR_IF_ZERO(indicesArr.size(), "the input indices array is empty for model with textID: " + textID);
	COM_ERROR_IF_ZERO(texturesArr.size(), "the input textures array is empty for model with textID: " + textID);

	const uint32_t model_idx = GenerateIndex();

	try
	{
		// fill in common data arrays 
		FillInDataArrays(model_idx,                               // set that this model has such an index
			textID,
			//(uint32_t)vertexBuffers_.back().GetVertexCount(),   // set the number of vertices of this model
			//0.0f,                                               // set speed for the model
			inPosition,                                           // set position for the model
			inDirection,                                          // set rotation for the model
			inPosModification,                                    // position modification value
			inRotModification);                                   // rotation modification value

		// create new vertex/index buffer for this new model
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, textID, verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		// set that this model is related to this particular vertex and index buffer
		AddNewRelationsModelsToBuffer((UINT)vertexBuffers_.size() - 1, { model_idx });

		// set default rendering shader type for this buffer
		useShaderForBufferRendering_.push_back(ModelsStore::COLOR_SHADER);

		// set default primitive topology for this model
		usePrimTopologyForBuffer_.push_back(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// set textures for this model
		for (TextureClass* pTexture : texturesArr)
			textures_.push_back(pTexture);

		// increase the number of all models
		++numOfModels_;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}

	return model_idx;
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::CreateNewModelWithData(ID3D11Device* pDevice,
	const std::string & textID,                   // a text identifier for this model
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	VertexBuffer<VERTEX> & vertexBuffer,
	IndexBuffer & indexBuffer,
	const std::vector<TextureClass*> & texturesArr,
	const DirectX::XMVECTOR & inPosModification,  // position modification; if we don't set this param the model won't move
	const DirectX::XMVECTOR & inRotModification)  // rotation modification; if we don't set this param the model won't rotate
{
	// THIS FUNCTION creates a new model using vertex/index buffers

	// check input data
	assert(vertexBuffer.GetVertexCount() > 0);
	assert(indexBuffer.GetIndexCount() > 0);
	assert(texturesArr.size() > 0);

	const uint32_t model_idx = GenerateIndex();

	try
	{
		// fill in common data arrays 
		FillInDataArrays(model_idx,                             // set that this model has such an index
			textID,
			inPosition,                                         // set position for the model
			inDirection,                                        // set rotation for the model
			inPosModification,                                  // position modification value
			inRotModification);                                 // rotation modification value

		// add new vertex/index buffer since this model is absolutely new
		vertexBuffers_.push_back(vertexBuffer);
		indexBuffers_.push_back(indexBuffer);

		// set that this model is related to this particular vertex buffer
		AddNewRelationsModelsToBuffer((UINT)vertexBuffers_.size() - 1, { model_idx });

		// set default rendering shader type for this buffer
		useShaderForBufferRendering_.push_back(ModelsStore::COLOR_SHADER);

		// set default primitive topology for this model
		usePrimTopologyForBuffer_.push_back(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// set textures for this model
		for (TextureClass* pTexture : texturesArr)
			textures_.push_back(pTexture);

		// increase the number of all the models
		++numOfModels_;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}

	// return an index (ID) of the created model
	return model_idx;
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::CreateOneCopyOfModelByIndex(ID3D11Device* pDevice,
	const UINT indexOfOrigin)
{
	const uint32_t indexOfCopy = GenerateIndex();
	//const UINT originDataIdx = indexOfOrigin - 1;

	try
	{
		// set that this model is related to the shared vertex and index buffer 
		// (which contain data of the origin model)
		AddNewRelationsModelsToBuffer(GetRelatedVertexBufferByModelIdx(indexOfOrigin), { indexOfCopy });

		// set textures for the copy 
		textures_.push_back(textures_[indexOfOrigin]);

		const DirectX::XMVECTOR & posForCopy = positions_[indexOfOrigin];
		const DirectX::XMVECTOR & rotForCopy = rotations_[indexOfOrigin];

		this->FillInDataArrays(indexOfCopy,
			textIDs_[indexOfOrigin] + "(copy)",
			//vertexCount,
			//0.0f,
			posForCopy, //positions_[indexOfOrigin],              // place this model at the same position as the origin one
			rotForCopy, //rotations_[indexOfOrigin],              // set the same rotation 
			positionsModificators_[indexOfOrigin],  // set the same position modification
			rotationModificators_[indexOfOrigin]);  // set the same rotation modification
			
		++numOfModels_;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't create a copy of the model by index: " + std::to_string(indexOfOrigin));
	}

	return (UINT)indexOfCopy;
}

const std::vector<uint32_t> ModelsStore::CreateBunchCopiesOfModelByIndex(
	const UINT indexOfOrigin,
	const UINT numOfCopies)
{
	// THIS FUNCTION creates a bunch of copies of some model by index;
	// it is faster to make lots of copies at once than making it by one;
	//
	// Input:  index to model which will be basic for others
	// Return: an array of indices to created copies

	assert(indexOfOrigin > 0);
	assert(numOfCopies > 0);

	// basic index
	const uint32_t basicIndex = GenerateIndex();

	// make indices (IDs) for copies
	std::vector<uint32_t> indices;

	for (UINT idx = 0; idx < numOfCopies; ++idx)
	{
		indices.push_back(basicIndex + idx);
	}

	// text id for each copy
	std::string textID{ textIDs_[indexOfOrigin] + "(copy)" };

	// fill in data for these models
	IDs_.insert(IDs_.end(), indices.begin(), indices.end());
	textIDs_.insert(textIDs_.end(), numOfCopies, textID);

	// -------------------------------------------------- //

	// set that this these models are related to the same vertex and index buffer 
	// (which contain data of the origin model)
	AddNewRelationsModelsToBuffer(GetRelatedVertexBufferByModelIdx(indexOfOrigin), indices);


	// get vertices count of the origin model
	//const uint32_t vertexCount = (uint32_t)vertexBuffers_[relatedToIndexBufferByIdx_[indexOfOrigin]].GetVertexCount();

	// set textures for the copy 
	TextureClass* pOriginDiffuseTexture = textures_[indexOfOrigin];
	textures_.insert(textures_.end(), numOfCopies, pOriginDiffuseTexture);
	
	// position/rotation data for these models
	positions_.insert(positions_.end(), numOfCopies, DirectX::XMVectorZero());
	rotations_.insert(rotations_.end(), numOfCopies, DirectX::XMVectorZero());

	// compute the world matrix which is based on input position and direction values
	//worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(inPosition) * DirectX::XMMatrixRotationRollPitchYawFromVector(inDirection));
	worldMatrices_.insert(worldMatrices_.end(), numOfCopies, DirectX::XMMatrixIdentity());
	positionsModificators_.insert(positionsModificators_.end(), numOfCopies, DirectX::XMVectorZero());
	rotationModificators_.insert(rotationModificators_.end(), numOfCopies, DirectX::XMVectorZero());

	//velocities_.insert(velocities_.end(), numOfCopies, 0.0f);                                                // speed value
	//vertexCounts_.insert(vertexCounts_.end(), numOfCopies, vertexCount);  // we will use this value later to show how much vertices were rendered onto the screen

	numOfModels_ += numOfCopies;

	return indices;
}




///////////////////////////////////////////////////////////////////////////////////////////////
//                                    PUBLIC UPDATE API
///////////////////////////////////////////////////////////////////////////////////////////////

void SelectModelsToUpdate(
	const ModelsStore & inStore,
	const UINT inNumOfModels,
	std::vector<UINT> & outModelsToUpdate)
{
	// here we define what models we want to update for this frame

	for (UINT idx = 0; idx < inNumOfModels; ++idx)
	{
		outModelsToUpdate.push_back(idx);
	}
}

///////////////////////////////////////////////////////////

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

	PrepareScalingMatrices(numOfModelsToUpdate,
		scale_,
		modelsTransientData_->scalingMatricesToUpdate);

	// compute new world matrices for the models to update
	ComputeWorldMatricesToUpdate(
		numOfModelsToUpdate,
		modelsTransientData_->scalingMatricesToUpdate,
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


/////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC RENDERING API
/////////////////////////////////////////////////////////////////////////////////////////////

void ModelsStore::RenderModels(ID3D11DeviceContext* pDeviceContext,
	FrustumClass & frustum,
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	LightShaderClass & lightShader,
	PointLightShaderClass & pointLightShader,
	const LightStore & lightsStore,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMFLOAT3 & cameraPos,
	UINT & renderedModelsCount,
	UINT & renderedVerticesCount,
	const float cameraDepth)
{

	uint32_t modelID = 0;

	//const UINT numOfModels = numOfModels_;
	const DirectX::XMFLOAT3 fogColor{ 0.5f, 0.5f, 0.5f };

	// ------------------------------------------------------- //

	std::vector<UINT> IDsToRender;
	std::vector<DirectX::XMMATRIX> worldMatricesForRendering;
	std::vector<ID3D11ShaderResourceView* const*> texturesSRVs;


	try
	{

		// get all the visible models for rendering
		PrepareIDsOfModelsToRender(
			cameraPos,
			cameraDepth,
			chunksCount_,
			minChunksDimensions_,
			maxChunksDimensions_,
			chunksCenterPositions_,
			relationsChunksToModels_,
			frustum,
			IDsToRender);

		//IDsToRender = IDs_;
		
		// we don't see any model so we don't have anything for rendering
		if (IDsToRender.size() == 0)
			return;
		
		// go through each vertex buffer and render its content 
		for (UINT buffer_idx = 0; buffer_idx < vertexBuffers_.size(); ++buffer_idx)
		{
			

			// contains ids of all the models which are related to the current vertex buffer
			std::vector<uint32_t> modelsIDsRelatedToVertexBuffer;

			// get all the models which are visible now and are related to the current vertex buffer
			for (UINT modelIdx : IDsToRender)
			{
				if (GetRelatedVertexBufferByModelIdx(modelIdx) == buffer_idx)
					modelsIDsRelatedToVertexBuffer.push_back(modelIdx);
			}

			// we don't have any visible model which are related to this vertex buffer for this frame
			if (modelsIDsRelatedToVertexBuffer.size() == 0)
				continue;

			PrepareWorldMatricesToRender(modelsIDsRelatedToVertexBuffer, worldMatrices_, worldMatricesForRendering);
		

			// --------------------- PREPARE BUFFERS DATA --------------------- //

			// current vertex buffer's data
			const VertexBufferStorage::VertexBufferData & vertexBuffData = vertexBuffers_[buffer_idx].GetData();
			ID3D11Buffer* vertexBufferPtr = vertexBuffData.pBuffer_;
			const UINT vertexBufferStride = vertexBuffData.stride_;
			

			// current index buffer's data
			const IndexBufferStorage::IndexBufferData & indexBuffData = indexBuffers_[buffer_idx].GetData();
			ID3D11Buffer* indexBufferPtr = indexBuffData.pBuffer_;
			const UINT indexCount = indexBuffData.indexCount_;

			// set what primitive topology we want to use to render this vertex buffer
			pDeviceContext->IASetPrimitiveTopology(usePrimTopologyForBuffer_[buffer_idx]);


			
			// define what shader we will use to render the vertex buffer by idx
			switch (useShaderForBufferRendering_[buffer_idx])
			{
				case COLOR_SHADER:
				{
					colorShader.Render(pDeviceContext,
						vertexBufferPtr,
						indexBufferPtr,
						vertexBufferStride,
						indexCount,
						worldMatricesForRendering,
						viewProj);
					
					break;
				}
				case TEXTURE_SHADER:
				{
					// if we want to render textured object we have to get its textures
					PrepareTexturesSRV_OfModelsToRender(modelsIDsRelatedToVertexBuffer, textures_, texturesSRVs);

					// render the bunch of models using the texture shader
					textureShader.Render(pDeviceContext,
						worldMatricesForRendering,
						viewProj,
						cameraPos,
						fogColor,
						texturesSRVs,
						vertexBufferPtr,
						indexBufferPtr,
						vertexBufferStride,
						indexCount,
						5.0f,
						cameraDepth,
						true,   // enable fog
						false);

					break;
				}
				case DIFFUSE_LIGHT_SHADER:
				{
					// if we want to render textured object we have to get its textures
					PrepareTexturesSRV_OfModelsToRender(modelsIDsRelatedToVertexBuffer, textures_, texturesSRVs);

					// render the bunch of models using the diffuse light shader
					lightShader.Render(pDeviceContext,
						lightsStore.diffuseLightsStore_,
						worldMatricesForRendering,
						viewProj,
						cameraPos,
						{ 0.2f, 0.4f, 0.6f},
						texturesSRVs,
						vertexBufferPtr,
						indexBufferPtr,
						vertexBufferStride,
						indexCount,
						5.0f,
						cameraDepth-15.0f,
						true);   // enable fog

					break;
				}
				case POINT_LIGHT_SHADER:
				{
					// if we want to render textured object we have to get its textures
					PrepareTexturesSRV_OfModelsToRender(modelsIDsRelatedToVertexBuffer, textures_, texturesSRVs);

					pointLightShader.Render(pDeviceContext,
						lightsStore.diffuseLightsStore_,
						lightsStore.pointLightsStore_,
						worldMatricesForRendering,
						viewProj,
						cameraPos,
						fogColor,
						texturesSRVs,
						vertexBufferPtr,
						indexBufferPtr,
						vertexBufferStride,
						indexCount,
						5.0f,
						cameraDepth,
						true);

					break;
				}
				default:
				{
					Log::Error(LOG_MACRO, "unknown type of the rendering shader");
				}
			}

			// the number of rendered models for this vertex buffer
			const UINT numOfRenderedModels = (UINT)modelsIDsRelatedToVertexBuffer.size();

			// rendered models count is just a sum of all the rendered models for this frame
			renderedModelsCount += numOfRenderedModels;

			// how many vertices were rendered during rendering of this vertex buffer for different models
			renderedVerticesCount += vertexBuffData.vertexCount_ * numOfRenderedModels;

			// clear the transient data array after rendering of
			// models which are related to this vertex buffer
			worldMatricesForRendering.clear();
			texturesSRVs.clear();
			modelsIDsRelatedToVertexBuffer.clear();
		}
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "out of range of some array during preparing of data for rendering");
	}


	return;
} 





/////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC QUERY API
/////////////////////////////////////////////////////////////////////////////////////////////

const UINT ModelsStore::GetIdxByTextID(const std::string & textID)
{
	// THIS FUNCTION searches for the input textID in the array of models text IDs;
	// if we found such textID we return its index in the array;
	// if we didn't find such textID we throw an std::out_of_range exception

	assert(!textID.empty());

	try
	{
		std::vector<std::string>::iterator it;
		it = std::find(textIDs_.begin(), textIDs_.end(), textID);

		// if we found such textID
		if (it != textIDs_.end())
		{
			// return its index (position in the array)
			return static_cast<int>(std::distance(textIDs_.begin(), it));
		}
		else
		{
			std::string errorMsgStr{ "Can't find index by such text id: " + textID };
			std::wstring errorMsg{ StringHelper::StringToWide(errorMsgStr) };

			MessageBox(0, errorMsg.c_str(), L"search info", 0);

			throw new std::out_of_range{ errorMsgStr };
		}
	}
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't find an index of model by textID: " + textID);
	}
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::GetRelatedVertexBufferByModelIdx(const uint32_t modelIdx)
{
	// THIS FUNCTION returns an index of the vertex buffer which
	// is related to the model by the input modelIdx
	try
	{
		return relatedToVertexBufferByIdx_.at(modelIdx);
	}
	catch (std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't find a related vertex buffer by such model index: " + std::to_string(modelIdx));
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////
const uint32_t ModelsStore::GenerateIndex()
{
	// generate an index for model (usually new model's data is placed 
	// at the end of all the data arrays so we generate an ID for the model as size of the IDs array)
	return (uint32_t)IDs_.size();
}
