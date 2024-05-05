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

#include <algorithm>
#include <functional>
#include <cmath>
#include <iterator>
#include <string>


using namespace DirectX;

// ************************************************************************************
//               Helper structs to store parts of the transient data
// ************************************************************************************

struct Details::ModelsStoreTransientData
{
	// stores one frame transient data;
	// This is intermediate data used by the update pipeline every frame and discarded 
	// at the end of the frame

	std::vector<UINT> modelsToUpdate;

	// UPDATE MODELS POSITIONS/ROTATIONS DATA
	std::vector<DirectX::XMVECTOR> posModificators;             // position changes
	std::vector<DirectX::XMVECTOR> quatRotationModificators;    // rotations changes
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
		quatRotationModificators.clear();
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




// ************************************************************************************
//                             PUBLIC MODIFICATION API
// ************************************************************************************

#pragma region ModelsModificationAPI
ModelsStore::ModelsStore()
	: numOfModels_(0)
	, modelsTransientData_(std::make_unique<Details::ModelsStoreTransientData>())
{
}


ModelsStore::~ModelsStore()
{
}

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
	_Inout_ std::vector<std::vector<uint32_t>> & outRelationsChunksToModels)
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

			//const DirectX::XMVECTOR & min = minChunksDimensions[chunk_idx];
			//const DirectX::XMVECTOR & max = maxChunksDimensions[chunk_idx];

			//if (XMVector3GreaterOrEqual(pos, min) &&
			//	XMVector3Less(pos, max))

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

	// calculate the number of visible chunks in line (for example: from visible minimal X to maximal X);
	// multiply by 2 because we must have chunks in both sides from the camera position (or the origin position)
	UINT chunksCountInRow = 2 * (renderDepth) / (chunkWidth);
	chunksCountInRow = (chunksCountInRow % 2 == 0) ? chunksCountInRow : chunksCountInRow + 1;   // we must have even number of chunks in row

	// how many chunks we want to create (the number of visible chunks around us)
	chunksCount_ = static_cast<UINT>(pow(chunksCountInRow, 2.0f));

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

// ************************************************************************************

const uint32_t ModelsStore::FillInDataArraysForOneModel(
	const std::string& textID)                            // a text identifier for this model
{
	std::string ID(textID);

	// index of model's data
	uint32_t idx = 0;

	// if there is such a textID so we have to modify the input one before storing
	while (std::binary_search(textIDs_.begin(), textIDs_.end(), ID))
	{
		static UINT num = 2;
		ID += "_" + std::to_string(num);
		++num;
	}
	
	// search an element right before the input one
	const auto it = std::upper_bound(textIDs_.begin(), textIDs_.end(), ID);


	// -----------------------------------
	// DEFINE AN INDEX OF THIS NEW MODEL
	// -----------------------------------

	// we add a new element at the end of data arrays
	if (it == textIDs_.end())
	{
		// add one empty model
		idx = PushBackEmptyModels(1);
	}

	// we add a new element somewhere between of records in the data arrays so
	// we have to shirt right all the elements which will be after this new one
	else
	{
		// add one empty model
		PushBackEmptyModels(1);
		
		// get an index before which we will put out new data
		const auto iter = std::upper_bound(textIDs_.begin(), textIDs_.end(), ID);
		idx = static_cast<uint32_t>(std::distance(textIDs_.begin(), iter));

		ShiftRightDataOfModels(1, idx, static_cast<UINT>(IDXs_.size()-1));
	}


	// -----------------------------------
	// SETUP SOME DATA FOR THIS NEW MODEL
	// -----------------------------------

	textIDs_[idx] = ID;

	// impossible (theoretically) index of vertex buffer so it it won't be set later
	// there will be an error
	relatedToVertexBufferByIdx_[idx] = 100000;


	return idx;
}

///////////////////////////////////////////////////////////

const uint32_t ModelsStore::CreateModelFromFile(
	ID3D11Device* pDevice,
	const std::string & filePath,                 // a path to the data file of this model
	const std::string & textID)                   // a text identifier for this model
{
	// THIS FUNCTION creates a new model, setups it, load its data from the data file;
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_FALSE(!filePath.empty(), "the input filePath is empty");

	uint32_t model_idx = 0;

	try
	{
		ModelInitializer modelInitializer;

		// initialize this model loading its data from the data file by filePath;
		// (loads vertices/indices/textures/etc.)
		modelInitializer.InitializeFromFile(
			pDevice,
			*this,
			filePath);

	
		// fill in data arrays for the model by textID
		model_idx = FillInDataArraysForOneModel(textID);

		// set what kind of geometry this vertex buffer stores
		

		// set that this model is related to a vertex buffer by index (vb_idx)
		const UINT vb_idx = static_cast<UINT>(vertexBuffers_.size() - 1);

		vertexBuffers_[vb_idx].SetGeometryType(textID);
		SetRelationsModelsToBuffer(vb_idx, { model_idx });
		
		++numOfModels_;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model by path: " + filePath + "; and text id: " + textID);
	}

	return model_idx;
}

///////////////////////////////////////////////////////////

const uint32_t ModelsStore::CreateNewModelWithRawData(ID3D11Device* pDevice,
	const std::string & textID,                               // a text identifier for this model
	const std::vector<VERTEX> & verticesArr,                  // raw vertices data
	const std::vector<UINT>   & indicesArr,                   // raw indices data
	const std::map<aiTextureType, TextureClass*> & textures)
{
	// THIS FUNCTION creates a new model, setups it with the input data,
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty for model with textID: " + textID);
	COM_ERROR_IF_ZERO(indicesArr.size(), "the input indices array is empty for model with textID: " + textID);
	COM_ERROR_IF_FALSE(!textures.empty(), "the input textures array is empty for model with textID: " + textID);

	// an index of a new model
	uint32_t model_idx = 0;

	try
	{
		// fill in common data arrays for the model by textID
		model_idx = FillInDataArraysForOneModel(textID);          

		// create and initialize vertex and index buffers, set textures for this model;
		// and get an index of the created vertex buffer
		const UINT vb_idx = CreateModelHelper(
			pDevice,
			verticesArr,                                          // raw vertices data
			indicesArr,                                           // raw indices data
			textures,                                             // map of pairs: ['texture_type' => 'ptr_to_texture']
			textID);                                              

		// set that this model is related to a vertex buffer by index (vb_idx)
		SetRelationsModelsToBuffer(vb_idx, { model_idx });

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

const uint32_t ModelsStore::CreateNewModelWithBuffers(ID3D11Device* pDevice,
	VertexBuffer<VERTEX> & vertexBuffer,
	IndexBuffer & indexBuffer,
	const std::string & textID,                                   // a text identifier for this model
	const std::map<aiTextureType, TextureClass*> & textures)
{
	// THIS FUNCTION creates a new model using input vertex/index buffers

	// check input data
	assert(vertexBuffer.GetVertexCount() > 0);
	assert(indexBuffer.GetIndexCount() > 0);
	assert(textures.empty() == false);

	// an index of a new model
	uint32_t model_idx = 0;

	try
	{
		// fill in common data arrays for the model by textID
		model_idx = FillInDataArraysForOneModel(textID);

		// add new vertex/index buffer since this model is absolutely new
		vertexBuffers_.push_back(vertexBuffer);
		indexBuffers_.push_back(indexBuffer);

		// set that this model is related to a vertex buffer by index (vb_idx)
		const UINT vb_idx = static_cast<UINT>(std::distance(vertexBuffers_.begin(), vertexBuffers_.end()));
		SetRelationsModelsToBuffer(vb_idx, { model_idx });

		// set rendering shader / primitive topology / etc. for this new VB
		SetDefaultRenderingParamsForVB(vb_idx);

		// set related textures to the last added VB
		textures_.push_back(textures);

		// increase the number of all the models
		++numOfModels_;
	}

	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model; it's text id: " + textID);
	}

	// return an index (ID) of the created model
	return model_idx;
}

///////////////////////////////////////////////////////////

const uint32_t ModelsStore::CreateOneCopyOfModelByIndex(
	ID3D11Device* pDevice,
	const UINT indexOfOrigin)
{
	// an index of a copied model
	uint32_t model_idx = 0;

	try
	{
		// fill in data arrays for this copied model
		model_idx = FillInDataArraysForOneModel(textIDs_[indexOfOrigin] + "(copy)");

		// set that this model is related to the shared vertex and index buffer 
		// (which contain data of the origin model)
		SetRelationsModelsToBuffer(GetRelatedVertexBufferByModelIdx(indexOfOrigin), { model_idx });
			
		++numOfModels_;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't create a copy of the model by index: " + std::to_string(indexOfOrigin));
	}

	return model_idx;
}

///////////////////////////////////////////////////////////

const std::vector<uint32_t> ModelsStore::CreateBunchCopiesOfModelByIndex(
	const UINT indexOfOrigin,
	const UINT numOfCopies)
{
	// THIS FUNCTION creates a bunch of copies of some model by index;
	// it is faster to make lots of copies at once than making it by one;
	//
	// Input:  index to model which will be basic for others
	// Return: an array of indices to created copies

	assert(indexOfOrigin >= 0);
	assert(numOfCopies > 0);

	const UINT idx_from = indexOfOrigin + 1;
	const UINT idx_to = indexOfOrigin + numOfCopies;

	// allocate additional memory for copies which will be created
	PushBackEmptyModels(numOfCopies);
	ShiftRightDataOfModels(numOfCopies, idx_from, idx_to);
	
	const std::string originTextID{ textIDs_[indexOfOrigin] };
	
	// data for copied models
	const uint32_t basicIndex = GenerateIndex();  // basic index of copied models 

	std::vector<uint32_t> copiedModelsIndices(numOfCopies);
	std::vector<std::string> copiesTextIDs(numOfCopies);  // text id for each copy

	const std::vector<DirectX::XMVECTOR> modelPos (numOfCopies, positions_[indexOfOrigin]);
	const std::vector<DirectX::XMVECTOR> modelRot (numOfCopies, rotations_[indexOfOrigin]);
	const std::vector<DirectX::XMVECTOR> modelScale (numOfCopies, scales_[indexOfOrigin]);
	const std::vector<DirectX::XMVECTOR> modelPosModif (numOfCopies, positionModificators_[indexOfOrigin]);
	const std::vector<DirectX::XMVECTOR> modelQuatRotModif (numOfCopies, rotationQuatModificators_[indexOfOrigin]);
	const std::vector<DirectX::XMVECTOR> modelScaleModif (numOfCopies, scaleModificators_[indexOfOrigin]);

	const std::vector<DirectX::XMMATRIX> modelWorldMatrix (numOfCopies, worldMatrices_[indexOfOrigin]);
	const std::vector<DirectX::XMMATRIX> modelTexTransform(numOfCopies, texTransform_[indexOfOrigin]);
	const std::vector<DirectX::XMFLOAT2> modelTexOffset(numOfCopies, texOffset_[indexOfOrigin]);

	const std::vector<Material> materialOfOrigin(numOfCopies, materials_[indexOfOrigin]);
	const UINT vertexBufferIdx = GetRelatedVertexBufferByModelIdx(indexOfOrigin);


	// --------------------------------------
	//       PREPARE DATA FOR COPIES
	// --------------------------------------

	for (UINT i = 0; i < copiedModelsIndices.size(); ++i)
	{
		copiedModelsIndices[i] = idx_from + i;
	}

	for (UINT idx = 0; idx < numOfCopies; ++idx)
		copiesTextIDs[idx] = originTextID + "_" + std::to_string(idx);

	// set or not that the copied models must be updated each frame
	if (IsModelModifiable(indexOfOrigin))
		modelsToUpdate_.insert(modelsToUpdate_.end(), copiedModelsIndices.begin(), copiedModelsIndices.end());


	// --------------------------------------
	//    fill in data for copies models
	// --------------------------------------

	std::copy(copiesTextIDs.begin(), copiesTextIDs.end(), textIDs_.begin() + idx_from);
	std::copy(modelPos.begin(), modelPos.end(), positions_.begin() + idx_from);
	std::copy(modelRot.begin(), modelRot.end(), rotations_.begin() + idx_from);
	std::copy(modelScale.begin(), modelScale.end(), scales_.begin() + idx_from);
	
	// positions/rotation/scale modificators for these models
	std::copy(modelPosModif.begin(), modelPosModif.end(), positionModificators_.begin() + idx_from);
	std::copy(modelQuatRotModif.begin(), modelQuatRotModif.end(), rotationQuatModificators_.begin() + idx_from);
	std::copy(modelScaleModif.begin(), modelScaleModif.end(), scaleModificators_.begin() + idx_from);

	// world matrices for copied models
	std::copy(modelWorldMatrix.begin(), modelWorldMatrix.end(), worldMatrices_.begin() + idx_from);

	// texture transformation / offset
	std::copy(modelTexTransform.begin(), modelTexTransform.end(), texTransform_.begin() + idx_from);
	std::copy(modelTexOffset.begin(), modelTexOffset.end(), texOffset_.begin() + idx_from);
	
	// copy material for each new model
	std::copy(materialOfOrigin.begin(), materialOfOrigin.end(), materials_.begin() + idx_from);


	// ---------------------------------------------------------------------------------

	// set that this these models are related to the same vertex and index buffer 
	// (which contain data of the origin model)
	SetRelationsModelsToBuffer(vertexBufferIdx, copiedModelsIndices);

	// increase the number of all the models
	numOfModels_ += numOfCopies;

	// return an array of indices (IDs) to copied models
	return copiedModelsIndices;
}

#pragma endregion

// ************************************************************************************
//                                PUBLIC UPDATE API
// ************************************************************************************

#pragma region ModelsUpdateAPI

void ModelsStore::SetModelAsModifiable(const UINT model_idx)
{
	// THIS FUNCTION sets that the model by model_idx must be updated each frame;
	// (world matrix of this model will be updated by its position/rotation/etc. modifcators)

	try
	{
		// check if this model isn't already set as modifiable
		const bool isAlreadyModifiable = std::binary_search(modelsToUpdate_.begin(), modelsToUpdate_.end(), model_idx);

		if (!isAlreadyModifiable)
		{
			modelsToUpdate_.push_back(model_idx);  // set this model as modifiable
			std::sort(modelsToUpdate_.begin(), modelsToUpdate_.end()); // sort an array of modifiable models for further binary_searches
		}
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::SetPosRotScaleForModelsByIdxs(
	const std::vector<UINT>& modelsIdxs,
	const std::vector<DirectX::XMVECTOR>& inPositions,
	const std::vector<DirectX::XMVECTOR>& inRotations,
	const std::vector<DirectX::XMVECTOR>& inScales)
{
	// set positions, rotations, and scales values 
	// for models by indices from modelsIdxs array;
	// after all we update world matrices of these models;

	const size_t indicesArrSize = modelsIdxs.size();
	assert(indicesArrSize > 0);
	assert(indicesArrSize == inPositions.size());
	assert(indicesArrSize == inRotations.size());
	assert(indicesArrSize == inScales.size());

	std::vector<DirectX::XMMATRIX> worldMatricesToUpdate(indicesArrSize);

	// set positions
	for (size_t i = 0; i < indicesArrSize; ++i)
		positions_[modelsIdxs[i]] = inPositions[i];

	// set rotations
	for (size_t i = 0; i < indicesArrSize; ++i)
		rotations_[modelsIdxs[i]] = inRotations[i];

	// set scales
	for (size_t i = 0; i < indicesArrSize; ++i)
		scales_[modelsIdxs[i]] = inScales[i];

	// compute new world matrices for each model 
	for (size_t i = 0; i < indicesArrSize; ++i)
	{
		worldMatricesToUpdate[i] = DirectX::XMMatrixAffineTransformation(
			inScales[i],
			inPositions[i],
			inRotations[i],
			inPositions[i]);
	}

	// apply new world matrices to each model 
	for (size_t i = 0; i < indicesArrSize; ++i)
		worldMatrices_[modelsIdxs[i]] = worldMatricesToUpdate[i];
}

void ModelsStore::SetPositionsForModelsByIdxs(
	const std::vector<UINT> & models_idxs,
	const std::vector<DirectX::XMVECTOR> & inPositions)
{
	// set new position for model by model_idx

	const UINT indicesCount = models_idxs.size();
	assert(indicesCount == inPositions.size());


	// update positions
	for (UINT i = 0; i < indicesCount; ++i)
		positions_.at(models_idxs[i]) = inPositions[i];

	// update world matrices of these models
	UpdateWorldMatricesForModelsByIdxs(models_idxs);
}

void ModelsStore::SetRotation(const UINT model_idx, const DirectX::XMVECTOR & newRot)
{
	// set new rotation for model by model_idx

	try
	{
		rotations_[model_idx] = newRot;
		UpdateWorldMatrixForModelByIdx(model_idx);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
	}
}

void ModelsStore::SetScale(const UINT model_idx, const DirectX::XMVECTOR & newScale)
{
	// set new scale for model by model_idx

	try
	{
		scales_[model_idx] = newScale;
		UpdateWorldMatrixForModelByIdx(model_idx);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::SetPositionModificator(const UINT model_idx, const DirectX::XMVECTOR & newPosModificator)
{
	// TODO
}

void ModelsStore::SetRotationModificator(const UINT model_idx, const DirectX::XMVECTOR & newQuatRotModificator)  
{
	// set a new rotation quaternion for model by idx;
	// INPUT:
	//  1. index of model
	//  2. new quaternion of rotation for the model

	try
	{
		rotationQuatModificators_[model_idx] = newQuatRotModificator;
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
	}
}

void ModelsStore::SetScaleModificator(const UINT model_idx, const DirectX::XMVECTOR & newScaleModificator)
{

}

///////////////////////////////////////////////////////////

void ModelsStore::SetWorldForModelByIdx(
	const UINT model_idx,
	const DirectX::XMVECTOR & scaleFactors,
	const DirectX::XMVECTOR & rotationOrigin,
	const DirectX::XMVECTOR & rotationQuaternion,
	const DirectX::XMVECTOR & translationFactors)
{
	// THIS FUNCTION computes a new world matrix for model by model_idx using input params;

	try
	{
		worldMatrices_[model_idx] = DirectX::XMMatrixAffineTransformation(
			scaleFactors,
			rotationOrigin,
			rotationQuaternion,
			translationFactors);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::UpdateWorldMatrixForModelByIdx(const UINT model_idx)
{
	// compute new world matrix for model by model_idx by its position,rotation,scale,etc.

	try
	{
		// compute world matrix
		worldMatrices_[model_idx] = DirectX::XMMatrixAffineTransformation(
			scales_[model_idx],     // scale factors
			positions_[model_idx],  // center of rotation
			rotations_[model_idx],  // rotation factors
			positions_[model_idx]); // translation offsets
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::UpdateWorldMatricesForModelsByIdxs(const std::vector<UINT> & model_idxs)
{
	// compute world matrix for models by model_idxs by its position,rotation,scale,etc.

	try
	{
		std::vector<DirectX::XMVECTOR> positions;
		std::vector<DirectX::XMVECTOR> rotations;
		std::vector<DirectX::XMVECTOR> scales;
		std::vector<DirectX::XMMATRIX> worldMatricesToUpdate;

		// prepare scales data
		for (const UINT idx : model_idxs)
			scales.push_back(scales_[idx]);

		// prepare rotations data
		for (const UINT idx : model_idxs)
			rotations.push_back(rotations_[idx]);

		// prepare positions data
		for (const UINT idx : model_idxs)
			positions.push_back(positions_[idx]);

		// compute new world matrices for each model 
		for (UINT idx = 0; idx < (UINT)model_idxs.size(); ++idx)
			worldMatricesToUpdate.push_back(DirectX::XMMatrixAffineTransformation(scales[idx], positions[idx], rotations[idx], positions[idx]));

		// apply new world matrices to each model from model_idxs
		UINT data_idx = 0;

		for (const UINT idx : model_idxs)
			worldMatrices_[idx] = worldMatricesToUpdate[data_idx++];
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index; " + std::string(e.what()));
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::UpdateModels(const float deltaTime)
{
	static int i = 0;


	const UINT numOfModels = numOfModels_;
	assert(numOfModels > 0);

	// select models which will be updated for this frame
	//SelectModelsToUpdate(*this, numOfModels, modelsTransientData_->modelsToUpdate);
	modelsTransientData_->modelsToUpdate = modelsToUpdate_;

	// define the number of models which will be updated
	const UINT numOfModelsToUpdate = (UINT)modelsTransientData_->modelsToUpdate.size();

	// -----------------------------------------------------------------------------------//
	// UPDATE POSITIONS/ROTATIONS OF THE MODELS

	// extract position/rotation modification data of the models which will be updated
	PrepareModificationVectors(modelsTransientData_->modelsToUpdate, positionModificators_, modelsTransientData_->posModificators);
	PrepareModificationVectors(modelsTransientData_->modelsToUpdate, rotationQuatModificators_, modelsTransientData_->quatRotationModificators);

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
		modelsTransientData_->quatRotationModificators, 
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
		scales_,
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

void ModelsStore::SetTexturesForVB_ByIdx(
	const UINT vb_idx,                                       // index of a vertex buffer             
	const std::map<aiTextureType, TextureClass*> textures)   // pairs: ['texture_type' => 'ptr_to_texture']
{
	//
	// THIS FUNC relates a texture with such type to a vertex buffer by index (vb_idx)
	//

	assert(vb_idx < textures_.size());

	try
	{
		// set textures for a vertex buffer by index 
		for (auto& texture : textures)
		{
			COM_ERROR_IF_NULLPTR(texture.second, "ptr to texture obj == nullptr");
			textures_[vb_idx].insert_or_assign(texture.first, texture.second);
		}
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't set textures for vertex buffer by idx: " + std::to_string(vb_idx));
	}
}

///////////////////////////////////////////////////////////

void ModelsStore::SetDefaultRenderingParamsForVB(const UINT vb_idx)
{
	const size_t size1 = useShaderForBufferRendering_.size();
	const size_t size2 = usePrimTopologyForBuffer_.size();

	assert(size1 == size2);
	assert(vb_idx <= size1);

	// we've created a new VB and pushed it back so its index is +1 after it
	if (vb_idx == size1)
	{
		// set default rendering params for this buffer
		useShaderForBufferRendering_.push_back(ModelsStore::COLOR_SHADER);
		usePrimTopologyForBuffer_.push_back(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		useShaderForBufferRendering_[vb_idx] = ModelsStore::COLOR_SHADER;
		usePrimTopologyForBuffer_[vb_idx] = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

#pragma endregion


// ************************************************************************************
//                                PUBLIC RENDERING API
// ************************************************************************************

#pragma region ModelsRenderingAPI

void ModelsStore::RenderModels(ID3D11DeviceContext* pDeviceContext,
	FrustumClass & frustum,
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	LightShaderClass & lightShader,
	const std::vector<DirectionalLight> & dirLights,
	const std::vector<PointLight> & pointLights,
	const std::vector<SpotLight> & spotLights,
	const DirectX::XMMATRIX & viewProj,
	const DirectX::XMFLOAT3 & cameraPos,
	UINT & renderedModelsCount,
	UINT & renderedVerticesCount,
	const float cameraDepth,                 // how far we can see
	const float totalGameTime)               // time passed since the start of the application
{
	std::vector<UINT> idxsOfVisibleModels;
	std::vector<DirectX::XMMATRIX> worldMatricesForRendering;
	std::map<aiTextureType, ID3D11ShaderResourceView* const*> texturesSRVs;

	// contains ids of all the models which are related to particular vertex buffer
	std::vector<uint32_t> modelsToRender;
	UINT buffer_idx = 0;                     // an index of the vertex buffer

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
			idxsOfVisibleModels);
		
		// we don't see any model so we don't have anything for rendering
		if (idxsOfVisibleModels.size() == 0)
			return;

		// update the lights params for this frame
		lightShader.SetLights(
			pDeviceContext,
			cameraPos,
			dirLights,
			pointLights,
			spotLights);

		
		// go through each vertex buffer and render its content for instancing
		for (buffer_idx = 0; buffer_idx < vertexBuffers_.size(); ++buffer_idx)
		{
			// get all the models which are visible now and are related to the current vertex buffer
			GetRelatedInputModelsToVertexBuffer(
				buffer_idx, 
				idxsOfVisibleModels, 
				relatedToVertexBufferByIdx_,
				modelsToRender);

			// we don't have any model for rendering now
			if (modelsToRender.size() == 0)
				continue;

			// get world matrices for each model which will be rendered
			PrepareWorldMatricesToRender(modelsToRender, worldMatrices_, worldMatricesForRendering);

		
			// PREPARE INPUT ASSEMLER (IA) STAGE BEFORE THE RENDERING PROCESS
			const VertexBufferStorage::VertexBufferData & vbData = vertexBuffers_[buffer_idx].GetData();
			const IndexBufferStorage::IndexBufferData   & ibData = indexBuffers_[buffer_idx].GetData();
			PrepareIAStageForRendering(pDeviceContext, buffer_idx, vbData, ibData);
			

			// RENDER GEOMETRY
			switch (useShaderForBufferRendering_[buffer_idx])
			{
				case RENDERING_SHADERS::COLOR_SHADER:
				{
					// render the geometry from the current vertex buffer
					colorShader.RenderGeometry(
						pDeviceContext,
						worldMatricesForRendering,
						viewProj,
						ibData.indexCount_,
						totalGameTime);

					break;
				}
				case RENDERING_SHADERS::TEXTURE_SHADER:
				{
					// if we want to render textured object we have to get its textures
					PrepareTexturesSRV_ToRender(textures_[buffer_idx], texturesSRVs);

					textureShader.PrepareShaderForRendering(pDeviceContext, cameraPos);

					textureShader.Render(
						pDeviceContext,
						worldMatricesForRendering,
						viewProj,
						texTransform_[modelsToRender[0]],      // textures transformation matrix
						texturesSRVs,
						ibData.indexCount_);

					break;
				}
				case RENDERING_SHADERS::LIGHT_SHADER:
				{
					std::vector<Material> materials(modelsToRender.size());

					for (UINT idx = 0; idx < materials.size(); ++idx)
						materials[idx] = materials_[modelsToRender[idx]];

					// if we want to render textured object we have to get its textures
					PrepareTexturesSRV_ToRender(textures_[buffer_idx], texturesSRVs);

					// render the geometry from the current vertex buffer
					lightShader.RenderGeometry(
						pDeviceContext,
						materials,
						viewProj,
						texTransform_[modelsToRender[0]],      // textures transformation matrix
						worldMatricesForRendering,
						texturesSRVs,
						ibData.indexCount_);

					break;
				}
			}

		

			// --------------------------------------------------------- //

			// the number of rendered models for this vertex buffer
			const UINT numOfRenderedModels = (UINT)modelsToRender.size();

			// compute the current number of rendered models and number of rendered vertices
			renderedModelsCount += numOfRenderedModels;
			renderedVerticesCount += vbData.vertexCount_ * numOfRenderedModels;

			// --------------------------------------------------------- //

			// clear the transient data array after rendering of
			// models which are related to this vertex buffer
			worldMatricesForRendering.clear();
			texturesSRVs.clear();
			modelsToRender.clear();

		} // end for
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false,
			"can't render a geometry with vertex buffer by idx: " + std::to_string(buffer_idx) +
			"; and geometry type: " + vertexBuffers_[buffer_idx].GetData().geometryType_);
	}
	catch (COMException& e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false,
			"can't render a geometry with vertex buffer by idx: " + std::to_string(buffer_idx) +
			"; and geometry type: " + vertexBuffers_[buffer_idx].GetData().geometryType_);
	}


	return;
} 

#pragma endregion


// ************************************************************************************
//                                PUBLIC QUERY API
// ************************************************************************************

#pragma region ModelsQueryAPI

const UINT ModelsStore::GetIndexOfModelByTextID(const std::string & textID)
{
	// THIS FUNCTION searches for the input textID in the array of models text IDs;
	// if we found such textID we return its index in the array;
	// if we didn't find such textID we throw an std::out_of_range exception

	assert(!textID.empty());

	
	std::vector<std::string>::iterator it;
	it = std::find(textIDs_.begin(), textIDs_.end(), textID);

	// if we found such textID
	if (it != textIDs_.end())
	{
		// return its index (position in the array) which is the same as model index
		return static_cast<int>(std::distance(textIDs_.begin(), it));
	}
	else
	{
		COM_ERROR_IF_FALSE(false, "can't find an index of model by text id: " + textID);
	}
}

///////////////////////////////////////////////////////////

const bool ModelsStore::IsModelModifiable(const UINT model_idx)
{
	// define if a model by model_idx must be updated each frame or not;
	//
	// we look at its modification factors (about position, rotation, scale, etc.)
	// and check if there is some values for modification 
	// if not the model isn't modifiable each frame;

	try
	{
		// get modificators of model by model_idx
		const DirectX::XMVECTOR & posModificator = positionModificators_[model_idx];
		const DirectX::XMVECTOR & rotModificator = rotationQuatModificators_[model_idx];
		const DirectX::XMVECTOR & scaleModificator = scaleModificators_[model_idx];
		
		// define if these modificators has some modification for the model
		const bool isPosModifiable = DirectX::XMVector3NotEqual(posModificator, DirectX::XMVectorZero());
		const bool isRotModifiable = DirectX::XMVector3NotEqual(rotModificator, DirectX::XMVectorZero());
		const bool isScaleModifiable = DirectX::XMVector3NotEqual(scaleModificator, { 1, 1, 1 });
		
		// return if model is modifiable or not
		return (isPosModifiable || isRotModifiable || isScaleModifiable);
	}
	catch (const std::out_of_range & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "there is no model by such index: " + std::to_string(model_idx));
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

///////////////////////////////////////////////////////////

const std::vector<DirectX::XMVECTOR> & ModelsStore::GetChunksCenterPositions() const
{
	return chunksCenterPositions_;
}

void ModelsStore::SetRenderingShaderForVertexBufferByIdx(const UINT vertexBuffer_idx, const ModelsStore::RENDERING_SHADERS renderingShader)
{
	// set a new rendering shader for the vertex buffer by vertexBuffer_idx
	assert(vertexBuffers_.size() > vertexBuffer_idx);
	useShaderForBufferRendering_[vertexBuffer_idx] = renderingShader;
}

void ModelsStore::SetRenderingShaderForVertexBufferByModelIdx(const UINT model_idx, const ModelsStore::RENDERING_SHADERS renderingShader)
{
	// set a new rendering shader for the vertex buffer which is 
	// related to the model by model_idx

	assert(numOfModels_ > model_idx);
	const UINT relatedVertexBuffer_idx = GetRelatedVertexBufferByModelIdx(model_idx);
	useShaderForBufferRendering_[relatedVertexBuffer_idx] = renderingShader;
}

void ModelsStore::SetPrimitiveTopologyForVertexBufferByIdx(const UINT vertexBuffer_idx, const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	// set a primitive topology for the vertex buffer by vertexBuffer_idx
	usePrimTopologyForBuffer_[vertexBuffer_idx] = topologyType;
}

#pragma endregion


// *********************************************************************************
//                           PRIVATE MODIFICATION API
// *********************************************************************************

const UINT ModelsStore::CreateModelHelper(ID3D11Device* pDevice,
	const std::vector<VERTEX>& verticesArr,
	const std::vector<UINT>& indicesArr,
	const std::map<aiTextureType, TextureClass*>& textures,
	const std::string & VB_GeometryType)
{
	// THIS FUNCTION helps to create a new model;
	// it creates and initializes vertex and index buffers, setups textures,
	// and does some configuration for rendering of this model

	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty");
	COM_ERROR_IF_ZERO(indicesArr.size(), "the input indices array is empty");
	COM_ERROR_IF_FALSE(!textures.empty(), "the input textures array is empty");

	try
	{
		// create and init vertex and index buffers for new model
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, "model_from_file", verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		// get index of the last added vertex buffer (VB)
		const UINT vb_idx = static_cast<UINT>(vertexBuffers_.size() - 1);

		// set rendering shader / primitive topology / etc. for this new VB
		SetDefaultRenderingParamsForVB(vb_idx);

		// set what kind of geometry the vertex buffer stores
		vertexBuffers_[vb_idx].SetGeometryType(VB_GeometryType);

		// set related textures to the last added VB
		textures_.push_back(textures);

		return vb_idx;
	}

	catch (COMException& e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}
}

///////////////////////////////////////////////////////////

const uint32_t ModelsStore::GenerateIndex()
{
	// generate an index for model (usually new model's data is placed 
	// at the end of all the data arrays so we generate an ID for the model as size of the IDs array)
	return (uint32_t)IDXs_.size();
}

void ModelsStore::SetRelationsModelsToBuffer(const UINT bufferIdx, const std::vector<uint32_t>& modelIndices)
{
	assert(bufferIdx < vertexBuffers_.size());

	for (const uint32_t model_idx : modelIndices)
	{
		relatedToVertexBufferByIdx_[model_idx] = bufferIdx;
	}
}

uint32_t ModelsStore::PushBackEmptyModels(const UINT modelsCountToPush)
{
	// push back particular count of empty models at the end 
	// of all the models data arrays

	assert(modelsCountToPush > 0);

	uint32_t index = GenerateIndex();

	for (UINT i = 0; i < modelsCountToPush; ++i)
	{
		IDXs_.push_back(index + i);
	}
	
	textIDs_.insert(textIDs_.end(), modelsCountToPush, { "empty_model" });

	// position/rotation/scale of the model
	positions_.insert(positions_.end(), modelsCountToPush, DirectX::XMVectorZero());
	rotations_.insert(rotations_.end(), modelsCountToPush, DirectX::XMVectorZero());
	scales_.insert(scales_.end(), modelsCountToPush, { 1, 1, 1, 1 });  // default scale

	// setup modificators for this model
	positionModificators_.insert(positionModificators_.end(), modelsCountToPush, DirectX::XMVectorZero());  // data for position changing
	rotationQuatModificators_.insert(rotationQuatModificators_.end(), modelsCountToPush, DirectX::XMVectorZero());   // data for rotation changing
	scaleModificators_.insert(scaleModificators_.end(), modelsCountToPush, { 1, 1, 1, 1 });  // default scale

	worldMatrices_.insert(worldMatrices_.end(), modelsCountToPush, DirectX::XMMatrixIdentity());
	texTransform_.insert(texTransform_.end(), modelsCountToPush, DirectX::XMMatrixIdentity());
	texOffset_.insert(texOffset_.end(), modelsCountToPush, { 0, 0 });

	// create a default material for this model
	materials_.insert(materials_.end(), modelsCountToPush, Material());

	relatedToVertexBufferByIdx_.insert(relatedToVertexBufferByIdx_.end(), modelsCountToPush, 10000);

	return static_cast<uint32_t>(IDXs_.size()-1);
}

void ModelsStore::ShiftRightDataOfModels(
	const UINT shiftFactor,
	const UINT fromIdx,
	const UINT toIdx)
{
	// shift right data of all the data arrays of models by some factor (shiftFactor) 
	// starting from input index (fromIdx)

	std::shift_right(textIDs_.begin() + fromIdx, textIDs_.end(), shiftFactor);

	std::shift_right(positions_.begin() + fromIdx, positions_.end(), shiftFactor);
	std::shift_right(rotations_.begin() + fromIdx, rotations_.end(), shiftFactor);
	std::shift_right(scales_.begin() + fromIdx, scales_.end(), shiftFactor);

	std::shift_right(positionModificators_.begin() + fromIdx, positionModificators_.end(), shiftFactor);
	std::shift_right(rotationQuatModificators_.begin() + fromIdx, rotationQuatModificators_.end(), shiftFactor);
	std::shift_right(scaleModificators_.begin() + fromIdx, scaleModificators_.end(), shiftFactor);
	
	std::shift_right(worldMatrices_.begin() + fromIdx, worldMatrices_.end(), shiftFactor);
	std::shift_right(texTransform_.begin() + fromIdx, texTransform_.end(), shiftFactor);
	std::shift_right(texOffset_.begin() + fromIdx, texOffset_.end(), shiftFactor);
	
	std::shift_right(materials_.begin() + fromIdx, materials_.end(), shiftFactor);
	std::shift_right(relatedToVertexBufferByIdx_.begin() + fromIdx, relatedToVertexBufferByIdx_.end(), shiftFactor);
}




// *********************************************************************************
//                            PRIVATE RENDERING API
// *********************************************************************************
void ModelsStore::PrepareIAStageForRendering(
	ID3D11DeviceContext* pDeviceContext,
	const UINT vb_buffer_idx,                               // index of the vertex buffer
	const VertexBufferStorage::VertexBufferData & vbData,   // vertex buffer data
	const IndexBufferStorage::IndexBufferData & ibData)     // index buffer data
{
	const UINT offset = 0;

	// set what primitive topology we want to use to render this vertex buffer
	pDeviceContext->IASetPrimitiveTopology(usePrimTopologyForBuffer_[vb_buffer_idx]);

	pDeviceContext->IASetVertexBuffers(
		0,                                 // start slot
		1,                                 // num buffers
		&vbData.pBuffer_,                  // ppVertexBuffers
		&vbData.stride_,               // pStrides
		&offset);                          // pOffsets

	pDeviceContext->IASetIndexBuffer(
		ibData.pBuffer_,                    // pIndexBuffer
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT, // format of the indices
		0);                                // offset, in bytes



	return;
}
