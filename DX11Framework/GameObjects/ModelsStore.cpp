///////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      ModelsStore.cpp
// Description:   
//
// Created:       05.07.23
///////////////////////////////////////////////////////////////////////////////////////////////
#include "ModelsStore.h"

#include "ModelInitializer.h"

#include "../Engine/COMException.h"
#include "../Engine/log.h"
#include "TextureManagerClass.h"

#include "ModelTranslationHelpers.h"


///////////////////////////////////////////////////////////////////////////////////////////////
//               Helper structs to store parts of the transient data
///////////////////////////////////////////////////////////////////////////////////////////////
struct MovementDataForModelsToUpdate
{
	DirectX::XMVECTOR position_;
	const DirectX::XMVECTOR direction_;
	const float speed_;
};

struct Mesh::MeshStoreTransientData
{
	// stores one frame transient data;
	// This is intermediate data used by the update pipeline every frame and discarded 
	// at the end of the frame

	std::vector<UINT> modelsToUpdate_;
	std::vector<DirectX::XMVECTOR> directionsToUpdate_;
	std::vector<float> velocitiesToUpdate_;
	std::vector<MovementDataForModelsToUpdate> movementDataForModelsToUpdate_;

	void Clear()
	{
		modelsToUpdate_.clear();
		directionsToUpdate_.clear();
		velocitiesToUpdate_.clear();
		movementDataForModelsToUpdate_.clear();
	}
};



///////////////////////////////////////////////////////////////////////////////////////////////
//                            PUBLIC MODIFICATION API
///////////////////////////////////////////////////////////////////////////////////////////////

ModelsStore::ModelsStore()
	: numOfModels_(0)
	, meshesTransientData_(std::make_unique<Mesh::MeshStoreTransientData>())
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
	IDs_.push_back(index);
	positions_.push_back(inPosition);
	rotations_.push_back(inDirection);
	worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(positions_[index]) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotations_[index]));
	velocities_.push_back(0.0f);         // speed value
	++numOfModels_;

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
	IDs_.push_back(index);
	positions_.push_back(inPosition);
	rotations_.push_back(inDirection);
	worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(positions_[index]) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotations_[index]));
	velocities_.push_back(0.0f);         // speed value
	++numOfModels_;

	try
	{
		vertexBuffers_.push_back({});
		vertexBuffers_.back().Initialize(pDevice, verticesArr, false);

		indexBuffers_.push_back({});
		indexBuffers_.back().Initialize(pDevice, indicesArr);

		for (TextureClass & texture : texturesArr)
			textures_.push_back(TextureClass(texture));
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
	IDs_.push_back(index);
	positions_.push_back(inPosition);
	rotations_.push_back(inDirection);
	worldMatrices_.push_back(DirectX::XMMatrixTranslationFromVector(positions_[index]) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotations_[index]));
	velocities_.push_back(0.0f);         // speed value
	++numOfModels_;

	try
	{
		vertexBuffers_.push_back(vertexBuffer);
		indexBuffers_.push_back(indexBuffer);

		for (TextureClass & texture : texturesArr)
			textures_.push_back(std::move(texture));
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a new model");
	}

	// return the index of the created model
	return index;
}

///////////////////////////////////////////////////////////

void ModelsStore::SetTextureByIndex(const UINT index, 
	const std::string & texturePath, 
	aiTextureType type)
{
	// Create a new texture from the file ans set it 
	// into the textures array by particular index

	// create a new texture from the file or just get a ptr to a texture object by key (its path) if it is already exists 
	TextureClass* pOriginTexture = TextureManagerClass::Get()->GetTexturePtrByKey(texturePath);

	// check if the textures array size is less than the index if so we push this texture
	// at the end of the array;
	if (index >= textures_.size())
		textures_.push_back(TextureClass(*pOriginTexture));
	else
		textures_[index] = TextureClass(*pOriginTexture);   // set texture by index
}



///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////



void SelectModelsToUpdate(
	const ModelsStore & inStore,
	const UINT inNumOfModels,
	std::vector<UINT> & outModelsToUpdate)
{
	for (UINT idx = 0; idx < inNumOfModels; ++idx)
		outModelsToUpdate.push_back(idx);


	//for (UINT idx = inNumOfModels * 2 / 3; idx < inNumOfModels; ++idx)
	//	outModelsToUpdate.push_back(idx);
}

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

	assert(numOfModels > 0);

	// select models which will be updated for this frame
	std::vector<UINT> modelsToUpdate;
	SelectModelsToUpdate(*this, numOfModels, modelsToUpdate);  

	const UINT numOfModelsToUpdate = (UINT)modelsToUpdate.size();


	// -----------------------------------------------------------------------------------//
	
	// UPDATE MODELS POSITIONS/ROTATIONS DATA
	std::vector<DirectX::XMVECTOR> posModificators;  // position changes
	std::vector<DirectX::XMVECTOR> rotModificators;  // rotations changes
	std::vector<DirectX::XMVECTOR> positionsDataToUpdate;
	std::vector<DirectX::XMVECTOR> rotationsDataToUpdate;
	std::vector<DirectX::XMVECTOR> newPositionsData;
	std::vector<DirectX::XMVECTOR> newRotationsData;
	
	
	PrepareModificationVectors(modelsToUpdate, positionsModificators_, posModificators);
	PrepareModificationVectors(modelsToUpdate, rotationModificators_, rotModificators);

	PreparePositionsToUpdate(modelsToUpdate, positions_, positionsDataToUpdate);
	PrepareRotationsToUpdate(modelsToUpdate, rotations_, rotationsDataToUpdate);

	ComputePositions(numOfModelsToUpdate, positionsDataToUpdate, posModificators, newPositionsData);
	ComputeRotations(numOfModelsToUpdate, rotationsDataToUpdate, rotModificators, newRotationsData);

	ApplyPositions(modelsToUpdate, newPositionsData, positions_);
	ApplyRotations(modelsToUpdate, newRotationsData, rotations_);

	// clear the transient data since we already don't need it
	posModificators.clear();
	rotModificators.clear();
	positionsDataToUpdate.clear();
	rotationsDataToUpdate.clear();

	// -----------------------------------------------------------------------------------//
	
	
	// UPDATE WORLD MATRICES
	std::vector<DirectX::XMMATRIX> translationMatricesToUpdate(numOfModelsToUpdate);
	std::vector<DirectX::XMMATRIX> rotationMatricesToUpdate(numOfModelsToUpdate);
	std::vector<DirectX::XMMATRIX> worldMatricesToUpdate(numOfModelsToUpdate);       // write into the finish matrices data which then will be used to update the world matrices of models

	PrepareTranslationMatrices(numOfModelsToUpdate, newPositionsData, translationMatricesToUpdate);
	PrepareRotationMatrices(numOfModelsToUpdate, newRotationsData, rotationMatricesToUpdate);

	ComputeWorldMatricesToUpdate(
		numOfModelsToUpdate,
		translationMatricesToUpdate,
		rotationMatricesToUpdate, 
		worldMatricesToUpdate);

	ApplyWorldMatrices(modelsToUpdate, worldMatricesToUpdate, worldMatrices_);

	// clear the transient data since we already don't need it
	newPositionsData.clear();
	newRotationsData.clear();
	translationMatricesToUpdate.clear();
	rotationMatricesToUpdate.clear();
	worldMatricesToUpdate.clear();

	modelsToUpdate.clear();


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



		pointLightShader.Render(pDeviceContext,
			indexCounts[idx],
			worldMatrices_[idx],
			viewProj,
			lightsStore.pointLightsStore_,
			texturesSRVs[idx]);
	}
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
