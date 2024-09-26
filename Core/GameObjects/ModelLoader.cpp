////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.cpp
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       07.07.23
////////////////////////////////////////////////////////////////////
#include "../GameObjects/ModelLoader.h"
#include "../GameObjects/ModelMath.h"
#include "../GameObjects/TextureManager.h"
#include "../GameObjects/ModelLoaderHelpers.h"

#include "../Engine/EngineException.h"
#include "../Engine/log.h"
#include "../Common/Types.h"
#include "../Common/Assert.h"

#include "../Common/Utils.h"

#include <algorithm>                      // for using std::replace()


using namespace DirectX;
using namespace Mesh;



void ModelLoader::LoadFromFile(ID3D11Device* pDevice,
	std::vector<MeshData>& rawMeshes,
	const std::string & filePath)
{
	// this function initializes a new model from the file 
	// of type .blend, .fbx, .3ds, .obj, etc.

	Assert::NotEmpty(filePath.empty(), "the input filePath is empty");

	try
	{
		Assimp::Importer importer;
#if 0
		const aiScene* pScene = importer.ReadFile(
			filePath,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded);

#endif

		const aiScene* pScene = importer.ReadFile(
			filePath, 
			aiProcessPreset_TargetRealtime_MaxQuality |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals);

		// assert that we successfully read the data file 
		Assert::NotNullptr(pScene, "can't read a model's data file: " + filePath);

		// load all the meshes/materials/textures of this model
		ProcessNode(pDevice, 
			rawMeshes,
			pScene->mRootNode, 
			pScene, 
			DirectX::XMMatrixIdentity(), 
			filePath);

		importer.FreeScene();
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		throw EngineException("can't initialize a model from the file: " + filePath);
	}
}




// ********************************************************************************
//                              PRIVATE FUNCTIONS
// ********************************************************************************

void ModelLoader::ProcessNode(ID3D11Device* pDevice,
	std::vector<MeshData>& rawMeshes,
	aiNode* pNode, 
	const aiScene* pScene,
	const DirectX::XMMATRIX & parentTransformMatrix,  // a matrix which is used to transform position of this mesh to the proper location
	const std::string & filePath)                     // full path to the model
{
	//
	// this function goes through each node of the scene's tree structure
	// starting from the root node and initializes a mesh using data of this each node;
	//
	// created mesh is pushed into the input meshes array
	//


	const XMMATRIX nodeTransformMatrix = XMMATRIX(&pNode->mTransformation.a1) * parentTransformMatrix;

	// go through all the meshes in the current model's node
	for (UINT i = 0; i < pNode->mNumMeshes; i++)
	{
		// get the mesh
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];

		// handle this mesh and push it into the model's meshes array
		ProcessMesh(pDevice,
			rawMeshes,
			pMesh, 
			pScene, 
			nodeTransformMatrix, 
			filePath);
	}

	// go through all the child nodes of the current node and handle it
	for (UINT i = 0; i < pNode->mNumChildren; i++)
	{
		ProcessNode(pDevice, 
			rawMeshes,
			pNode->mChildren[i], 
			pScene, 
			nodeTransformMatrix, 
			filePath);
	}
}

///////////////////////////////////////////////////////////

void ModelLoader::ProcessMesh(ID3D11Device* pDevice,
	std::vector<MeshData>& rawMeshes,
	aiMesh* pMesh,                                    // the current mesh of the model
	const aiScene* pScene,                            // a ptr to the scene of this model type
	const DirectX::XMMATRIX & transformMatrix,        // a matrix which is used to transform position of this mesh to the proper location
	const std::string& filePath)                     // full path to the model
{
	// arrays to fill with data
	rawMeshes.push_back({});
	MeshData& meshData = rawMeshes.back();

	try
	{	
		meshData.path = filePath;

		SetMeshName(pMesh, rawMeshes);

		// fill in arrays with vertices/indices data
		GetVerticesAndIndicesFromMesh(pMesh, meshData);

		// do some math calculations with these vertices (for instance: computation of tangents/bitangents)
		ExecuteModelMathCalculations(meshData.vertices);

		// get material data of this mesh
		aiMaterial* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

		LoadMaterialColors(pMaterial, meshData.material);

		// load available textures for this mesh
		LoadMaterialTextures(
			pDevice,
			pMaterial,
			pScene,
			filePath,
			meshData);
		
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't create a mesh of model by path: " + filePath);
	}
	catch (EngineException& e)
	{
		Log::Error(e);

		// maybe we just can't load textures for this mesh so set them to default (unloaded)
		const TexID unloadedTexID = TextureManager::Get()->GetIDByName("unloaded");
		meshData.texIDs.resize(TextureClass::TEXTURE_TYPE_COUNT, unloadedTexID);
	}
}

///////////////////////////////////////////////////////////

void ModelLoader::SetMeshName(aiMesh* pMesh, std::vector<Mesh::MeshData>& meshes)
{
	// setup a name for the last added mesh

	MeshName name = pMesh->mName.C_Str();
	MeshData& mesh = meshes.back();

	// check for emptiness (if empty we generate a name for this mesh)
	name = (!name.empty()) ? name :	mesh.path + "_mesh_" + std::to_string(meshes.size());

	// check for duplication with names of other meshes of the current model
	for (MeshData& data : meshes)
	{
		if (data.name == name)
		{
			name += std::to_string(CoreUtils::GenID());
			break;
		}
	}

	mesh.name = name;
}

///////////////////////////////////////////////////////////

void ModelLoader::LoadMaterialColors(aiMaterial* pMaterial, Mesh::Material& mat)
{
	// read material colors for this mesh

	aiColor4D ambient;
	aiColor4D diffuse;
	aiColor4D specular;
	float shininess;

	aiReturn ret[4];

	ret[0] = pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	ret[1] = pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	ret[2] = pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	ret[3] = pMaterial->Get(AI_MATKEY_SHININESS, shininess);

	// aiReturn_SUCCESS == 0; so if for instance (ret[0] == 0) we use loaded color 
	// or default in another case
	mat.ambient_ = (!ret[0]) ? XMFLOAT4(ambient.r, ambient.g, ambient.b, ambient.a) : Mesh::defaultMaterialAmbient;
	mat.diffuse_ = (!ret[1]) ? XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, diffuse.a) : Mesh::defaultMaterialDiffuse;
	mat.specular_ = ((!ret[2]) & (!ret[3])) ? XMFLOAT4(specular.r, specular.g, specular.b, shininess) : Mesh::defaultMaterialSpecular;
}

///////////////////////////////////////////////////////////

void ModelLoader::LoadMaterialTextures(
	ID3D11Device* pDevice,
	aiMaterial* pMaterial,
	const aiScene* pScene,
	const std::string& filePath,
	MeshData& meshData)
{
	//
	// load all the available textures for this mesh by its material data
	//
	
	// set all the textures of this mesh material to default value
	const TexID unloadedTexID = TextureManager::Get()->GetIDByName("unloaded");
	meshData.texIDs.resize(TextureClass::TEXTURE_TYPE_COUNT, unloadedTexID);


	TextureManager* pTexMgr = TextureManager::Get();
	std::vector<aiTextureType> texTypesToLoad;
	std::vector<UINT> texCounts;


	// define what texture types to load
	for (u32 texTypeIdx = 1; texTypeIdx < TextureClass::TEXTURE_TYPE_COUNT; ++texTypeIdx)
	{
		aiTextureType type = (aiTextureType)texTypeIdx;

		// if there are some textures by this type
		if (UINT texCount = pMaterial->GetTextureCount(type))
		{
			texTypesToLoad.push_back(type);
			texCounts.push_back(texCount);
		}
	}

	// get path to the directory which contains a model's data file
	const std::string modelDirPath{ StringHelper::GetDirPath(filePath) + '/' };
	

	// go through available texture type and load responsible texture
	for (size idx = 0; idx < std::ssize(texTypesToLoad); ++idx)
	{
		const aiTextureType type = texTypesToLoad[idx];
		const UINT texCount = texCounts[idx];
		TextureStorageType storeType = TextureStorageType::Invalid;

		// go through each texture of this aiTextureType for this aiMaterial
		for (UINT i = 0; i < texCount; i++)
		{
			// get path to the texture file
			aiString path;
			pMaterial->GetTexture(type, i, &path);

			// determine what the texture storage type is
			const TextureStorageType storeType = DetermineTextureStorageType(pScene, pMaterial, i, type);

			switch (storeType)
			{

			// load a texture which is located on the disk
			case TextureStorageType::Disk:
			{
				// load a texture by path and setup the material with this texture
				const TexID id = pTexMgr->LoadFromFile(modelDirPath + path.C_Str());
				meshData.texIDs[type] = id;

				break;
			}

			// load an embedded compressed texture
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture* pAiTexture = pScene->GetEmbeddedTexture(path.C_Str());
				const TexPath texPath = filePath + path.C_Str();

				// create a new embedded texture object
				TextureClass embeddedTexture = TextureClass(
					pDevice,
					texPath,
					(uint8_t*)(pAiTexture->pcData),          // data of texture
					pAiTexture->mWidth);                     // size of texture
					

				// store this embedded texture into the texture manager
				// and setup the material with this new texture
				const TexID id = pTexMgr->Add(texPath, embeddedTexture);
				meshData.texIDs[type] = id;

				break;
			}

			// load an embedded indexed compressed texture
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				const UINT index = GetIndexOfEmbeddedCompressedTexture(&path);
				//const std::string dirName = StringHelper::GetDirPath(filePath);
				const std::string fileName = StringHelper::GetFileName(filePath);

				const TexPath texName = fileName + "_" + meshData.name + "_" + namesOfTexTypes[type] + "_" + path.C_Str()[1];

				// create a new embedded indexed texture object;
				TextureClass embeddedIndexedTexture = TextureClass(
					pDevice,
					texName,
					(uint8_t*)(pScene->mTextures[index]->pcData),  // data of texture
					pScene->mTextures[index]->mWidth);             // size of texture
					

				// store this embedded texture into the texture manager
				// and setup the material with this new texture
				const TexID id = pTexMgr->Add(texName, embeddedIndexedTexture);
				meshData.texIDs[type] = id;

				break;

			} // end switch/case
			}
		}
	}
}

///////////////////////////////////////////////////////////

void ModelLoader::GetVerticesAndIndicesFromMesh(
	const aiMesh* pMesh,
	Mesh::MeshData& meshData)
{
	//
	// fill in the arrays with vertices/indices data of the input mesh
	//

	using namespace DirectX;

	std::vector<VERTEX>& vertices = meshData.vertices;
	std::vector<UINT>& indices = meshData.indices;

	vertices.reserve(pMesh->mNumVertices);
	indices.reserve(pMesh->mNumFaces * 3);

	// get vertices of this mesh
	for (UINT i = 0; i < pMesh->mNumVertices; i++)
	{
		aiVector3D& pos  = pMesh->mVertices[i];
		aiVector3D& tex  = pMesh->mTextureCoords[0][i];
		aiVector3D& norm = pMesh->mNormals[i];

		vertices.emplace_back(
			XMFLOAT3(pos.x, pos.y, pos.z),
			XMFLOAT2(tex.x, tex.y),
			XMFLOAT3(norm.x, norm.y, norm.z),
			XMFLOAT3(0,0,0),                    // tangent
			XMFLOAT3(0,0,0),                    // binormal
			PackedVector::XMCOLOR(1,1,1,1));             // ARGB color
	}

	// get indices of this mesh
	for (u32 i = 0; i < pMesh->mNumFaces; i++)
	{
		indices.push_back(pMesh->mFaces[i].mIndices[0]);
		indices.push_back(pMesh->mFaces[i].mIndices[1]);
		indices.push_back(pMesh->mFaces[i].mIndices[2]);
	}

	// ----------------------------------- 

	// compute the bounding box of the mesh
	XMVECTOR vMin{ FLT_MAX, FLT_MAX, FLT_MAX };
	XMVECTOR vMax{ FLT_MIN, FLT_MIN, FLT_MIN };

	for (int i = 0; i < pMesh->mNumVertices; ++i)
	{
		aiVector3D& pos = pMesh->mVertices[i];
		XMVECTOR P{pos.x, pos.y, pos.z};
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	// convert min/max representation to center and extents representation
	XMStoreFloat3(&meshData.AABB.Center,  0.5f * (vMin + vMax));
	XMStoreFloat3(&meshData.AABB.Extents, 0.5f * (vMax - vMin));
}

///////////////////////////////////////////////////////////

void ModelLoader::ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr)
{
	// is used for calculations of the model's normal vector, binormal, etc.
	ModelMath modelMath;

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	modelMath.CalculateModelVectors(verticesArr, false);
}
