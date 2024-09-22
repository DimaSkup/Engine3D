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
	const std::string& filePath)                     // full path to the model
{
	// arrays to fill with data
	rawMeshes.push_back({});
	MeshData& meshData = rawMeshes.back();

	try
	{	
		// get name of the mesh or generate it if it is empty
		meshData.name = pMesh->mName.C_Str();
		meshData.name = (!meshData.name.empty()) ? 
			meshData.name : 
			filePath + "_no_name_" + std::to_string(rawMeshes.size() - 1);

		meshData.path = filePath;
		

		// fill in arrays with vertices/indices data
		GetVerticesAndIndicesFromMesh(pMesh, meshData.vertices, meshData.indices);

		// do some math calculations with these vertices (for instance: computation of tangents/bitangents)
		ExecuteModelMathCalculations(meshData.vertices);

		// get material data of this mesh
		aiMaterial* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

		// read material colors for this mesh
		aiColor4D ambientColor;
		aiColor4D diffuseColor;
		aiColor4D specularColor;
		float shininess;           
		
		pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		pMaterial->Get(AI_MATKEY_SHININESS, shininess);

		meshData.material.ambient_  = { ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a };
		meshData.material.diffuse_  = { diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a };
		meshData.material.specular_ = { specularColor.r, specularColor.g, specularColor.b, shininess };

		meshData.material.ambient_ = { 0.3f,0.3f,0.3f,1 };
		//meshData.material.diffuse_ = { 1,1,1,1 };
		meshData.material.specular_ = { 0,0,0,0 };


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

void ModelLoader::GetVerticesAndIndicesFromMesh(
	const aiMesh* pMesh,
	std::vector<VERTEX>& vertices,
	std::vector<UINT>& indices)
{
	//
	// fill in the arrays with vertices/indices data of the input mesh
	//

	using namespace DirectX;

	vertices.reserve(pMesh->mNumVertices);

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

	// if we have any precomputed normal vectors
	if (pMesh->mNormals != nullptr)
	{
		for (int i = 0; i < (int)pMesh->mNumVertices; ++i)
		{
			// store vertex's normals
			vertices[i].normal.x = pMesh->mNormals[i].x;
			vertices[i].normal.y = pMesh->mNormals[i].y;
			vertices[i].normal.z = pMesh->mNormals[i].z;
		}
	}

	// get indices of this mesh
	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
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
