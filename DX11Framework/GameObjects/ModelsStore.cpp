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

const UINT ModelsStore::CreateModel(ID3D11Device* pDevice,
	                          const std::string & filePath,          // a path to the data file of this model
	                          const DirectX::XMVECTOR & inPosition,
	                          const DirectX::XMVECTOR & inDirection)
{
	// this function creates a new model, setups it, load its data from the data file;
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(filePath.size(), "the input filePath is empty");

	const UINT index = (UINT)IDs_.size();

	try
	{
		// make an initializer object which is used for initialization of this model from file
		ModelInitializer modelInitializer;

		// initialize this model loading its data from the data file by filePath
		modelInitializer.InitializeFromFile(
			pDevice,
			filePath,
			vertexBuffers_,
			indexBuffers_,
			textures_);

		// fill in data arrays 
		IDs_.push_back(index);
		positions_.push_back(inPosition);
		rotations_.push_back(inDirection);
		worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(positions_[index]) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotations_[index]));
		velocities_.push_back(0.0f);                                      // speed value
		vertexCounts_.push_back(vertexBuffers_.back().GetVertexCount());  // we will use this value later to show how much vertices were rendered onto the screen

		++numOfModels_;

	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}

	return index;
}

///////////////////////////////////////////////////////////

const UINT ModelsStore::CreateModelWithData(ID3D11Device* pDevice,
	const DirectX::XMVECTOR & inPosition,
	const DirectX::XMVECTOR & inDirection,
	const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT>   & indicesArr,
	std::vector<TextureClass> & texturesArr)
{
	// this function creates a new model, setups it with the input data,
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(verticesArr.size(), "the input vertices array is empty");
	COM_ERROR_IF_ZERO(indicesArr.size(), "the input indices array is empty");
	COM_ERROR_IF_ZERO(texturesArr.size(), "the input textures array is empty");

	const UINT index = (UINT)IDs_.size();


	try
	{
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		IDs_.push_back(index);
		positions_.push_back(inPosition);
		rotations_.push_back(inDirection);
		worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(positions_[index]) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotations_[index]));
		velocities_.push_back(0.0f);         // speed value
		vertexCounts_.push_back(vertexBuffers_.back().GetVertexCount());  // we will use this value later to show how much vertices were rendered onto the screen


		for (TextureClass & texture : texturesArr)
			textures_.push_back(TextureClass(texture));

		++numOfModels_;
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
	std::vector<TextureClass> & texturesArr)
{
	// check input data
	assert(vertexBuffer.GetVertexCount() > 0);
	assert(indexBuffer.GetIndexCount() > 0);
	assert(texturesArr.size() > 0);

	const UINT index = (UINT)IDs_.size();

	try
	{
		vertexBuffers_.push_back(vertexBuffer);
		indexBuffers_.push_back(indexBuffer);

		for (TextureClass & texture : texturesArr)
			textures_.push_back(std::move(texture));

		IDs_.push_back(index);
		positions_.push_back(inPosition);
		rotations_.push_back(inDirection);
		worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(positions_[index]) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotations_[index]));
		velocities_.push_back(0.0f);         // speed value
		vertexCounts_.push_back(vertexBuffers_.back().GetVertexCount());  // we will use this value later to show how much vertices were rendered onto the screen


		++numOfModels_;
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
	const TextureClass & originTexture = TextureManagerClass::Get()->GetTextureByKey(texturePath);

	// check if the textures array size is less than the index if so we push this texture
	// at the end of the array;
	if (index >= textures_.size())
		textures_.push_back(TextureClass(originTexture));
	else
		textures_[index] = TextureClass(originTexture);   // set texture by index
}



///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void PrepareTexturesSRV_OfModelsToRender(
	const UINT numOfModels,
	const std::vector<TextureClass> & textures,
	std::vector<ID3D11ShaderResourceView* const*> & texturesSRVs)
{
	// allocate memory for the proper number of textures shader recource views
	//texturesSRVs.resize(numOfModels);

	assert(textures.size() == numOfModels);
	assert(texturesSRVs.size() == 0);

	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		texturesSRVs.push_back(textures[idx].GetTextureResourceViewAddress());
	}
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
} // end Render
