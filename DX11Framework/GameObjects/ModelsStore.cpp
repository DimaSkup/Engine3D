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

///////////////////////////////////////////////////////////////////////////////////////////////
//               Helper structs to store parts of the transient data
///////////////////////////////////////////////////////////////////////////////////////////////
struct MovementDataForModelsToUpdate
{
	DirectX::XMVECTOR position_;
	const DirectX::XMVECTOR direction_;
	const float speed_;
};

struct MeshStoreTransientData
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

void ModelsStore::CreateModel(const uint64_t inID,
	                          const std::string & filePath,          // a path to the data file of this model
	                          const DirectX::XMVECTOR & inPosition,
	                          const DirectX::XMVECTOR & inDirection)
{
	// this function creates a new model, setups it, load its data from the data file or 
	// calls a function for manual generating of data for this model;
	//
	// then adds this model's data into the array of models data;


	// check input params
	COM_ERROR_IF_ZERO(filePath.size(), "the input filePath is empty");

	IDs_.push_back(inID);
	positions_.push_back(inPosition);
	directions_.push_back(inDirection);
	velocities_.push_back(0.0f);         // speed value
	++numOfModels_;

	// initialize a model's data
	// make an initializer object which will be used for initialization of this model from file
	std::unique_ptr<ModelInitializer> pModelInitializer = std::make_unique<ModelInitializer>();

	try
	{

		// initialize this model loading its data from the data file by filePath
		const bool result = pModelInitializer->InitializeFromFile(filePath);
		COM_ERROR_IF_FALSE(result, "can't initialize a model from file: " + filePath);

#if 0
		// compute the summary count of vertices and indices of all the meshes from this model
		for (const Mesh* pMesh : this->meshes_)
		{
			sumVertexCount_ += pMesh->GetVertexCount();
			sumIndicesCount_ += pMesh->GetIndexCount();
		}
#endif
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a model by ID: " + inID);
	}

	return;
}



///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void ModelsStore::RenderModels()
{
	// Put the vertex buffer data and index buffer data on the video card 
	// to prepare this data for rendering;
	// after that we call the shader rendering function through the model_to_shader mediator;

	pDataContainer->world = this->GetWorldMatrix();
	pDataContainer->WVP = pDataContainer->world * pDataContainer->viewProj;
	pDataContainer->color = this->GetColor();

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
	
	

	return;
} // end Render

///////////////////////////////////////////////////////////

void Model::InitializeOneMesh(const std::vector<VERTEX> & verticesArr,
	const std::vector<UINT> & indicesArr,
	const std::map<std::string, aiTextureType> & texturesPaths,
	const bool isVertexBufferDynamic)
{
	// this function:
	//   1. initializes one mesh with vertices/indices data;
	//   2. creates a default grey texture for this mesh;
	//   3. and pushes this mesh into the meshes array of the model

	try
	{
		std::vector<std::unique_ptr<TextureClass>> texturesArr;   // an array for textures

		// if we have some path to the texture for this mesh
		if (!texturesPaths.empty())
		{
			// go through each path and init a texture object
			for (const auto & texture: texturesPaths)
			{
				// texture.first -- name;   texture.second -- type
				std::unique_ptr<TextureClass> pTexture = std::make_unique<TextureClass>(this->pDevice_, texture.first, texture.second);
				texturesArr.push_back(std::move(pTexture));
			}
		}
		// we have no path so create a default grey texture for this mesh
		else
		{
			std::unique_ptr<TextureClass> pTexture = std::make_unique<TextureClass>(this->pDevice_, Colors::UnloadedTextureColor, aiTextureType_DIFFUSE);
			texturesArr.push_back(std::move(pTexture));
		}

		// create a new mesh obj
		Mesh* pMesh = new Mesh(this->pDevice_, this->pDeviceContext_,
			verticesArr,
			indicesArr,
			texturesArr,
			DirectX::XMMatrixIdentity(), // we have no separate transformation for this mesh
			isVertexBufferDynamic);  

		// compute the number of vertices of this mesh and add it into the sum of all vertices of this model
		sumVertexCount_ += pMesh->GetVertexCount();
		sumIndicesCount_ += pMesh->GetIndexCount();

		// and push this mesh into the meshes array of the model
		this->meshes_.push_back(pMesh);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't create a mesh object");
	}

	return;

} // end InitializeOneMesh

///////////////////////////////////////////////////////////

UINT Model::GetVertexCount() const
{
	// returns a sum of all vertices counts of all the meshes of this model
	return sumVertexCount_;
}

UINT Model::GetIndexCount() const
{
	// returns a sum of all indices counts of all the meshes
	return sumIndicesCount_;
}

///////////////////////////////////////////////////////////

ID3D11Device* Model::GetDevice() const
{
	return this->pDevice_;
}

ID3D11DeviceContext* Model::GetDeviceContext() const
{
	return this->pDeviceContext_;
}

///////////////////////////////////////////////////////////

Mesh* Model::GetMeshByIndex(const UINT index) const
{
	// this function returns a mesh by index from the model's meshes array
	return this->meshes_[index];
}

const std::vector<Mesh*> & Model::GetMeshesArray() const
{
	// returns an array of pointers to the model's meshes
	return meshes_;
}