////////////////////////////////////////////////////////////////////
// Filename:      ModelInitializer.cpp
// Description:   a concrete implementation of the ModelInitializerInterface;
// 
// Created:       07.07.23
////////////////////////////////////////////////////////////////////
#include "../GameObjects/ModelInitializer.h"
#include "../GameObjects/ModelMath.h"
#include "../GameObjects/TextureManagerClass.h"

using namespace DirectX;


ModelInitializer::ModelInitializer()
{
}


void ModelInitializer::InitializeFromFile(ID3D11Device* pDevice,
	const std::string & filePath,
	std::vector<VertexBuffer<VERTEX>> & vertexBuffers,
	std::vector<IndexBuffer>          & indexBuffers,
	std::vector<TextureClass>         & textures)
{
	// this function initializes a new model from the file 
	// of type .blend, .fbx, .3ds, .obj, etc.

	// check input params
	COM_ERROR_IF_ZERO(filePath.length(), "the input filePath is empty");

	try
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded);

		// assert that we successfully read the data file 
		COM_ERROR_IF_FALSE(pScene, "can't read a model's data file: " + filePath);

		// load all the meshes/materials/textures of this model
		this->ProcessNode(pDevice, 
			vertexBuffers,
			indexBuffers,
			textures,
			pScene->mRootNode, 
			pScene, 
			DirectX::XMMatrixIdentity(), 
			filePath);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize a model from the file: " + filePath);
	}

	return;

} // end InitializeFromFile




///////////////////////////////////////////////////////////////////////////////////////////
//                              PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

void ModelInitializer::ProcessNode(ID3D11Device* pDevice,
	std::vector<VertexBuffer<VERTEX>> & vertexBuffers,
	std::vector<IndexBuffer>          & indexBuffers,
	std::vector<TextureClass>         & textures,
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
		this->ProcessMesh(pDevice,
			vertexBuffers,
			indexBuffers,
			textures, 
			pMesh, 
			pScene, 
			nodeTransformMatrix, 
			filePath);
	}

	// go through all the child nodes of the current node and handle it
	for (UINT i = 0; i < pNode->mNumChildren; i++)
	{
		this->ProcessNode(pDevice, 
			vertexBuffers,
			indexBuffers,
			textures, 
			pNode->mChildren[i], 
			pScene, 
			nodeTransformMatrix, 
			filePath);
	}

	return;
} // end Process Node

///////////////////////////////////////////////////////////

void ModelInitializer::ProcessMesh(ID3D11Device* pDevice,
	std::vector<VertexBuffer<VERTEX>> & vertexBuffers,
	std::vector<IndexBuffer>          & indexBuffers,
	std::vector<TextureClass>         & textures,
	aiMesh* pMesh,                                    // the current mesh of the model
	const aiScene* pScene,                            // a ptr to the scene of this model type
	const DirectX::XMMATRIX & transformMatrix,        // a matrix which is used to transform position of this mesh to the proper location
	const std::string & filePath)                     // full path to the model
{
	try
	{
		// arrays to fill with data
		std::vector<VERTEX> verticesArr(pMesh->mNumVertices);
		std::vector<UINT> indicesArr;

		// fill in arrays with vertices/indices data
		GetVerticesAndIndicesFromMesh(pMesh, verticesArr, indicesArr);

		// do some math calculations with these vertices
		this->ExecuteModelMathCalculations(verticesArr);

		// create textures objects (array of it) by material data of this mesh
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
		std::vector<TextureClass> texturesArr;

		// load diffuse/normal textures for this mesh
		LoadMaterialTextures(texturesArr, pDevice, material, aiTextureType::aiTextureType_DIFFUSE, pScene, filePath);
		LoadMaterialTextures(texturesArr, pDevice, material, aiTextureType::aiTextureType_NORMALS, pScene, filePath);
		
		


		vertexBuffers.push_back({});
		vertexBuffers.back().Initialize(pDevice, verticesArr, false);

		indexBuffers.push_back({});
		indexBuffers.back().Initialize(pDevice, indicesArr);

		textures.push_back(texturesArr[0]);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't create a mesh obj");
	}

	return;

} // end ProcessMesh

///////////////////////////////////////////////////////////

void ModelInitializer::LoadMaterialTextures(
	std::vector<TextureClass> & materialTextures,
	ID3D11Device* pDevice,
	aiMaterial* pMaterial,
	aiTextureType textureType,
	const aiScene* pScene,
	const std::string & filePath)
{
	// this function loads a texture by material data


	TextureStorageType storeType = TextureStorageType::Invalid;
	UINT textureCount = pMaterial->GetTextureCount(textureType);

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
			// create a new diffuse texture with some particular color
			case aiTextureType_DIFFUSE:
			{
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())    // if color == black, just use unloaded_texture_color (grey)
				{
					materialTextures.push_back(TextureClass(pDevice, Colors::UnloadedTextureColor, aiTextureType_DIFFUSE));
					return;
				}

				const BYTE red   = (BYTE)(aiColor.r * 255.0f);
				const BYTE green = (BYTE)(aiColor.g * 255.0f);
				const BYTE blue  = (BYTE)(aiColor.b * 255.0f);

				// push a texture into the textures array
				materialTextures.push_back(TextureClass(pDevice, Color(red, green, blue), textureType));
				return;

				break;

			} // case
		} // switch
	} // if


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
					TextureClass* pOriginTexture = TextureManagerClass::Get()->GetTexturePtrByKey(texturePath);

					// create a copy of texture object by its ptr 
					// and push it into the textures array
					TextureClass texture = TextureClass(*pOriginTexture);

					texture.SetType(textureType);  // change a type to the proper one

					materialTextures.push_back(std::move(texture));
					break;
				}

				// load an embedded compressed texture
				case TextureStorageType::EmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());

					// create a new embedded texture object
					TextureClass embeddedTexture = TextureClass(pDevice,
						reinterpret_cast<uint8_t*>(pTexture->pcData),
						pTexture->mWidth,
						textureType);

					// move this new texture into the textures array
					materialTextures.push_back(std::move(embeddedTexture));
					break;
				}

				// load an embedded indexed compressed texture
				case TextureStorageType::EmbeddedIndexCompressed:
				{
					int index = GetTextureIndex(&path);

					// create a new embedded indexed texture object;
					TextureClass embeddedIndexedTexture = TextureClass(pDevice,
						reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
						pScene->mTextures[index]->mWidth,
						textureType);

					// move this new texture into the textures array
					materialTextures.push_back(std::move(embeddedIndexedTexture));
					break;
				}
			} // switch
		} // for
	} // else



	if (materialTextures.size() == 0)
	{
		// create a new unhandled texture and push it into the textures array
		materialTextures.push_back(TextureClass(pDevice, Colors::UnhandledTextureColor, aiTextureType_DIFFUSE));
	} // if

	} // end try
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR_IF_FALSE(false, "can't create a texture");
	}
	
	return;

} // end LoadMaterialTextures

///////////////////////////////////////////////////////////

TextureStorageType ModelInitializer::DetermineTextureStorageType(const aiScene* pScene,
	aiMaterial* pMaterial, 
	const UINT index,
	const aiTextureType textureType)
{
	// this function determines all the possible texture storage types

	if (pMaterial->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	// get path to the texture
	aiString path;
	pMaterial->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();

	// check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSES TEXTURES" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}

	// check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSES TEXTURES" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}

	// lastly check if texture is a filepath by check for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None;   // no texture exists

} // end DetermineTextureStorageType

///////////////////////////////////////////////////////////

UINT ModelInitializer::GetTextureIndex(aiString* pStr)
{
	// this function returns an index of the embedded compressed texture by path pStr

	assert(pStr->length >= 2);             // assert that path is "*0", "*1", or something like that
	return (UINT)atoi(&pStr->C_Str()[1]);  // return an index
}

#if 0

bool ModelInitializer::ConvertModelFromFile(const std::string & modelType,
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
			COM_ERROR_IF_FALSE(false, "the function address is invalid");
		}

		// call the function for importing the model 
		(ImportModelFromFileFunc)(fullPathToModelInputDataFile.c_str(), fullPathToModelOutputDataFile.c_str());

	}
	
	return true;
}

#endif

///////////////////////////////////////////////////////////

void ModelInitializer::GetVerticesAndIndicesFromMesh(const aiMesh* pMesh,
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

void ModelInitializer::ExecuteModelMathCalculations(std::vector<VERTEX> & verticesArr)
{
	// is used for calculations of the model's normal vector, binormal, etc.
	std::unique_ptr<ModelMath> pModelMath = std::make_unique<ModelMath>(); 

	// after the model data has been loaded we now call the CalculateModelVectors() to
	// calculate the tangent and binormal. It also recalculates the normal vector;
	pModelMath->CalculateModelVectors(verticesArr, false);
}
