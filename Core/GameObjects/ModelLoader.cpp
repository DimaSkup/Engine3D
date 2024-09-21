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

		const aiScene* pScene = importer.ReadFile(
			filePath,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded);

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

	return;
}




///////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

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
	const std::string & filePath)                     // full path to the model
{
	// arrays to fill with data
	rawMeshes.push_back({});
	MeshData& meshData = rawMeshes.back();

	try
	{
		

		meshData.name = pMesh->mName.C_Str();
		meshData.path = filePath;
		meshData.vertices.resize(pMesh->mNumVertices);

		// fill in arrays with vertices/indices data
		GetVerticesAndIndicesFromMesh(pMesh, meshData.vertices, meshData.indices);

		// do some math calculations with these vertices (for instance: computation of tangents/bitangents)
		ExecuteModelMathCalculations(meshData.vertices);

		// get material data of this mesh
		aiMaterial* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

		// read material colors for this mesh
		aiColor3D color[3];
		//float specPower;
		pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color[0]);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color[1]);
		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color[2]);
		//pMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, specPower);
	
		meshData.material.SetAmbient({ color[0].r, color[0].g, color[0].b, 1.0f });
		meshData.material.SetDiffuse({ color[1].r, color[1].g, color[1].b, 1.0f });
		meshData.material.SetSpecular({ color[2].r, color[2].g, color[2].b, 1.0f });
		meshData.material.SetSpecularPower(1.0f);

		// set all the textures of this mesh to default value
		const TexID unloadedTexID = TextureManager::Get()->GetIDByName("unloaded");
		meshData.texIDs.resize(TextureClass::TEXTURE_TYPE_COUNT, unloadedTexID);

		// load available textures for this mesh
		LoadMaterialTextures(
			pDevice,
			pMaterial,
			pScene,
			filePath,
			meshData.texIDs);
		
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
		//throw EngineException("can't create a mesh of model by path: " + filePath);
	}

	return;

}

///////////////////////////////////////////////////////////

void ModelLoader::LoadMaterialTextures(
	ID3D11Device* pDevice,
	aiMaterial* pMaterial,
	const aiScene* pScene,
	const std::string& filePath,
	std::vector<TexID>& outMatTextures)
{
	//
	// load all the available textures for this mesh by its material data
	//

	assert(((u32)std::ssize(outMatTextures) == TextureClass::TEXTURE_TYPE_COUNT) && "wrong size of the textures IDs arr");

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
	const std::string modelDirPath{ StringHelper::GetDirectoryFromPath(filePath) + '/' };
	

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
				outMatTextures[type] = id;

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
				outMatTextures[type] = id;

				break;
			}

			// load an embedded indexed compressed texture
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				const UINT index = GetIndexOfEmbeddedCompressedTexture(&path);
				const TexPath texPath = filePath + path.C_Str();

				// create a new embedded indexed texture object;
				TextureClass embeddedIndexedTexture = TextureClass(
					pDevice,
					texPath,
					(uint8_t*)(pScene->mTextures[index]->pcData),  // data of texture
					pScene->mTextures[index]->mWidth);             // size of texture
					

				// store this embedded texture into the texture manager
				// and setup the material with this new texture
				const TexID id = pTexMgr->Add(texPath, embeddedIndexedTexture);
				outMatTextures[type] = id;

				break;

			} // end switch/case
			}
		}
	}
}

///////////////////////////////////////////////////////////


#if 0

bool ModelLoader::ConvertModelFromFile(const std::string & modelType,
	const std::string & modelFilename)
{
	// generate full path to the INPUT/OUTPUT model's data files
	//std::string relativePathToModelsDir{ Settings::Get()->GetString("MODEL_DIR_PATH") };
	std::string fullPathToModelsDir{ PROJECT_DIR };
	std::string fullPathToModelInputDataFile{ fullPathToModelsDir + modelFilename + ".obj" };
	std::string fullPathToModelOutputDataFile{ fullPathToModelsDir + modelFilename + ".txt" };

	// make correction about slash symbols
	std::replace(fullPathToModelInputDataFile.begin(), fullPathToModelInputDataFile.end(), '\\', '/');
	std::replace(fullPathToModelOutputDataFile.begin(), fullPathToModelOutputDataFile.end(), '\\', '/');

	// check if we already have an OUTPUT data file for the model of such type
	std::ifstream fin(fullPathToModelOutputDataFile, std::ios::in | std::ios::binary);
	bool executeModelConvertation = executeModelConvertation = fin.fail();

	// try to convert the model
	if (executeModelConvertation)
	{
		// load a dll which has model convertation functionals
		UtilsForDLL utils(L"ModelConverterDLL.dll");

		// load an adress of the dll's function so we can call it later
		DLLPROC ImportModelFromFileFunc = utils.GetProcAddrFromDLL("ImportModelFromFile");

		if (ImportModelFromFileFunc == NULL)
		{
			throw EngineException("the function address is invalid");
		}

		// call the function for importing the model 
		(ImportModelFromFileFunc)(fullPathToModelInputDataFile.c_str(), fullPathToModelOutputDataFile.c_str());

	}
	
	return true;
}

#endif

///////////////////////////////////////////////////////////

void ModelLoader::GetVerticesAndIndicesFromMesh(const aiMesh* pMesh,
	std::vector<VERTEX> & verticesArr,
	std::vector<UINT> & indicesArr)
{
	//
	// fill in the arrays with vertices/indices data of the input mesh
	//

	// get vertices of this mesh
	for (UINT i = 0; i < pMesh->mNumVertices; i++)
	{
		// store vertex's coords
		verticesArr[i].position.x = pMesh->mVertices[i].x;
		verticesArr[i].position.y = pMesh->mVertices[i].y;
		verticesArr[i].position.z = pMesh->mVertices[i].z;

		// if we have some texture coords for this vertex store it as well
		if (pMesh->mTextureCoords[0])
		{
			verticesArr[i].texture.x = pMesh->mTextureCoords[0][i].x;
			verticesArr[i].texture.y = pMesh->mTextureCoords[0][i].y;
		}	
	}

	// if we have any precomputed normal vectors
	if (pMesh->mNormals != nullptr)
	{
		for (int i = 0; i < pMesh->mNumVertices; ++i)
		{
			// store vertex's normals
			verticesArr[i].normal.x = pMesh->mNormals[i].x;
			verticesArr[i].normal.y = pMesh->mNormals[i].y;
			verticesArr[i].normal.z = pMesh->mNormals[i].z;
		}
	}

	// get indices of this mesh
	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indicesArr.push_back(face.mIndices[j]);
	}
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
