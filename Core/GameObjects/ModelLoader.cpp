////////////////////////////////////////////////////////////////////
// Filename:      ModelLoader.cpp
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       07.07.23
////////////////////////////////////////////////////////////////////
#include "../GameObjects/ModelLoader.h"
#include "../GameObjects/ModelMath.h"
#include "../GameObjects/TextureManagerClass.h"

#include "../GameObjects/ModelLoaderHelpers.h"


using namespace DirectX;
using namespace Mesh;


ModelLoader::ModelLoader()
{
}


void ModelLoader::LoadFromFile(ID3D11Device* pDevice,
	std::vector<MeshData>& rawMeshes,
	const std::string & filePath)
{
	// this function initializes a new model from the file 
	// of type .blend, .fbx, .3ds, .obj, etc.

	ASSERT_TRUE(!filePath.empty(), "the input filePath is empty");

	try
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded);

		// assert that we successfully read the data file 
		ASSERT_NOT_NULLPTR(pScene, "can't read a model's data file: " + filePath);

		// load all the meshes/materials/textures of this model
		ProcessNode(pDevice, 
			rawMeshes,
			pScene->mRootNode, 
			pScene, 
			DirectX::XMMatrixIdentity(), 
			filePath);
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		ASSERT_TRUE(false, "can't initialize a model from the file: " + filePath);
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

	//XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&pNode->mTransformation.a1)) * parentTransformMatrix;
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

	return;
}

///////////////////////////////////////////////////////////

void ModelLoader::ProcessMesh(ID3D11Device* pDevice,
	std::vector<MeshData>& rawMeshes,
	aiMesh* pMesh,                                    // the current mesh of the model
	const aiScene* pScene,                            // a ptr to the scene of this model type
	const DirectX::XMMATRIX & transformMatrix,        // a matrix which is used to transform position of this mesh to the proper location
	const std::string & filePath)                     // full path to the model
{
	try
	{
		// arrays to fill with data
		MeshData meshData;
		meshData.name = pMesh->mName.C_Str();
		meshData.vertices.resize(pMesh->mNumVertices);

		// fill in arrays with vertices/indices data
		GetVerticesAndIndicesFromMesh(pMesh, meshData.vertices, meshData.indices);

		// do some math calculations with these vertices
		ExecuteModelMathCalculations(meshData.vertices);

		// create textures objects (array of it) by material data of this mesh
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];

		// read material colors for this mesh
		aiColor3D color[3];
		material->Get(AI_MATKEY_COLOR_AMBIENT, color[0]);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color[1]);
		material->Get(AI_MATKEY_COLOR_SPECULAR, color[2]);
	
		meshData.material.SetAmbient(color[0]);
		meshData.material.SetDiffuse(color[1]);
		meshData.material.SetSpecular(color[2]);
		
		// load diffuse/normal textures for this mesh
		LoadMaterialTextures(meshData.textures, pDevice, material, aiTextureType::aiTextureType_DIFFUSE, pScene, filePath);
		LoadMaterialTextures(meshData.textures, pDevice, material, aiTextureType::aiTextureType_LIGHTMAP, pScene, filePath);
		LoadMaterialTextures(meshData.textures, pDevice, material, aiTextureType::aiTextureType_HEIGHT, pScene, filePath);
		LoadMaterialTextures(meshData.textures, pDevice, material, aiTextureType::aiTextureType_SPECULAR, pScene, filePath);
		// 
		//aiUVTransform bumpMapTexture;
		//material->Get(AI_MATKEY_TEXTURE_HEIGHT, bumpMapTexture);
		//LoadMaterialTextures(mesh.textures, pDevice, material, aiTextureType::aiTextureType_SPECULAR, pScene, filePath);

		// create a new raw mesh using the prepared data arrays
		rawMeshes.push_back(meshData);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't create a mesh obj");
	}

	return;

}

///////////////////////////////////////////////////////////

void ModelLoader::LoadMaterialTextures(
	std::unordered_map<aiTextureType, TextureClass*> & materialTextures,
	ID3D11Device* pDevice,
	aiMaterial* pMaterial,
	aiTextureType textureType,
	const aiScene* pScene,
	const std::string & filePath)
{
	//
	// this function loads a texture by material data
	//

	const UINT textureCount = pMaterial->GetTextureCount(textureType);
	TextureStorageType storeType = TextureStorageType::Invalid;
	TextureManagerClass* pTextureManager = TextureManagerClass::Get();


	try
	{

	// ----------------------------------------------------------//

	// if there are no textures
	if (textureCount == 0)      	
	{
		storeType = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);

		switch (textureType)
		{
			// create a new DIFFUSE texture with a single color
			case aiTextureType_DIFFUSE:
			{
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())    // if color == black, just use unloaded_texture_color (grey)
				{
					TextureClass* pUnloadedTexture = pTextureManager->GetTextureByKey("unloaded_texture");
					materialTextures.insert_or_assign(aiTextureType_DIFFUSE, pUnloadedTexture);

					return;
				}

				const BYTE red   = (BYTE)(aiColor.r * 255.0f);
				const BYTE green = (BYTE)(aiColor.g * 255.0f);
				const BYTE blue  = (BYTE)(aiColor.b * 255.0f);

				// create a new diffuse texture with some particular color
				TextureClass* colorTexture = pTextureManager->CreateTextureWithColor(Color(red, green, blue), aiTextureType_DIFFUSE);

				// add new diffuse texture with a single color
				materialTextures.insert_or_assign(aiTextureType_DIFFUSE, colorTexture);

				return;
			} 
			// create a new HEIGHT (BUMP MAP) texture with a single color
			case aiTextureType_HEIGHT:
			{
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())    // if color == black, just use unloaded_texture_color (grey)
				{
					TextureClass* pUnloadedTexture = pTextureManager->GetTextureByKey("unloaded_texture");
					materialTextures.insert_or_assign(aiTextureType_HEIGHT, pUnloadedTexture);

					return;
				}

				const BYTE red = (BYTE)(aiColor.r * 255.0f);
				const BYTE green = (BYTE)(aiColor.g * 255.0f);
				const BYTE blue = (BYTE)(aiColor.b * 255.0f);

				// create a new diffuse texture with some particular color
				TextureClass* colorTexture = pTextureManager->CreateTextureWithColor(Color(red, green, blue), aiTextureType_HEIGHT);

				// add new diffuse texture with a single color
				materialTextures.insert_or_assign(aiTextureType_HEIGHT, colorTexture);

				return;
			}
			// create a new NORMALS (BUMP MAP) texture with a single color
			case aiTextureType_SPECULAR:
			{
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())    // if color == black, just use unloaded_texture_color (grey)
				{
					TextureClass* pUnloadedTexture = pTextureManager->GetTextureByKey("unloaded_texture");
					materialTextures.insert_or_assign(aiTextureType_SPECULAR, pUnloadedTexture);

					return;
				}

				const BYTE red = (BYTE)(aiColor.r * 255.0f);
				const BYTE green = (BYTE)(aiColor.g * 255.0f);
				const BYTE blue = (BYTE)(aiColor.b * 255.0f);

				// create a new diffuse texture with some particular color
				TextureClass* colorTexture = pTextureManager->CreateTextureWithColor(Color(red, green, blue), aiTextureType_SPECULAR);

				// add new diffuse texture with a single color
				materialTextures.insert_or_assign(aiTextureType_SPECULAR, colorTexture);

				return;
			}
		} 
	} 


	// ----------------------------------------------------------//

	// we have some texture(s)
	else   
	{
		// go through each texture for this material
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);

			// determine what the texture storage type is
			TextureStorageType storeType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);

			switch (storeType)
			{
				// load a texture which is located on the disk
				case TextureStorageType::Disk:
				{
					// get path to the directory which contains a model's data file
					const std::string modelDirPath { StringHelper::GetDirectoryFromPath(filePath) };
					const std::string texturePath { modelDirPath + '/' + path.C_Str() };

					// get a ptr to the texture from the textures manager
					TextureClass* pTexture = pTextureManager->GetTextureByKey(texturePath);

					// make a pair ['texture_type' => 'ptr_to_texture']
					materialTextures.insert_or_assign(textureType, pTexture);

					break;
				}

				// load an embedded compressed texture
				case TextureStorageType::EmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());

					// create a new embedded texture object
					TextureClass embeddedTexture = TextureClass(pDevice,
						path.C_Str(),
						reinterpret_cast<uint8_t*>(pTexture->pcData),  // data of texture
						pTexture->mWidth,                              // size of texture
						textureType);

					TextureClass* pEmbeddedTexture =  pTextureManager->AddTextureByKey(path.C_Str(), embeddedTexture);

					// make a pair ['texture_type' => 'ptr_to_texture']
					materialTextures.insert_or_assign(textureType, pEmbeddedTexture);

					break;
				}

				// load an embedded indexed compressed texture
				case TextureStorageType::EmbeddedIndexCompressed:
				{
					int index = GetIndexOfEmbeddedCompressedTexture(&path);

					// create a new embedded indexed texture object;
					TextureClass embeddedIndexedTexture = TextureClass(
						pDevice,
						path.C_Str(),
						reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),  // data of texture
						pScene->mTextures[index]->mWidth,                              // size of texture
						textureType);

					TextureClass* pTexture = pTextureManager->AddTextureByKey(path.C_Str(), embeddedIndexedTexture);

					// make a pair ['texture_type' => 'ptr_to_texture']
					materialTextures.insert_or_assign(textureType, pTexture);
					//materialTextures.insert_or_assign(aiTextureType_DIFFUSE, pTexture);

					break;
				}
			}
		}
	} 

	if (materialTextures.size() == 0)
	{
		// create a new unhandled texture and push it into the textures array
		materialTextures.insert_or_assign(aiTextureType_DIFFUSE, pTextureManager->GetTextureByKey("unhandled_texture"));
	} 

	} // end try

	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't create a texture");
	}
	
	return;

}

///////////////////////////////////////////////////////////


#if 0

bool ModelLoader::ConvertModelFromFile(const std::string & modelType,
	const std::string & modelFilename)
{
	// generate full path to the INPUT/OUTPUT model's data files
	//std::string relativePathToModelsDir{ Settings::Get()->GetSettingStrByKey("MODEL_DIR_PATH") };
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
			ASSERT_TRUE(false, "the function address is invalid");
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

		// store vertex's normals
		verticesArr[i].normal.x = pMesh->mNormals[i].x;
		verticesArr[i].normal.y = pMesh->mNormals[i].y;
		verticesArr[i].normal.z = pMesh->mNormals[i].z;

		
	}

	// get indices of this mesh
	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indicesArr.push_back(face.mIndices[j]);
	}

	return;

} // end GetVerticesAndIndicesFromMesh

///////////////////////////////////////////////////////////

void ModelLoader::ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr)
{
	// is used for calculations of the model's normal vector, binormal, etc.
	ModelMath modelMath;

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	modelMath.CalculateModelVectors(verticesArr, false);
}
